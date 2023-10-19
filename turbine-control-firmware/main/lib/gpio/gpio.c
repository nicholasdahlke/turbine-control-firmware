/*********************************************************************/
/**
  * @file gpio.c
  * @brief Access to the turbine GPIOs
  * @author Nicholas Dahlke nicholas.dahlke@phaenovum.de
  *
  * @date 19.10.2023 - first Implementation
  ********************************************************************/

#pragma once
#include "gpio.h"

static gpio_num_t get_esp_gpio_num(gpio_id_t id)
{
    gpio_num_t gpio_num;
    switch (id) {
        case DO1:
            gpio_num = GPIO_NUM_27;
            break;
        case DO2:
            gpio_num = GPIO_NUM_32;
            break;
        default:
            gpio_num = GPIO_NUM_NC;
    }
    return gpio_num;
}

static uint8_t get_mcp23008_address(gpio_id_t id)
{
    uint8_t address;
    switch (id) {
        case DO3:
        case DO4:
        case DO6:
        case DI1:
        case DI2:
        case DI3:
        case FAN:
            address = 0x20;
            break;
        case REED_1:
        case REED_2:
        case REED_3:
            address = 0x27;
            break;
        default:
            address = 0;
    }
    return address;
}