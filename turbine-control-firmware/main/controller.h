//
// Created by Nicholas Dahlke on 02.11.2023.
//

#pragma once
#include <mcp23008.h>
#include <ads111x.h>
#include <driver/gpio.h>
#include <driver/pulse_cnt.h>
#include <math.h>



#define MCP1_ADDR 0x20
#define MCP2_ADDR 0x27
#define ADC1_ADDR 0x49
#define ADC2_ADDR 0x48

#define SDA_GPIO GPIO_NUM_18
#define SCL_GPIO GPIO_NUM_19

#define FAN_GPIO 6
#define DI1_GPIO 3
#define DI2_GPIO 4
#define DI3_GPIO 5
#define DO4_GPIO 0
#define DO5_GPIO 1
#define DO6_GPIO 2
#define DO3_GPIO 7
#define DIVIDER1_GPIO 0
#define DIVIDER2_GPIO 1
#define DIVIDER3_GPIO 2
#define DO1_GPIO 27
#define DO2_GPIO 32
#define A_GPIO 23
#define B_GPIO 34
#define Z_GPIO 17
#define ANEM_GPIO 21

#define LOAD_DAC_CHANNEL DAC_CHAN_0

#define ENCODER_PULSES 1024
#define RADIAN_PER_PULSE (2*M_PI/ENCODER_PULSES)

#define TIMER_DIVIDER (16)
#define TIMER_SCALE (TIMER_CLK_FREQ / TIMER_DIVIDER) * 16


// IO Expander
static i2c_dev_t mcp1;
static i2c_dev_t mcp2;

static i2c_dev_t adc1;
static i2c_dev_t adc2;

//ADC
static float adc_gain = 0.0f;

//PCNT
static pcnt_unit_handle_t pcnt_unit_encoder = NULL;
static pcnt_unit_handle_t pcnt_unit_windspeed = NULL;
static const float speed_interval_encoder = 0.1f;
static volatile int pulses_per_interval_encoder = 0;
static volatile int pulses_per_interval_windspeed = 0;


typedef struct
{
    float coefficient1;
    float offset;
} linear_calib_t;

extern const linear_calib_t div1_calib;
extern const linear_calib_t div2_calib;
extern const linear_calib_t div3_calib;

extern const linear_calib_t current_sense_calib;
extern const linear_calib_t load_calib;


void init_controller();

void set_fan_state(bool state);

void set_divider_state(int divider);

static int16_t get_current_adc_raw();
static int16_t get_voltage_adc_raw();
float get_current_adc(linear_calib_t calib);
float get_voltage_adc(linear_calib_t calib);

static void set_mos_current(float current, linear_calib_t calib);
void set_load_current(float current_setpoint);

float get_angular_speed();
static bool encoder_speed_isr_callback(void * args);

float get_wind_speed();
static bool windspeed_isr_callback(void * args);

bool get_gpio(uint8_t gpio_num);
void set_gpio(uint8_t gpio_num, bool value);
//End of file controller.h
