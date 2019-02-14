#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "System.h"
#include "LightSensor.h"
#include "HumiditySensor.h"
#include "UDPNetworking.h"
#include "Utils.h"
#include "AccelDrv.h"
#include "ReedSwitch.h"
#include "Utils.h"
#include "Oled.h"
#include "OledManager.h"
#include "WatchdogHitter.h"
#include "PositionTracker.h"

void * runOledThread()
{
    float humidity = 0;
    float hTemperature = 0;

    while (true) {
        double bTemperature = BarometerSensor_getTemperature();
        double pressure = BarometerSensor_getPressure();
        HumiditySensor_getHumidityAndTemperature(&humidity, &hTemperature);
        double voltage = LightSensor_getVoltage();

        OledManager_setTextOnRow(0, "RH: %.2f", humidity);
        OledManager_setTextOnRow(1, "hCel: %.2f", hTemperature);
        OledManager_setTextOnRow(2, "bCel: %.2f", bTemperature);
        OledManager_setTextOnRow(3, "Light: %.2f%%", voltage * 100);
        OledManager_setTextOnRow(4, "Mag: %d", ReedSwitchSensor_isDoorOpen());
        OledManager_setTextOnRow(5, "Bars: %.2f", pressure / 1000.0);

        if (OledManager_isOledOn()) {
            OledManager_redraw(false);
        }

        Utils_sleepMilliseconds(1000);
    }
    return NULL;
}

void * hitWatchdogThread()
{
    static const int HIT_DELAY_IN_MS = 15000;
    while (true) {
        WatchdogHitter_hit();
        Utils_sleepMilliseconds(HIT_DELAY_IN_MS);
    }
    return NULL;
}

int standardizeZState(zState_t zState)
{
    if (zState == MOVING_UP) {
        return 1;
    } else if (zState == MOVING_DOWN) {
        return 0;
    } else {
        return -1;
    }
}

int main(void)
{
    System_init();

    static pthread_t runOledTID;
    static pthread_t hitWatchdogTID;
    pthread_create(&runOledTID, NULL, runOledThread, NULL);
    pthread_create(&hitWatchdogTID, NULL, hitWatchdogThread, NULL);

    while (true) {
        char *currentCmd = UDPNetworking_getCurrentCommand();
        char **currentCmdTokens = NULL;
        size_t currentCmdSize = 0;
        Utils_splitStr(&currentCmdTokens, &currentCmdSize, currentCmd);
        if (currentCmdSize >= 1 && strcmp(currentCmdTokens[0], "GET_UPDATES") == 0) {
            float humidityVal, humidityTemp;
            HumiditySensor_getHumidityAndTemperature(&humidityVal, &humidityTemp);
            zState_t zState = AccelDrv_getZState();
            double barometerHeight = BarometerSensor_getHeight();
            double markerHeight = PositionTracker_getCurrentHeight();

            char *toSendMsg;
            double voltagePercent = LightSensor_getVoltage() * 100;
            if (asprintf(&toSendMsg,
                         "{\n"
                         "    \"humidityVal\": \"%f\",\n"
                         "    \"temperatureVal\": \"%f\",\n"
                         "    \"positionVal\": \"%f\",\n"
                         "    \"markerPositionVal\": \"%f\",\n"
                         "    \"pressureVal\": \"%f\",\n"
                         "    \"direction\": \"%d\",\n"
                         "    \"lightLevel\": \"%f\",\n"
                         "    \"doorStateVal\": \"%d\"\n"
                         "}\n",
                         humidityVal,
                         BarometerSensor_getTemperature(),
                         barometerHeight,
                         markerHeight,
                         BarometerSensor_getPressure(),
                         standardizeZState(zState),
                         voltagePercent,
                         ReedSwitchSensor_isDoorOpen()) == -1) {
                System_exitError("Cannot allocate temporary string!");
            }
            UDPNetworking_sendMessage(toSendMsg);
            free(toSendMsg);
            toSendMsg = NULL;
        }
        Utils_freeStrArr(currentCmdTokens, currentCmdSize);
        currentCmdTokens = NULL;
        currentCmdSize = 0;
    }
}