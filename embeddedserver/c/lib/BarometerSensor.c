#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

#include "System.h"
#include "BarometerSensor.h"

#define BAROMETER_TEMPERATURE_FILE "/sys/bus/i2c/drivers/bmp085/1-0077/temp0_input"
#define BAROMETER_PRESSURE_FILE "/sys/bus/i2c/drivers/bmp085/1-0077/pressure0_input"
#define HEIGHT_PRESSURE_SAMPLES 30

static _Bool isInit = false;
static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;

// Private function forward declarations.
static double getTemperature();
static double getPressure();
static void readFileIntoString(char** ret, char* file);
static double baselinePressure;

void BarometerSensor_init()
{
    pthread_mutex_lock(&libLock);
    if (isInit) {
        System_exitError("Cannot initialize \"BarometerSensor\" more than once!");
    }
    FILE* fd = fopen("/sys/devices/platform/bone_capemgr/slots", "r");
    if (!fd) {
        System_exitError("Cannot open \"/sys/devices/platform/bone_capemgr/slots\" for read!");
    }
    fprintf(fd, "BB-I2C1");
    if (fclose(fd)) {
        System_exitError("Cannot close file descriptor for \"/sys/devices/platform/bone_capemgr/slots\"!");
    }

    double sumPressure = 0;
    for (int i = 0; i < HEIGHT_PRESSURE_SAMPLES; i++) {
        sumPressure += getPressure();
    }
    baselinePressure = sumPressure / HEIGHT_PRESSURE_SAMPLES;

    isInit = true;
    pthread_mutex_unlock(&libLock);

}

static double getTemperature()
{
    char* voltageAsStr;
    readFileIntoString(&voltageAsStr, BAROMETER_TEMPERATURE_FILE);
    char* endPtr = NULL;
    errno = 0;
    long retVal = strtol(voltageAsStr, &endPtr, 10);
    if (errno) {
        System_exitError("String to long conversion failed!");
    }
    free(voltageAsStr);
    return retVal / 10;

}

static double getPressure()
{
    char* voltageAsStr;
    readFileIntoString(&voltageAsStr, BAROMETER_PRESSURE_FILE);
    char* endPtr = NULL;
    errno = 0;
    long retVal = strtol(voltageAsStr, &endPtr, 10);
    if (errno) {
        System_exitError("String to long conversion failed!");
    }
    free(voltageAsStr);
    return retVal;
}

static void readFileIntoString(char** ret, char* file)
{
    char* buffer = NULL;
    long length = 0;
    FILE* f = fopen(file, "r");
    if (!f) {
        char* tempStr;
        if (asprintf(&tempStr, "Cannot open file \"%s\" for read!", file) == -1) {
            System_exitError("Cannot allocate temporary string!");
        }
        System_exitError(tempStr);
    }
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek (f, 0, SEEK_SET);
    buffer = (char*) malloc(sizeof(char) * length + 1);
    if (!buffer) {
        System_exitError("Cannot allocate return buffer!");
    }
    fread(buffer, 1, length, f);
    fclose(f);
    *ret = buffer;
}

double BarometerSensor_getTemperature()
{
    pthread_mutex_lock(&libLock);
    if (!isInit) {
        System_exitError("You need to initialize \"LightSensor\" before calling \"getTemperature\"");
    }
    double retVal = (double) getTemperature();
    pthread_mutex_unlock(&libLock);
    return retVal;
}

double BarometerSensor_getPressure()
{
    pthread_mutex_lock(&libLock);
    if (!isInit) {
        System_exitError("You need to initialize \"BarometerSensor\" before calling \"getPressure\"");
    }
    double retVal = (double) getPressure();
    pthread_mutex_unlock(&libLock);
    return retVal;
}

// Source: BMP 180 Datasheet
//         https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf
static double calculateHeight(double currentPressure, double baselinePressure)
{
    return (44330.0 * (1 - pow(currentPressure / baselinePressure, 1 / 5.255)));
}


double BarometerSensor_getHeight()
{
    pthread_mutex_lock(&libLock);
    double height = calculateHeight(getPressure(), baselinePressure);
    pthread_mutex_unlock(&libLock);
    return height;
}