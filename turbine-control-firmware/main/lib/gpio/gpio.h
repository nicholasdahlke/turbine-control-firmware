/*********************************************************************/
/**
  * @file gpio.h
  * @brief Access to the turbine GPIOs
  * @author Nicholas Dahlke nicholas.dahlke@phaenovum.de
  *
  * @date 19.10.2023 - first Implementation
  ********************************************************************/

#pragma once
#include <inttypes.h>
#include <stddef.h>
#include "driver/gpio.h"

typedef enum
{
    DO1,
    DO2,
    DO3,
    DO4,
    DO5,
    DO6,
    DI1,
    DI2,
    DI3,
    FAN,
    REED_1,
    REED_2,
    REED_3
} gpio_id_t;

static gpio_num_t get_esp_gpio_num(gpio_id_t id);
static uint8_t get_mcp23008_address(gpio_id_t id);
static uint8_t get_mcp23008_gpio(gpio_id_t);