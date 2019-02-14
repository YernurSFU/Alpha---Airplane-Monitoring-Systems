#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdbool.h>

#define GPIO_EXPORT_FILE_PATH "/sys/class/gpio/export"
#define DIRECTION_FILE(PIN) "/sys/class/gpio/gpio" PIN "/direction"
#define VALUE_FILE(PIN) "/sys/class/gpio/gpio" PIN "/value"
static const int UTILS_SUCCESS = 1;
static const int UTILS_FAILURE = 0;

int Utils_round(double num);
int Utils_writeToFile(const char *filePath, const char *textForWriting);

_Bool Utils_isDoubleZero(double num);

// Busy wait delay for most accurate timing, but high CPU usage.
// Only use this for short periods of time (a few hundred milliseconds at most)!
void Utils_busyWaitMilliseconds(uint32_t millis);

// General delay that sleeps so CPU usage is low, but accuracy is potentially bad.
void Utils_sleepMilliseconds(uint32_t millis);

// Increase scheduling priority and algorithm to try to get 'real time' results.
void Utils_setMaxPriority(void);

// Drop scheduling priority back to normal/default.
void Utils_setDefaultPriority(void);

// Split string according to spaces.
void Utils_splitStr(char*** retArr, size_t* retSize, char* cmd);

// Destruct string array.
void Utils_freeStrArr(char** strArr, size_t size);

#endif