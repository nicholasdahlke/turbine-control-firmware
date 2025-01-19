//
// Created by Nicholas Dahlke on 02.11.2023.
//

#include "controller.h"
#include <esp_log.h>
#include <driver/timer.h>
#include <string.h>
static const char *TAG = "Controller";

const linear_calib_t div1_calib = {31.53140f, -6.46155f};
const linear_calib_t div2_calib = {62.67711f, -12.85545f};
const linear_calib_t div3_calib = {1.0f, 0.0f};
const linear_calib_t current_sense_calib = {4.83048f, -0.94478};
const linear_calib_t load_calib = {21.98560f, -6.716618f};


static bool IRAM_ATTR windspeed_isr_callback(void * args)
{
    int pulses = 0;
    pcnt_unit_get_count(pcnt_unit_windspeed, &pulses);
    pulses_per_interval_windspeed = pulses;
    pcnt_unit_clear_count(pcnt_unit_windspeed);
    return false;
}


void init_controller()
{
    ESP_LOGI(TAG, "Initializing I2C Devices");
    ESP_ERROR_CHECK(i2cdev_init());

    memset(&mcp1, 0, sizeof(mcp1));
    memset(&mcp2, 0, sizeof(mcp2));
    memset(&adc1, 0, sizeof(adc1));
    memset(&adc2, 0, sizeof(adc2));


    // Init IO Expanders
    ESP_LOGI(TAG, "Initializing IO Expanders");
    ESP_ERROR_CHECK(mcp23008_init_desc(&mcp1, MCP1_ADDR, 0, SDA_GPIO, SCL_GPIO));
    ESP_ERROR_CHECK(mcp23008_init_desc(&mcp2, MCP2_ADDR, 0, SDA_GPIO, SCL_GPIO));

    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp1, FAN_GPIO, MCP23008_GPIO_OUTPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp1, DI1_GPIO, MCP23008_GPIO_INPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp1, DI2_GPIO, MCP23008_GPIO_INPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp1, DI3_GPIO, MCP23008_GPIO_INPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp1, DO4_GPIO, MCP23008_GPIO_OUTPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp1, DO3_GPIO, MCP23008_GPIO_OUTPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp1, DO5_GPIO, MCP23008_GPIO_OUTPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp1, DO6_GPIO, MCP23008_GPIO_OUTPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp2, DIVIDER1_GPIO, MCP23008_GPIO_OUTPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp2, DIVIDER2_GPIO, MCP23008_GPIO_OUTPUT));
    ESP_ERROR_CHECK(mcp23008_set_mode(&mcp2, DIVIDER3_GPIO, MCP23008_GPIO_OUTPUT));

    ESP_LOGI(TAG, "Initializing IO Expanders finished");

    // Init ESP GPIO
    ESP_ERROR_CHECK(gpio_set_direction(DO1_GPIO, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(DO2_GPIO, GPIO_MODE_OUTPUT));

    ESP_ERROR_CHECK(gpio_pulldown_dis(DO1_GPIO));
    ESP_ERROR_CHECK(gpio_pulldown_dis(DO2_GPIO));

    ESP_ERROR_CHECK(gpio_pullup_en(DO1_GPIO));
    ESP_ERROR_CHECK(gpio_pullup_en(DO2_GPIO));



    // Init ADCs and set calibration coefficients
    ESP_LOGI(TAG, "Initializing ADCs");
    ESP_ERROR_CHECK(ads111x_init_desc(&adc1, ADC1_ADDR, 0, SDA_GPIO, SCL_GPIO));
    ESP_ERROR_CHECK(ads111x_init_desc(&adc2, ADC2_ADDR, 0, SDA_GPIO, SCL_GPIO));

    ESP_ERROR_CHECK(ads111x_set_mode(&adc1, ADS111X_MODE_CONTINUOUS));
    ESP_ERROR_CHECK(ads111x_set_mode(&adc2, ADS111X_MODE_CONTINUOUS));

    ESP_ERROR_CHECK(ads111x_set_data_rate(&adc1, ADS111X_DATA_RATE_128));
    ESP_ERROR_CHECK(ads111x_set_data_rate(&adc2, ADS111X_DATA_RATE_128));

    adc_gain = ads111x_gain_values[ADS111X_GAIN_2V048];

    // Init DAC
//    ESP_ERROR_CHECK(dac_output_enable(LOAD_DAC_CHANNEL));
//    ESP_ERROR_CHECK(dac_output_voltage(LOAD_DAC_CHANNEL, 0));

    const dac_oneshot_config_t dac_config = {
	     .chan_id = LOAD_DAC_CHANNEL
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac_config, &current_dac_handle));
    ESP_ERROR_CHECK(dac_oneshot_output_voltage(current_dac_handle, 0));

    //Init pulse counter for quadrature decoding
    const pcnt_unit_config_t unit_config_encoder = {
            .high_limit = 30000,
            .low_limit = -30000
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config_encoder, &pcnt_unit_encoder));
    const pcnt_chan_config_t chan_config_encoder = {
            .edge_gpio_num = A_GPIO,
            .level_gpio_num = B_GPIO
    };
    pcnt_channel_handle_t pcnt_chan_encoder = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit_encoder, &chan_config_encoder, &pcnt_chan_encoder));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_encoder, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_encoder, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    const pcnt_glitch_filter_config_t filter_config = {
            .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit_encoder, &filter_config));
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit_encoder));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit_encoder));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit_encoder));

    //Init pulse counter for wind speed measurement
    const pcnt_unit_config_t unit_config_windspeed = {
            .high_limit = 32000,
            .low_limit = -1
    };
    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config_windspeed, &pcnt_unit_windspeed));
    const pcnt_chan_config_t chan_config_windspeed = {
            .edge_gpio_num = ANEM_GPIO,
            .level_gpio_num = -1
    };
    pcnt_channel_handle_t pcnt_chan_windspeed = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit_windspeed, &chan_config_windspeed, &pcnt_chan_windspeed));
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_windspeed, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit_windspeed, &filter_config));
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit_windspeed));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit_windspeed));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit_windspeed));

    //Init timers for angular velocity measurement
    const timer_config_t timer_config = {
            .divider = TIMER_DIVIDER,
            .counter_dir = TIMER_COUNT_UP,
            .counter_en = TIMER_PAUSE,
            .alarm_en = TIMER_ALARM_EN,
            .auto_reload = TIMER_AUTORELOAD_EN
    };
    ESP_ERROR_CHECK(timer_init(TIMER_GROUP_0, TIMER_0, &timer_config));
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, speed_interval_encoder * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, encoder_speed_isr_callback, 0, 0);
    timer_start(TIMER_GROUP_0, TIMER_0);

    //Init timer for wind speed measurement
    ESP_ERROR_CHECK(timer_init(TIMER_GROUP_1, TIMER_0, &timer_config));
    timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 0);
    timer_set_alarm_value(TIMER_GROUP_1, TIMER_0, 1.0 * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_1, TIMER_0);
    timer_isr_callback_add(TIMER_GROUP_1, TIMER_0, windspeed_isr_callback, 0, 0);
    timer_start(TIMER_GROUP_1, TIMER_0);

}



static int16_t get_voltage_adc_raw()
{
    int16_t raw = 0;
    if(ads111x_get_value(&adc2, &raw) != ESP_OK)
    {
        ESP_LOGE(TAG, "Error while reading voltage ADC value");
    }
    return raw;
}

static int16_t get_current_adc_raw()
{
    int16_t raw = 0;
    if(ads111x_get_value(&adc1, &raw) != ESP_OK)
    {
        ESP_LOGE(TAG, "Error while reading voltage ADC value");
    }
    return raw;
}

static void set_mos_current(float current, linear_calib_t calib)
{
    uint8_t dac_value = (current > 0) ? (current * calib.coefficient1) + calib.offset : 0;
    dac_oneshot_output_voltage(current_dac_handle, dac_value);
    ESP_LOGI(TAG, "Set dac output channel to %d", dac_value);
}

void set_load_current(float current_setpoint)
{
    set_mos_current(current_setpoint, load_calib);
    ESP_LOGI(TAG, "Set Load current to %f A", current_setpoint);
}

float get_current_adc(linear_calib_t calib)
{
    return ((adc_gain / ADS111X_MAX_VALUE * get_current_adc_raw())*calib.coefficient1) + calib.offset;
}

float get_voltage_adc(linear_calib_t calib)
{
    return ((adc_gain / ADS111X_MAX_VALUE * get_voltage_adc_raw())*calib.coefficient1) + calib.offset;
}

void set_fan_state(bool state)
{
    mcp23008_set_level(&mcp1, FAN_GPIO, state);
    ESP_LOGI(TAG, "Fan state: %d", state);
}

void set_divider_state(int divider)
{
    mcp23008_set_level(&mcp2, DIVIDER1_GPIO, false);
    mcp23008_set_level(&mcp2, DIVIDER2_GPIO, false);
    mcp23008_set_level(&mcp2, DIVIDER3_GPIO, false);

    uint8_t divider_pin;
    switch (divider)
    {
        case 1:
            divider_pin = DIVIDER1_GPIO;
            break;
        case 2:
            divider_pin = DIVIDER2_GPIO;
            break;
        case 3:
            divider_pin = DIVIDER3_GPIO;
            break;
        default:
            divider_pin = 5;
            break;
    }

    mcp23008_set_level(&mcp2, divider_pin, true);
    ESP_LOGI(TAG, "Set divider %d at GPIO %d, to %d", divider, divider_pin, true);
}

float get_angular_speed()
{
    if (pulses_per_interval_encoder == 0)
    {
        return 0.0f;
    }
    else
    {
        return (pulses_per_interval_encoder / speed_interval_encoder) * RADIAN_PER_PULSE;
    }
}

static bool IRAM_ATTR encoder_speed_isr_callback(void * args)
{
    int pulses = 0;
    pcnt_unit_get_count(pcnt_unit_encoder, &pulses);
    pulses_per_interval_encoder = pulses;
    pcnt_unit_clear_count(pcnt_unit_encoder);
    return false;
}

float get_wind_speed()
{
    int pulses;
    return 1.0f * pcnt_unit_get_count(pcnt_unit_windspeed, &pulses);
    if (pulses_per_interval_windspeed == 0)
    {
        return 0.0f;
    }
    else
    {
        return 1/pulses_per_interval_windspeed;
    }
}


bool get_gpio(uint8_t gpio_num)
{
    uint32_t out;
    mcp23008_get_level(&mcp1, gpio_num, &out);
    return out;
}

void set_gpio(uint8_t gpio_num, bool value)
{
    switch (gpio_num)
    {
        case DO1_GPIO:
        case DO2_GPIO:
            gpio_set_level(gpio_num, value);
            break;
        case DO3_GPIO:
        case DO4_GPIO:
        case DO5_GPIO:
        case DO6_GPIO:
            mcp23008_set_level(&mcp1, gpio_num, value);
            break;
    }
    ESP_LOGI(TAG, "Set GPIO %d to %d", gpio_num, value);
}
//End of file controller.c
