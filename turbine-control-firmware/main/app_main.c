#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "lib/mpc23008/mcp23008.h"
#include "lib/ads1113/ads1113.h"
#include <driver/i2c.h>

#define MCP1 0x20
#define MCP2 0x27

#define I2C_MASTER_FREQ_HZ 400000

void initialize()
{
    int i2c_master_num = 0;
    i2c_config_t i2c_config = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = GPIO_NUM_18,
            .sda_pullup_en = GPIO_PULLUP_DISABLE,
            .scl_io_num  = GPIO_NUM_19,
            .scl_pullup_en = GPIO_PULLUP_DISABLE,
            .master.clk_speed = I2C_MASTER_FREQ_HZ,
            .clk_flags = 0
    };
    i2c_param_config(i2c_master_num, &i2c_config);
    i2c_driver_install(i2c_master_num,  i2c_config.mode, 0, 0, 0);
}

void app_main(void)
{
    initialize();

    mcp23008_device_config_t mcp23008_config;
    mcp23008_config.interrupt_open_drain_en = 0;
    mcp23008_config.interrupt_polarity_en = 0;
    mcp23008_config.sda_slew_rate_control_en = 0;
    mcp23008_config.sequential_operation_en = 1;
    mcp23008_configure_device(MCP1, mcp23008_config);
    printf("configured device\n");


    mcp23008_gpio_config_t mcp23008_gpio_0_config;
    mcp23008_gpio_0_config.channel = 6;
    mcp23008_gpio_0_config.direction = GPIO_OUTPUT;
    mcp23008_gpio_0_config.interrupt_compare_defval = 0;
    mcp23008_gpio_0_config.interrupt_on_change_en = 0;
    mcp23008_gpio_0_config.interrupt_type = 0;
    mcp23008_gpio_0_config.invert_input_en = 0;
    mcp23008_gpio_0_config.pull_up_en = 0;
    uint8_t data = 0;
    mcp23008_configure_channel(MCP1, mcp23008_gpio_0_config);
    mcp23008_write_channel(MCP1, 6, 0);

}
