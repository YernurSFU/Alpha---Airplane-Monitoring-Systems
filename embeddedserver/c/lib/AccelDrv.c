#include "AccelDrv.h"
#include "I2CDrv.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include "AccelDrv.h"
#include "Utils.h"

static i2c_device_t accel_device = {I2C_HW_BUS_1, 0x1C, -1};

#define REG_STATUS 0x00

// Registers for data read (0x00 through
#define SIZE_TO_READ 10
#define REG_ADDR_START_ADDR REG_STATUS
#define REG_ADDR_XMSB 0x01
#define REG_ADDR_XLSB 0x02
#define REG_ADDR_YMSB 0x03
#define REG_ADDR_YLSB 0x04
#define REG_ADDR_ZMSB 0x05
#define REG_ADDR_ZLSB 0x06


#define REG_ADDR_WHO_AM_I   0x0D
#define EXPECTED_WHO_AM_I	0x2A

#define REG_ADDR_CTRL_REG1  0x2A
#define CTRL_REG1_ACTIVE_BIT 0x00	// BIT number

// 16 bit value (12 bits real, bottom 0's), max scale 2G, signed.
#define FULL_SCALE_G   (2)
#define MAX_ACCEL_READ ((float)((1 << 15) - 1)/FULL_SCALE_G)


void AccelDrv_init()
{
	system("echo BB-I2C1 > /sys/devices/platform/bone_capemgr/slots");
	// Turn on the I2C support
	I2cDrv_init(&accel_device);

	// Set accelerometer to active
	I2cDrv_write_register(&accel_device, REG_ADDR_CTRL_REG1, (1 << CTRL_REG1_ACTIVE_BIT));

	// Check who-am-I register
	printf("Accelerometer 'who am i': 0x%02x\n", I2cDrv_read_register(&accel_device, REG_ADDR_WHO_AM_I));

	// Read in a loop:
	for (int offset = -10; offset < 2; offset++) {
		unsigned char me_buff[SIZE_TO_READ];
		I2cDrv_read_registers(&accel_device, REG_ADDR_WHO_AM_I, me_buff - 2, SIZE_TO_READ);
		printf("  WHO AM I Data (OFFSET @ %3d: ", offset);
		for (int i = 0; i < SIZE_TO_READ; i++) {
			printf("0x%02x ", me_buff[i]);
		}
		printf("\n");
	}

	unsigned char buff[SIZE_TO_READ];
	int bytes_read = I2cDrv_read_registers(&accel_device, REG_ADDR_START_ADDR, buff, SIZE_TO_READ);
	printf("Accelerometer: Read %d bytes (expecting up to %d)\n", bytes_read, SIZE_TO_READ);
	printf("  Data: ");
	for (int i = 0; i < SIZE_TO_READ; i++) {
		printf("0x%02x ", buff[i]);
	}
	printf("\n");
}

void AccelDrv_cleanup()
{
	I2cDrv_cleanup(&accel_device);
}

void AccelDrv_getReading(double *dx, double *dy, double *dz)
{
	unsigned char buff[SIZE_TO_READ];
	int bytes_read = I2cDrv_read_registers(&accel_device, REG_ADDR_START_ADDR, buff, SIZE_TO_READ);
	assert(bytes_read == SIZE_TO_READ);

	int16_t x = (buff[REG_ADDR_XMSB] << 8) | (buff[REG_ADDR_XLSB]);
	int16_t y = (buff[REG_ADDR_YMSB] << 8) | (buff[REG_ADDR_YLSB]);
	int16_t z = (buff[REG_ADDR_ZMSB] << 8) | (buff[REG_ADDR_ZLSB]);

	*dx = x / MAX_ACCEL_READ;
	*dy = y / MAX_ACCEL_READ;
	*dz = z / MAX_ACCEL_READ;
}

zState_t AccelDrv_getZState()
{
	static const double MOVING_UP_THRESHOLD = 1.1;
	static const double MOVING_DOWN_THRESHOLD = 0.9;
	double xVal, yVal, zVal;
	AccelDrv_getReading(&xVal, &yVal, &zVal);
	zState_t zState = STATIONARY;
	if (zVal >= MOVING_UP_THRESHOLD) {
		zState = MOVING_UP;
	} else if (zVal <= MOVING_DOWN_THRESHOLD) {
		zState = MOVING_DOWN;
	}
	if (zState == MOVING_DOWN || zState == MOVING_UP) {
		// Debounce before return.
		Utils_sleepMilliseconds(500);
	}
	return zState;
}