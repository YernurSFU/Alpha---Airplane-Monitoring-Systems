#include <stdio.h>
#include <assert.h>

#include "Utils.h"
#include "Joystick.h"

#define JOYSTICK_UP_PIN "26"
#define JOYSTICK_DOWN_PIN "46"
#define JOYSTICK_LEFT_PIN "65"
#define JOYSTICK_RIGHT_PIN "47"
#define JOYSTICK_CENTER_PIN "27"
#define JOYSTICK_INPUT_MODE "in"

#define JOYSTICK_PRESSED '0'

static _Bool doneInit = false;

int Joystick_init(void)
{
    // Note: the GPIO pin for the joystick may already be exported causing a
    //       'Device or resource busy' error, so errors are not handled here
    Utils_writeToFile(GPIO_EXPORT_FILE_PATH, JOYSTICK_UP_PIN);
    Utils_writeToFile(GPIO_EXPORT_FILE_PATH, JOYSTICK_DOWN_PIN);
    Utils_writeToFile(GPIO_EXPORT_FILE_PATH, JOYSTICK_LEFT_PIN);
    Utils_writeToFile(GPIO_EXPORT_FILE_PATH, JOYSTICK_RIGHT_PIN);
    Utils_writeToFile(GPIO_EXPORT_FILE_PATH, JOYSTICK_CENTER_PIN);

    int returnStatus = Utils_writeToFile(DIRECTION_FILE(JOYSTICK_UP_PIN), JOYSTICK_INPUT_MODE);
    if (returnStatus != UTILS_SUCCESS) {
        perror(DIRECTION_FILE(JOYSTICK_UP_PIN));
    }
    returnStatus = Utils_writeToFile(DIRECTION_FILE(JOYSTICK_DOWN_PIN), JOYSTICK_INPUT_MODE);
    if (returnStatus != UTILS_SUCCESS) {
        perror(DIRECTION_FILE(JOYSTICK_DOWN_PIN));
    }
    returnStatus = Utils_writeToFile(DIRECTION_FILE(JOYSTICK_LEFT_PIN), JOYSTICK_INPUT_MODE);
    if (returnStatus != UTILS_SUCCESS) {
        perror(DIRECTION_FILE(JOYSTICK_LEFT_PIN));
    }
    returnStatus = Utils_writeToFile(DIRECTION_FILE(JOYSTICK_RIGHT_PIN), JOYSTICK_INPUT_MODE);
    if (returnStatus != UTILS_SUCCESS) {
        perror(DIRECTION_FILE(JOYSTICK_RIGHT_PIN));
    }
    returnStatus = Utils_writeToFile(DIRECTION_FILE(JOYSTICK_CENTER_PIN), JOYSTICK_INPUT_MODE);
    if (returnStatus != UTILS_SUCCESS) {
        perror(DIRECTION_FILE(JOYSTICK_CENTER_PIN));
    }
    doneInit = true;
    return returnStatus;
}

static _Bool checkIfPressed(const char *joystickValueFile)
{
    assert(doneInit);
    FILE *joystickValueFileStream = fopen(joystickValueFile, "r");
    if (joystickValueFileStream == NULL) {
        perror(joystickValueFile);
        return false;
    }
    int joystickValue = fgetc(joystickValueFileStream);
    int returnStatus = false;
    if (joystickValue != EOF && joystickValue == JOYSTICK_PRESSED) {
        returnStatus = true;
    }
    fclose(joystickValueFileStream);
    return returnStatus;
}

_Bool Joystick_checkIfPressedUp(void)
{
    return checkIfPressed(VALUE_FILE(JOYSTICK_UP_PIN));
}

_Bool Joystick_checkIfPressedDown(void)
{
    return checkIfPressed(VALUE_FILE(JOYSTICK_DOWN_PIN));
}

_Bool Joystick_checkIfPressedLeft(void)
{
    return checkIfPressed(VALUE_FILE(JOYSTICK_LEFT_PIN));
}

_Bool Joystick_checkIfPressedRight(void)
{
    return checkIfPressed(VALUE_FILE(JOYSTICK_RIGHT_PIN));
}

_Bool Joystick_checkIfPressedCenter(void)
{
    return checkIfPressed(VALUE_FILE(JOYSTICK_CENTER_PIN));
}

