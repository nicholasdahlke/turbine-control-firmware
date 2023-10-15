/*********************************************************************/
/**
  * @file ads1113.h
  * @brief ESP32 Driver for the I2C based 16bit ADC ADS1113
  * @author Nicholas Dahlke nicholas.dahlke@phaenovum.de
  *
  * @date 14.10.2023 - first Implementation
  ********************************************************************/

#pragma once
#include <inttypes.h>
#include <stddef.h>

typedef enum
{
    CONV = 0x00,
    CONF = 0x01,
    LOTH = 0x02,
    HITH = 0x03
} ads1113_register_t;

typedef struct
{
    uint8_t                   : 5;
    uint8_t data_rate         : 3;
    uint8_t mode              : 1;
    uint8_t                   : 6;
    uint8_t single_shot_start : 1;

} ads1113_device_config_t;



static void ads1113_register_write(uint8_t ads1113_addr, uint8_t register_address, uint16_t *p_data);

static void ads1113_register_read(uint8_t ads1113_addr, uint8_t register_address, uint16_t *p_data);


//End of file ads1113.h
