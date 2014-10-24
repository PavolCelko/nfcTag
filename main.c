#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <linux/i2c-dev.h>

#include <time.h>

#include "ntag_defines.h"
#include "ntag_mem_if.h"
#include "ntag_func_if.h"
#include "ntag_app_if.h"

#include "raspi_i2c_if.h"

#define NTAG_SLAVE_ADDRESS		0xAA
#define NTAG_I2C_BITRATE		100 /*or 370 or 400*/

#define IS_BETWEEN(x, a, b)	(x >= a && x <= b ? 1 : 0)
//---------------------------------------------------------------------
///
/// Initialize Buffer
/// @param buffer pointer to Buffer
/// @param buffer_len  length of Buffer
/// @param type how the Buffer shall be initialized
void Buffer_Init(uint8_t *buffer, uint32_t buf_len, uint8_t type)
{
  uint8_t i;

  if (type)
  {
    for (i = 0; i < buf_len; i++)
    {
      buffer[i] = i;
    }
  }
  else
  {
    for (i = 0; i < buf_len; i++)
    {
      buffer[i] = 0;
    }
  }
}


int16_t eraseEEPROM()
{
	int16_t err;
	uint8_t block;
	uint8_t *txBuff[16];
	memset(txBuff, 0x00, 16);

	for(block = NTAG_MEM_BLOCK_START_ADDR_USER_MEMORY; block <= 0x78; block++)
	{
		err = ntagWriteData(block, txBuff, 16);
	}
	return err;
}

//---------------------------------------------------------------------
///
/// main Programm
/// @return should never return
int main(void)
{
  uint16_t i, counter, length;

  uint8_t ntag_i2c_address;

  int err = -1;

  int i2c_fd = -1;

  uint8_t ntag_eeprom_block_address;

  const uint8_t RECcarString[21] = {'H','e','l','l','o','!',' ','I',' ','a','m',' ','R','E','C',' ','c','a','r','.','\0'};

  uint8_t readBuff[16];
  uint8_t writeBuff[16];

  char commandLineString[30];
  char string[30];

//  main buffer used to read and write user memory
//  Buffer_Init(readBuff, sizeof(readBuff), 0);
//
//  err = eraseEEPROM();
//
//  memcpy(writeBuff, RECcarString, sizeof(RECcarString));
//  err = ntagWriteData(NTAG_MEM_BLOCK_START_ADDR_USER_MEMORY, writeBuff, sizeof(RECcarString));
//
//  err = ntagReadData(NTAG_MEM_BLOCK_START_ADDR_USER_MEMORY, readBuff, sizeof(readBuff));

  printf("written string: %s\n", RECcarString);

  ntag_i2c_address = 0x22;

  err = ntagI2CMemIfInit(100,
		  	  	   ntag_i2c_address,
		  	  	   /*int16_t (*cbMasterInit)(uint32_t)*/0,
		  	  	   /*int16_t (*cbSend)(uint8_t*, uint8_t)*/NTAG_i2c_write,
		  	  	   /*int16_t (*cbReceive)(uint8_t*, uint8_t)*/NTAG_i2c_read,
		  	  	   /*int16_t (*cbSendReceive)(uint8_t*, uint8_t, uint8_t*, uint8_t)*/0,
		  	  	   /*int16_t (*cbClose)(void) )*/NTAG_i2c_close
  	  	  	  	   );

  if(err == NTAG_ERR_OK)
	  printf("NTAG interface set OK.\n");
  else
	  printf("NTAG interface set FAILED.\n");


  err = NTAG_i2c_init(ntag_i2c_address, &i2c_fd);

  switch( err ) {
  	  case IMUPI_NO_ERROR:
  		  fprintf (stdout, "Device found.\n" );
  		  break;
  	  case IMUPI_I2C_OPEN_ERROR:
  		  fprintf (stderr, "Unable to open I2C device.\n" );
          exit( EXIT_FAILURE );
  	  case IMUPI_I2C_DEV_NOT_FOUND:
          fprintf (stderr, "Device not found.\n" );
          exit( EXIT_FAILURE );
  	  case IMUPI_I2C_WRITE_ERROR:
          fprintf (stderr, "I2C write error.\n" );
          exit( EXIT_FAILURE );
      default:
          fprintf (stderr, "Initialization errror.\n" );
          exit( EXIT_FAILURE );
  }


  printf("select an NTAG EEPROM memory block to be read: ");

//  scanf("%s", commandLineString);

  do{

	  scanf("%s", commandLineString);

	  if(strlen(commandLineString) > 2)
	  {
		  printf("invalid request!\nretype address or 'q' to quit");
	  	  continue;
	  }
	  else if(strlen(commandLineString) == 1)
	  {
		  if(IS_BETWEEN(commandLineString[0], '0', '9') ||
				  IS_BETWEEN(commandLineString[0], 'A', 'F') ||
				  IS_BETWEEN(commandLineString[0], 'a', 'f') == 1 ||
				  commandLineString[0] == 'Q' || commandLineString[0] == 'q')
			  break;
	  }
	  else if(strlen(commandLineString) == 2)
	  {
		  if((IS_BETWEEN(commandLineString[0], '0', '9') ||
				  IS_BETWEEN(commandLineString[0], 'A', 'F') ||
				  IS_BETWEEN(commandLineString[0], 'a', 'f') == 1)
				  &&
			  (IS_BETWEEN(commandLineString[1], '0', '9') ||
			  	  IS_BETWEEN(commandLineString[1], 'A', 'F') ||
			  	  IS_BETWEEN(commandLineString[1], 'a', 'f') == 1)
			  	  )
			  break;
	  }
  }while(1);


  if(commandLineString[0] == 'q' || commandLineString[0] == 'Q')
  	  return 0;

  for(i = 0; i < strlen(commandLineString); i++)
  {
	  if(IS_BETWEEN(commandLineString[i], '0', '9'))
	  	  ntag_eeprom_block_address = commandLineString[i] - '0';
	  else if(IS_BETWEEN(commandLineString[i], 'A', 'F'))
	  	  ntag_eeprom_block_address = commandLineString[i] - 'A' + 0x0A;
	  else if(IS_BETWEEN(commandLineString[i], 'a', 'f'))
		  ntag_eeprom_block_address = commandLineString[i] - 'a' + 0x0A;
  }

  printf("reading block %d:\n", ntag_eeprom_block_address);


   err = ntagI2CReadBlock(ntag_eeprom_block_address, readBuff);

   switch(err) {
   		case IMUPI_NO_ERROR:
   			break;

   		case IMUPI_INIT_ERROR:
   			fprintf (stderr, "Trying to read an uninitialized device.\n" );
   			exit( EXIT_FAILURE );

   		case IMUPI_I2C_DEV_NOT_FOUND:
   			fprintf (stderr, "Device not found.\n" );
   			exit( EXIT_FAILURE );

   		case IMUPI_I2C_READ_ERROR:
   			fprintf (stderr, "I2C read error.\n" );
   			exit( EXIT_FAILURE );

   		default:
   			fprintf (stderr, "Read errror.\n" );
   			exit( EXIT_FAILURE );
   	}

   fprintf(stdout, "Device read!\n" );

	for(i = 0, counter = 0; i < 16; i++)
	{
		printf("0x%02X", readBuff[i]);
		if(counter++ % 4 == 3)
			printf("\n");
		else
			printf(" - ");
	}

	ntagI2CMemIFClose();

  return err;
}

/**
 * @}
 */
