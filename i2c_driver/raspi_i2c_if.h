/*
 * i2cNTAG.h
 *
 *  Created on: 18.9.2014
 *      Author: zil00068
 */

#ifndef I2CNTAG_H_
#define I2CNTAG_H_

/*
 *  Definitions for Invensense MPU-9150.
 */



#define IMUPI_DEVNAME             "/dev/i2c-1"

#define IMUPI_NO_ERROR            0
#define IMUPI_I2C_OPEN_ERROR      1
#define IMUPI_I2C_DEV_NOT_FOUND   2
#define IMUPI_I2C_WRITE_ERROR     3
#define IMUPI_I2C_READ_ERROR      4
#define IMUPI_INIT_ERROR          5

int NTAG_i2c_init( uint8_t i2c_slave_address, int *fd );
int NTAG_i2c_close( void );
int NTAG_i2c_write( uint8_t *buffer, uint8_t length );
int NTAG_i2c_read( uint8_t *buffer, uint8_t length );

#endif /* I2CNTAG_H_ */
