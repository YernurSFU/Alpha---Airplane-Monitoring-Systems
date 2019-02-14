#ifndef SYSTEM_H
#define SYSTEM_H

#include "BarometerSensor.h"
#include "System.h"
#include "LightSensor.h"
#include "OledManager.h"
#include "ReedSwitch.h"
#include "AccelDrv.h"
#include "UDPNetworking.h"

void System_init();
void System_exitError(char* errMsg);

#endif
