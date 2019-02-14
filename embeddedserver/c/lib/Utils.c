// Credit: Tony DiCola; Adafruit Industries.
//         for sleep and CPU priority functions
// [https://github.com/adafruit/Adafruit_Python_DHT]

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#include "Utils.h"

int Utils_round(double num)
{
    return (num > 0) ? (num + 0.5) : (num - 0.5);
}

int Utils_writeToFile(const char *filePath, const char *textForWriting)
{
    assert(filePath != NULL && textForWriting != NULL);
    int returnStatus = UTILS_SUCCESS;
    FILE *targetFile = fopen(filePath, "w");

    if (targetFile != NULL) {
        if (fputs(textForWriting, targetFile) <= 0) {
            returnStatus = UTILS_FAILURE;
        }
        fclose(targetFile);
    } else {
        returnStatus = UTILS_FAILURE;
    }

    return returnStatus;
}

_Bool Utils_isDoubleZero(double num)
{
    static const double MAX_MIN_ZERO_LIMIT = 10e-7;
    return (-MAX_MIN_ZERO_LIMIT <= num && num <= MAX_MIN_ZERO_LIMIT);
}

void Utils_busyWaitMilliseconds(uint32_t millis)
{
    // Set delay time period.
    struct timeval deltatime;
    deltatime.tv_sec = millis / 1000;
    deltatime.tv_usec = (millis % 1000) * 1000;
    struct timeval walltime;
    // Get current time and add delay to find end time.
    gettimeofday(&walltime, NULL);
    struct timeval endtime;
    timeradd(&walltime, &deltatime, &endtime);
    // Tight loop to waste time (and CPU) until enough time as elapsed.
    while (timercmp(&walltime, &endtime, < )) {
        gettimeofday(&walltime, NULL);
    }
}

void Utils_sleepMilliseconds(uint32_t millis)
{
    struct timespec sleep;
    sleep.tv_sec = millis / 1000;
    sleep.tv_nsec = (millis % 1000) * 1000000L;
    while (clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, &sleep) && errno == EINTR);
}

void Utils_setMaxPriority(void)
{
    struct sched_param sched;
    memset(&sched, 0, sizeof(sched));
    // Use FIFO scheduler with highest priority for the lowest chance of the kernel context switching.
    sched.sched_priority = sched_get_priority_max(SCHED_FIFO);
    sched_setscheduler(0, SCHED_FIFO, &sched);
}

void Utils_setDefaultPriority(void)
{
    struct sched_param sched;
    memset(&sched, 0, sizeof(sched));
    // Go back to default scheduler with default 0 priority.
    sched.sched_priority = 0;
    sched_setscheduler(0, SCHED_OTHER, &sched);
}

void Utils_splitStr(char*** retArr, size_t* retSize, char* cmd)
{
    *retSize = 0;
    *retArr = NULL;
    char *token;
    while ((token = strsep(&cmd, " \t\n\v\f\r")) != NULL) {
        (*retSize)++;
        (*retArr) = (char **)realloc((*retArr), (sizeof(char *) * (*retSize)));
        (*retArr)[(*retSize) - 1] = strdup(token);
    }
}

void Utils_freeStrArr(char **strArr, size_t size)
{
    size_t i;
    for (i = 0; i < size; i++) {
        free(strArr[i]);
    }
    free(strArr);
}