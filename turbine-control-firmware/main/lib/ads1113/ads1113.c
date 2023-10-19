/*********************************************************************/
/**
  * @file ads1113.c
  * @brief ESP32 Driver for the I2C based 16bit ADC ADS1113
  * @author Nicholas Dahlke nicholas.dahlke@phaenovum.de
  *
  * @date 14.10.2023 - first Implementation
  ********************************************************************/

#include "ads1113.h"
#include "driver/i2c.h"

/*****************************************************//**
 * @brief Timeout for all I2C operations
 *********************************************************/
#define TIMEOUT (1000 / portTICK_PERIOD_MS)

/*****************************************************//**
 *  @brief Write a value to a register
 *
 *  This functions writes a 16 bit value to a
 *  register of the MCP23008
 *
 *  @param ads1113_addr I2C Address of the ADS1113, set
 *                      at the hardware level.
 *  @param register_address Address of the register which
 *                          the data is written to.
 *  @param p_data Pointer to a 16 bit variable, where the
 *                data to be written is stored.
 *********************************************************/
static void ads1113_register_write(uint8_t ads1113_addr, uint8_t register_address, uint16_t * p_data)
{
    i2c_cmd_handle_t h_cmd = i2c_cmd_link_create();
    i2c_master_start(h_cmd);
    i2c_master_write_byte(h_cmd, (ads1113_addr<<1), true);
    i2c_master_write_byte(h_cmd, register_address, true);

    uint8_t front = (uint8_t)(0xff00&(*p_data))>>8; // Gets the first byte of a 16-bit integer
    uint8_t back = (uint8_t)(0x00ff&(*p_data));     // Gets the last byte of a 16-bit integer

    i2c_master_write_byte(h_cmd, front, true);
    i2c_master_write_byte(h_cmd, back, true);
    i2c_master_stop(h_cmd);
    i2c_master_cmd_begin(0, h_cmd, TIMEOUT);
    i2c_cmd_link_delete(h_cmd);
}


/*****************************************************//**
 *  @brief Read a value from a register
 *
 *  This functions reads a 16 bit value from a
 *  register of the ADS1113
 *
 *  @param ads1113_addr I2C Address of the ADS1113, set
 *                      at the hardware level.
 *  @param register_address Address of the register which
 *                          the p_data is read from.
 *  @param p_data Pointer to an 16 bit variable, where the
 *                p_data to be read is stored. Needs to
 *                remain valid while writing.
 *********************************************************/
static void ads1113_register_read(uint8_t ads1113_addr, uint8_t register_address, uint16_t *p_data)
{
    i2c_cmd_handle_t h_cmd = i2c_cmd_link_create();
    i2c_master_start(h_cmd);
    i2c_master_write_byte(h_cmd, (ads1113_addr<<1), true);
    i2c_master_write_byte(h_cmd, register_address, true);
    i2c_master_stop(h_cmd);

    i2c_master_start(h_cmd);
    i2c_master_write_byte(h_cmd, (ads1113_addr<<1)|1, true);
    uint8_t front = 0;
    uint8_t back = 0;
    i2c_master_read_byte(h_cmd, &front, true);
    i2c_master_read_byte(h_cmd, &back, true);
    i2c_master_stop(h_cmd);
    i2c_master_cmd_begin(0, h_cmd, TIMEOUT);
    i2c_cmd_link_delete(h_cmd);

    *p_data = back | (front<<8);

}