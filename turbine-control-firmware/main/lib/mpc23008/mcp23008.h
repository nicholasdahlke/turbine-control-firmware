/*********************************************************************/
/**
  * @file mcp23008.h
  * @brief ESP32 Driver for the I2C based 8 channel IO Expander MCP23008
  * @author Nicholas Dahlke nicholas.dahlke@phaenovum.de
  *
  * @date 14.10.2023 - first Implementation
  ********************************************************************/

#pragma once
#include <stdbool.h>
#include <inttypes.h>
#include <stddef.h>

/*****************************************************//**
 * @brief An enum containing all the MCP23008 registers
 *********************************************************/
typedef enum
{
    IODIR = 0x00, /**< I/O direction register */
    IPOL = 0x01, /**< Input polarity register */
    GPINTEN = 0x02, /**< Interrupt-on-change control register */
    DEFVAL = 0x03, /**< Default compare register for interrupt-on-change */
    INTCON = 0x04, /**< Interrupt control register */
    IOCON = 0x05, /**< Configuration register */
    GPPU = 0x06, /**< Pull-up resistor configuration register */
    INTF = 0x07, /**< Interrupt flag register */
    INTCAP = 0x08, /**< Interrupt capture register */
    GPIO = 0x09, /**< Port register */
    OLAT = 0x0A /**< Output Latch register */

} mcp23008_register_t;


/*****************************************************//**
 * @brief A structure used to configure a device channel
 *********************************************************/
typedef struct
{
    uint8_t channel; /**< The channel to be configured. */
    uint8_t direction; /**< Channel direction. 0=OUTPUT; 1=INPUT */
    uint8_t invert_input_en; /**< Apply a Logic NOT to the value. 0=No; 1=Yes */
    uint8_t interrupt_on_change_en; /**< Enable an interrupt on change. 0=No; 1=Yes */
    uint8_t interrupt_compare_defval;  /**< Logic Value to compare against if set via interrupt_type */
    uint8_t interrupt_type; /**< 0=Compare against prev. value for interrupt; 1=Compare against interrupt_compare_defval */
    uint8_t pull_up_en;  /**< Enable internal pullup. 0=No, 1=Yes */
} mcp23008_gpio_config_t;


/*****************************************************//**
 * @brief A structure used to configure the MCP23008
 *********************************************************/
typedef struct
{
    unsigned int                          : 2;
    unsigned int sequential_operation_en  : 1;  /**< 0=Enable sequential operation, Address pointer increments; 1=Disable sequential operation, Address pointer does not increment */
    unsigned int sda_slew_rate_control_en : 1;  /**< 0=Enable slew rate control on the SDA pin; 1=Disable slew rate control */
    unsigned int                          : 1;
    unsigned int interrupt_open_drain_en  : 1;  /**< 0=INT Ouput as active driver output, 1=Int Output as Open-drain output */
    unsigned int interrupt_polarity_en    : 1;  /**< Sets the polarity of the Int pin, 0=Active-low; 1=Active high */
    unsigned int                          : 1;
} mcp23008_device_config_t;



bool mcp23008_read_channel(uint8_t mcp23008_addr, uint8_t gpio_number);
void mcp23008_write_channel(uint8_t mcp23008_addr, uint8_t gpio_number, bool value);
void mcp23008_configure_channels(uint8_t mcp23008_addr, const mcp23008_gpio_config_t mcp23008_gpio_config);

void mcp23008_configure_device(uint8_t mcp23008_addr, const mcp23008_device_config_t mcp23008_device_config);
static void mcp23008_register_read(uint8_t mcp23008_addr, uint8_t register_address, uint8_t *p_data, size_t len);
static void mcp23008_register_write(uint8_t mcp23008_addr, uint8_t register_address, uint8_t *p_data, size_t len);
static void mcp23008_register_write_bit(uint8_t mcp23008_addr, uint8_t register_address, uint8_t pos, bool value);
static bool mcp23008_register_read_bit(uint8_t mcp23008_addr, uint8_t register_address, uint8_t pos);

// End of file mcp23008.h