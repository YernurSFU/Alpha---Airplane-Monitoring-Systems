#ifndef BAROMETER_SENSOR_H
#define BAROMETER_SENSOR_H

void BarometerSensor_init();
double BarometerSensor_getTemperature();
double BarometerSensor_getPressure();
double BarometerSensor_getHeight();

#endif