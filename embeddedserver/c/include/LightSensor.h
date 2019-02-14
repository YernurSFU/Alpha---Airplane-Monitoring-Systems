#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H

void   LightSensor_init();
double LightSensor_getRawVoltage();
double LightSensor_getVoltage();
double LightSensor_getResistance();

#endif
