#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "System.h"
#include "LightSensor.h"

#define MAX_RAW_VOLTAGE 4095

// Private global values.
static _Bool isInit = false;
static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;

// Private function forward declarations.
static long getRawVoltage();
static void readFileIntoString(char** ret, char* file);

static long getRawVoltage()
{
    char* voltageAsStr;
    readFileIntoString(&voltageAsStr, "/sys/bus/iio/devices/iio:device0/in_voltage1_raw");
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

void LightSensor_init()
{
    pthread_mutex_lock(&libLock);
    if (isInit) {
        System_exitError("Cannot initialize \"LightSensor\" more than once!");
    }
    FILE* fd = fopen("/sys/devices/platform/bone_capemgr/slots", "r");
    if (!fd) {
        System_exitError("Cannot open \"/sys/devices/platform/bone_capemgr/slots\" for read!");
    }
    fprintf(fd, "BB-ADC");
    if (fclose(fd)) {
        System_exitError("Cannot close file descriptor for \"/sys/devices/platform/bone_capemgr/slots\"!");
    }
    isInit = true;
    pthread_mutex_unlock(&libLock);
}

double LightSensor_getRawVoltage()
{
    pthread_mutex_lock(&libLock);
    if (!isInit) {
        System_exitError("You need to initialize \"LightSensor\" before calling \"getRawVoltage\"");
    }
    double retVal = (double) getRawVoltage();
    pthread_mutex_unlock(&libLock);
    return retVal;
}

double LightSensor_getVoltage()
{
    pthread_mutex_lock(&libLock);
    if (!isInit) {
        System_exitError("You need to initialize \"LightSensor\" before calling \"getRawVoltage\"");
    }
    double retVal = ((double) getRawVoltage()) / MAX_RAW_VOLTAGE;
    pthread_mutex_unlock(&libLock);
    return retVal;
}

double LightSensor_getResistance()
{
    pthread_mutex_lock(&libLock);
    if (!isInit) {
        System_exitError("You need to initialize \"LightSensor\" before calling \"getRawVoltage\"");
    }
    double retVal = (1000 / (((double) getRawVoltage()) / MAX_RAW_VOLTAGE)) - 1000;
    pthread_mutex_unlock(&libLock);
    return retVal;
}
