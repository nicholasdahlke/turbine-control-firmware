/*********************************************************************/
/**
  * @file mcp23008.c
  * @brief ESP32 Driver for the I2C based 8 channel IO Expander MCP23008
  * @author Nicholas Dahlke nicholas.dahlke@phaenovum.de
  *
  * @date 14.10.2023 - first Implementation
  ********************************************************************/
#include "mcp23008.h"
#include "driver/i2c.h"
#include "esp_log.h"


/*****************************************************//**
 * @brief Timeout for all I2C operations
 *********************************************************/
#define TIMEOUT (100 / portTICK_PERIOD_MS)

/*****************************************************//**
 * @brief MCP23008 register size in bytes
 *********************************************************/
#define MCP23008_REGISTER_SIZE 1

/*****************************************************//**
 *  @brief Reads the value of an input channel
 *
 *  The channel needs to be configured as an input first,
 *  using the mcp23008_configure_channel() function
 *
 *  @param mcp23008_addr I2C Address of the MCP23008, set
 *                      at the hardware level.
 *  @param gpio_number Hardware channel that is read.
 *
 *  @return Returns the value of the channel, if the
 *          channel is not inverted, 0=LOW and 1=HIGH
 *********************************************************/
bool mcp23008_read_channel(uint8_t mcp23008_addr, uint8_t gpio_number)
{
    return mcp23008_register_read_bit(mcp23008_addr, GPIO, gpio_number);
}

/*****************************************************//**
 *  @brief Set the value of an output channel
 *
 *  The channel needs to be configured as an output first,
 *  using the mcp23008_configure_channel() function
 *
 *  @param mcp23008_addr I2C Address of the MCP23008, set
 *                      at the hardware level.
 *  @param gpio_number Hardware channel that is set.
 *
 *  @param value Value, which the channel is set to.
 *               0=LOW and 1=HIGH
 *********************************************************/
void mcp23008_write_channel(uint8_t mcp23008_addr, uint8_t gpio_number, bool value)
{
    mcp23008_register_write_bit(mcp23008_addr, GPIO, gpio_number, value);
}

/*****************************************************//**
 *  @brief Configure a channel
 *
 *  This functions allows configuration of a channel,
 *  using the mcp23008_gpio_config_t as a parameters
 *
 *  @param mcp23008_addr I2C Address of the MCP23008, set
 *                      at the hardware level.
 *  @param mcp23008_gpio_config Configuration struct
 *********************************************************/
void mcp23008_configure_channel(uint8_t mcp23008_addr, const mcp23008_gpio_config_t mcp23008_gpio_config)
{
    uint8_t chan = mcp23008_gpio_config.channel;

    mcp23008_register_write_bit(mcp23008_addr, GPPU, chan, mcp23008_gpio_config.pull_up_en);
    mcp23008_register_write_bit(mcp23008_addr, IODIR, chan, (uint8_t)mcp23008_gpio_config.direction);
    mcp23008_register_write_bit(mcp23008_addr, IPOL, chan, mcp23008_gpio_config.invert_input_en);
    mcp23008_register_write_bit(mcp23008_addr, GPINTEN, chan, mcp23008_gpio_config.interrupt_on_change_en);
    mcp23008_register_write_bit(mcp23008_addr, DEFVAL, chan, mcp23008_gpio_config.interrupt_compare_defval);
    mcp23008_register_write_bit(mcp23008_addr, INTCON, chan, mcp23008_gpio_config.interrupt_type);
}

/*****************************************************//**
 *  @brief Configure the MCP23008
 *
 *  This function sets global configuration parameters
 *  using the mcp23008_device_config struct
 *
 *  @param mcp23008_addr I2C Address of the MCP23008, set
 *                      at the hardware level.
 *  @param mcp23008_device_config Configuration struct
 *********************************************************/
void mcp23008_configure_device(uint8_t mcp23008_addr, const mcp23008_device_config_t mcp23008_device_config)
{
    mcp23008_register_write(mcp23008_addr, IOCON, (uint8_t*)&mcp23008_device_config, sizeof(mcp23008_device_config));
}

/*****************************************************//**
 *  @brief Read a value from a register
 *
 *  This functions reads an 8 bit value from a
 *  register of the MCP23008
 *
 *  @param mcp23008_addr I2C Address of the MCP23008, set
 *                      at the hardware level.
 *  @param register_address Address of the register which
 *                          the p_data is read from.
 *  @param p_data Pointer to an 8 bit variable, where the
 *                p_data to be read is stored. Needs to
 *                remain valid while writing.
 *********************************************************/
void mcp23008_register_read(uint8_t mcp23008_addr, uint8_t register_address, uint8_t *p_data, size_t len)
{
    i2c_master_write_read_device(0, mcp23008_addr, &register_address, 1,  p_data, len, TIMEOUT);

}

/*****************************************************//**
 *  @brief Write a value to a register
 *
 *  This functions writes an 8 bit value to a
 *  register of the MCP23008
 *
 *  @param mcp23008_addr I2C Address of the MCP23008, set
 *                      at the hardware level.
 *  @param register_address Address of the register which
 *                          the p_data is written to.
 *  @param p_data Pointer to an 8 bit variable, where the
 *                p_data to be written is stored.
 *********************************************************/
static void mcp23008_register_write(uint8_t mcp23008_addr, uint8_t register_address, uint8_t *p_data, size_t len)
{
    uint8_t write_buf[2] = {register_address, *p_data};
    i2c_master_write_to_device(0, mcp23008_addr, write_buf, sizeof(write_buf), TIMEOUT);
}

/*****************************************************//**
 *  @brief Write a bit in a register at a specific position
 *
 *  This functions allows us to write a single bit in a
 *  register, without needing to think about the others
 *  @param mcp23008_addr I2C Address of the MCP23008, set
 *                      at the hardware level.
 *  @param register_address Address of the register,
 *                          the bit is written to.
 *  @param pos Defines the bit to be changed (with 0=LSB)
 *
 *  @param value The value which is written to the bit

 *********************************************************/
static void mcp23008_register_write_bit(uint8_t mcp23008_addr, uint8_t register_address, uint8_t pos, bool value)
{
    uint8_t mcp23008_reg_read = 0;
    mcp23008_register_read(mcp23008_addr, register_address, &mcp23008_reg_read,MCP23008_REGISTER_SIZE);

    uint8_t mcp23008_reg_write = mcp23008_reg_read | (1<<pos);

    if(mcp23008_reg_read != mcp23008_reg_write)
    {
        mcp23008_register_write(mcp23008_addr, register_address, &mcp23008_reg_write, MCP23008_REGISTER_SIZE);
    }
}

/*****************************************************//**
 *  @brief Read a bit from a register at a specific position
 *
 *  This functions allows us to read a single bit from a
 *  register, without needing to think about the others
 *  @param mcp23008_addr I2C Address of the MCP23008, set
 *                      at the hardware level.
 *  @param register_address Address of the register,
 *                          the bit is read from.
 *  @param pos Defines the bit to be read (with 0=LSB)
 *
 *  @return The value of the bit

 *********************************************************/
static bool mcp23008_register_read_bit(uint8_t mcp23008_addr, uint8_t register_address, uint8_t pos)
{
    uint8_t mcp23008_reg_read = 0;
    mcp23008_register_read(mcp23008_addr, register_address, &mcp23008_reg_read,MCP23008_REGISTER_SIZE);
    return ((1 << pos) & mcp23008_reg_read ) != 0;

}

// End of file mcp23008.c