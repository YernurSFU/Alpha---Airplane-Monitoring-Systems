// Reads temperature and humidity from DHT22/RHT03

// Credit: Tony DiCola; Adafruit Industries.
// [https://github.com/adafruit/Adafruit_Python_DHT]

#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include <stdint.h>

// Define errors and return values.
#define DHT_ERROR_TIMEOUT -1
#define DHT_ERROR_CHECKSUM -2
#define DHT_ERROR_ARGUMENT -3
#define DHT_ERROR_GPIO -4
#define DHT_SUCCESS 0

// Read DHT sensor connected to GPIO bin GPIO<base>_<number>, for example P8_11 is GPIO1_13 with
// base = 1 and number = 13.  Humidity and temperature will be returned in the provided parameters.
// If a successfull reading could be made a value of 0 (DHT_SUCCESS) will be returned.  If there
// was an error reading the sensor a negative value will be returned.  Some errors can be ignored
// and retried, specifically DHT_ERROR_TIMEOUT or DHT_ERROR_CHECKSUM.
int HumiditySensor_getHumidityAndTemperature(float* humidity, float* temperature);

#endif