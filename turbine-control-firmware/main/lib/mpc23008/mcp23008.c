//
// Created by Nicholas Dahlke on 12.10.2023.
//
#include "mcp23008.h"
#include "driver/i2c.h"
#define TIMEOUT (1000 / portTICK_PERIOD_MS)


bool mcp23008_read_channel(uint8_t mcp23008_addr, uint8_t gpio_number)
{
    uint8_t GPIO_reg = 0;
    mcp23008_register_read(mcp23008_addr, GPIO, &GPIO_reg, sizeof(GPIO_reg));
    return ((1 << gpio_number) & GPIO_reg ) != 0;
}

void mcp23008_write_channel(uint8_t mcp23008_addr, uint8_t gpio_number, bool value)
{
    uint8_t GPIO_reg_pre = 0;

    mcp23008_register_read(mcp23008_addr, GPIO, &GPIO_reg_pre, sizeof(GPIO_reg_pre));

    uint8_t GPIO_reg_after = (1 << gpio_number) | GPIO_reg_pre;

    if((GPIO_reg_pre | GPIO_reg_after) != GPIO_reg_pre)
    {
        mcp23008_register_write(mcp23008_addr, GPIO, &GPIO_reg_after, sizeof(GPIO_reg_after));
    }
}

void mcp23008_configure_channel(uint8_t mcp23008_addr, const mcp23008_gpio_config_t mcp23008_gpio_config)
{
    mcp23008_register_write(mcp23008_addr, IODIR, (uint8_t*)&mcp23008_gpio_config.direction, \
                        sizeof(mcp23008_gpio_config.direction));
    mcp23008_register_write(mcp23008_addr, IPOL, (uint8_t*)&mcp23008_gpio_config.invert_input_en, \
                        sizeof(mcp23008_gpio_config.invert_input_en));
    mcp23008_register_write(mcp23008_addr, GPINTEN, (uint8_t*)&mcp23008_gpio_config.interrupt_on_change_en, \
                        sizeof(mcp23008_gpio_config.interrupt_on_change_en));
    mcp23008_register_write(mcp23008_addr, DEFVAL, (uint8_t*)&mcp23008_gpio_config.interrupt_compare_defval, \
                        sizeof(mcp23008_gpio_config.interrupt_compare_defval));
    mcp23008_register_write(mcp23008_addr, INTCON, (uint8_t*)&mcp23008_gpio_config.interrupt_type, \
                        sizeof(mcp23008_gpio_config.interrupt_type));
    mcp23008_register_write(mcp23008_addr, GPPU, (uint8_t*)&mcp23008_gpio_config.pull_up_en, \
                        sizeof(mcp23008_gpio_config.direction));
}

static void mcp23008_configure_device(uint8_t mcp23008_addr, const mcp23008_device_config_t mcp23008_device_config)
{
    mcp23008_register_write(mcp23008_addr, IOCON, (uint8_t*)&mcp23008_device_config, sizeof(mcp23008_device_config));
}

static void mcp23008_register_read(uint8_t mcp23008_addr, uint8_t register_address, uint8_t *data, size_t len)
{
    i2c_master_read_from_device(0, mcp23008_addr, data, len, TIMEOUT);
}

static void mcp23008_register_write(uint8_t mcp23008_addr, uint8_t register_address, uint8_t *data, size_t len)
{
    i2c_master_write_to_device(0, mcp23008_addr, data, len, TIMEOUT);
}

// End of file mcp23008.c