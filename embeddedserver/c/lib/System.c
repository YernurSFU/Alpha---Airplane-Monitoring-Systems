#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "PositionTracker.h"
#include "BarometerSensor.h"
#include "System.h"
#include "LightSensor.h"
#include "OledManager.h"
#include "ReedSwitch.h"
#include "AccelDrv.h"
#include "UDPNetworking.h"
#include "LightSensor.h"
#include "HumiditySensor.h"
#include "UDPNetworking.h"
#include "Utils.h"
#include "AccelDrv.h"
#include "ReedSwitch.h"
#include "WatchdogHitter.h"

static _Bool isInit = false;
static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t printerLock = PTHREAD_MUTEX_INITIALIZER;

void System_init()
{
    pthread_mutex_lock(&libLock);
    if (isInit) {
        System_exitError("Cannot initialize System more than once!");
    }

    LightSensor_init();
    OledManager_init();
    BarometerSensor_init();
    ReedSwitchSensor_init();
    UDPNetworking_init();
    AccelDrv_init();
    PositionTracker_init();
    WatchdogHitter_start();

    isInit = true;
    pthread_mutex_unlock(&libLock);
}

void System_exitError(char* errMsg)
{
    // Only allow one caller to print their error message.
    pthread_mutex_lock(&printerLock);
    if (errMsg)
        printf("Error: %s\n", errMsg);
    exit(1);
}
