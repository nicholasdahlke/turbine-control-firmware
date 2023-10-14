//
// Created by Nicholas Dahlke on 11.10.2023.
//

#pragma once
#include <stdbool.h>
#include <inttypes.h>
#include <stddef.h>

typedef enum
{
    IODIR = 0x00,
    IPOL = 0x01,
    GPINTEN = 0x02,
    DEFVAL = 0x03,
    INTCON = 0x04,
    IOCON = 0x05,
    GPPU = 0x06,
    INTF = 0x07,
    INTCAP = 0x08,
    GPIO = 0x09,
    OLAT = 0x0A

} mcp23008_register_t;

typedef struct
{  // Always write the whole register (provide a bitmask for every parameter)
    uint8_t direction;
    uint8_t invert_input_en;
    uint8_t interrupt_on_change_en;
    uint8_t interrupt_compare_defval;
    uint8_t interrupt_type;
    uint8_t pull_up_en;
} mcp23008_gpio_config_t;

typedef struct
{
    unsigned int                          : 1;
    unsigned int                          : 1;
    unsigned int sequential_operation_en  : 1;  // SEQOP
    unsigned int sda_slew_rate_control_en : 1;  // DISSLW
    unsigned int                          : 1;
    unsigned int interrupt_open_drain_en  : 1;  // ODR
    unsigned int interrupt_polarity_en    : 1;  // INTPOL
    unsigned int                          : 1;
} mcp23008_device_config_t;



bool mcp23008_read_channel(uint8_t mcp23008_addr, uint8_t gpio_number);
void mcp23008_write_channel(uint8_t mcp23008_addr, uint8_t gpio_number, bool value);
void mcp23008_configure_channels(uint8_t mcp23008_addr, const mcp23008_gpio_config_t mcp23008_gpio_config);

static void mcp23008_configure_device(uint8_t mcp23008_addr, const mcp23008_device_config_t mcp23008_device_config);
static void mcp23008_register_read(uint8_t mcp23008_addr, uint8_t register_address, uint8_t *data, size_t len);
static void mcp23008_register_write(uint8_t mcp23008_addr, uint8_t register_address, uint8_t *data, size_t len);



// End of file mcp23008.h