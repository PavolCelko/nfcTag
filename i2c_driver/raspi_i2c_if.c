/*
 * i2c.c
 *
 *  Created on: 18.9.2014
 *      Author: zil00068
 */


/*
 *  imupi.c: test i2c access to the Invensense MPU-9150 Gyro, Accelerometer, and Compass chip
 *
 *  To enable i2c dev:  sudo apt-get install i2c-tools libi2c-dev
 *  To load kernel mod: in "/etc/modprobe.d/raspi-blacklist.conf" comment "blacklist i2c-bcm2708"
 *                      in "/etc/modules" add 2 lines "i2c-dev" and "i2c-bcm2708"
 *                      sudo bash -c "echo options i2c_bcm2708 baudrate=400000 > /etc/modprobe.d/i2c.conf"
 *                      reboot
 *  Check i2c address:  sudo i2cdetect -y 1
 *  Access i2c as user: sudo usermod -G i2c pi
 *  Compile with:       gcc -o imupi imupi.c -lrt
 *  Run with:           ./imupi
 *
 *  JG, 1/1/2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <linux/i2c-dev.h>

#include "raspi_i2c_if.h"

#include "ntag_defines.h"
//#include "MPU-9150.h"

int             imupi_dev         = -1;

/*
 *  imupi_init: initialize the board registers.
 */
int NTAG_i2c_init( uint8_t i2c_slave_address, int *fd )  {

  /* Open I2C device */
  imupi_dev = open( IMUPI_DEVNAME, O_RDWR );
  fd = imupi_dev;
  if ( imupi_dev == -1 )
    return IMUPI_I2C_OPEN_ERROR;

  /* Initialize magnetometer */
  if ( ioctl( imupi_dev, I2C_SLAVE, i2c_slave_address >> 1 ) < 0 )
    return IMUPI_I2C_DEV_NOT_FOUND;

  /* Give some time to the chip to initialize */
  usleep( 100000 );

//  /* Test data reading */

  return IMUPI_NO_ERROR;
}

int NTAG_i2c_close( void )
{
	fclose( imupi_dev );
}

/*
 *  NTAG_i2c_write: linux i2c write - interface for linux i2c
 */
int NTAG_i2c_write(uint8_t *buffer, uint8_t length) {

  if ( imupi_dev == -1 )
    return IMUPI_INIT_ERROR;

  if ( ioctl( imupi_dev, I2C_SLAVE, buffer[0] >> 1 ) < 0 )
    return IMUPI_I2C_DEV_NOT_FOUND;

  write( imupi_dev, &buffer[1], length - 1);

  return IMUPI_NO_ERROR;
}


/*
 *  NTAG_i2c_read: linux i2c read - interface for linux i2c
 */
int NTAG_i2c_read(uint8_t *buffer, uint8_t length) {

  if ( imupi_dev == -1 )
    return IMUPI_INIT_ERROR;

  if ( ioctl( imupi_dev, I2C_SLAVE, buffer[0] >> 1 ) < 0 )
    return IMUPI_I2C_DEV_NOT_FOUND;

  read( imupi_dev, &buffer[1], length - 1 );

  return IMUPI_NO_ERROR;
}
