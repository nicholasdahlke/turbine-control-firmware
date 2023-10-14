//
// Created by Nicholas Dahlke on 12.10.2023.
//

#pragma once
#include <inttypes.h>

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

//End of file ads1113.h
