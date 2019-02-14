#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "System.h"
#include "ReedSwitch.h"
#include "Utils.h"

#define MAX_RAW_VOLTAGE          4095
#define REED_SWITCH_GPIO         "15"
#define REED_SWITCH_GPIO_PATH    "/sys/class/gpio/gpio15"

// Private global values.
static _Bool isInit = false;
static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;

// Private function forward declarations.
static _Bool isDoorOpen();
static void readFileIntoString(char** ret, char* file);

static _Bool isDoorOpen()
{
    char* res;
    readFileIntoString(&res, "/sys/class/gpio/gpio15/value");
    char retVal = res[0];
    free(res);
    return retVal == '0';
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

void ReedSwitchSensor_init()
{
    pthread_mutex_lock(&libLock);
    int res = Utils_writeToFile("/sys/class/gpio/export",  REED_SWITCH_GPIO);
    if (res != UTILS_SUCCESS) {
        perror("/sys/class/gpio/export");
    }
    res = Utils_writeToFile("/sys/class/gpio/gpio15/direction",  "in");
    if (res != UTILS_SUCCESS) {
        perror("/sys/class/gpio/export");
    }
    isInit = true;
    pthread_mutex_unlock(&libLock);
}

_Bool ReedSwitchSensor_isDoorOpen()
{
    pthread_mutex_lock(&libLock);
    if (!isInit) {
        System_exitError("You need to initialize \"ReedSwitchSensor\" before calling \"getRawVoltage\"");
    }

    _Bool retVal = (_Bool) isDoorOpen();
    pthread_mutex_unlock(&libLock);
    return retVal;
}
