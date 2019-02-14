#include <stdio.h>
#include <pthread.h>
#include <assert.h>

#include "Joystick.h"
#include "JoystickMonitor.h"
#include "Utils.h"

#define SAMPLING_DELAY_IN_MS 200
#define BUTTON_HOLD_DELAY_IN_MS 2000

void *monitorJoystick();
static void validateActions(joystickActions_t *actions);
static _Bool doneInit = false;
static _Bool stopping = false;
static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t monitorJoystickTID;
static joystickActions_t joystickActions;

void JoystickMonitor_init(joystickActions_t actions)
{
    pthread_mutex_lock(&libLock);
    assert(!doneInit);
    validateActions(&actions);
    joystickActions = actions;
    Joystick_init();
    pthread_create(&monitorJoystickTID, NULL, monitorJoystick, NULL);
    doneInit = true;
    pthread_mutex_unlock(&libLock);
}

void JoystickMonitor_cleanup()
{
    stopping = true;
    pthread_join(monitorJoystickTID, NULL);
}

static void validateActions(joystickActions_t *actions)
{
    assert(actions->upAction != NULL &&
           actions->downAction != NULL &&
           actions->leftAction != NULL &&
           actions->rightAction != NULL &&
           actions->centerOffAction != NULL &&
           actions->centerOnAction != NULL);
}

void *monitorJoystick()
{
    const int ITERATIONS_TO_HOLD = (BUTTON_HOLD_DELAY_IN_MS / SAMPLING_DELAY_IN_MS);
    int buttonHeldIterations = ITERATIONS_TO_HOLD;
    int cooldownIterations = 0;
    _Bool doneCenterOnAction = false;

    while (!stopping) {
        Utils_sleepMilliseconds(SAMPLING_DELAY_IN_MS);
        if (cooldownIterations > 0) {
            cooldownIterations--;
        } else {
            if (Joystick_checkIfPressedCenter()) {
                if (buttonHeldIterations <= 0) {
                    if (doneCenterOnAction) {
                        (joystickActions.centerOffAction)();
                        doneCenterOnAction = false;
                    } else {
                        (joystickActions.centerOnAction)();
                        doneCenterOnAction = true;
                    }
                    cooldownIterations = ITERATIONS_TO_HOLD;
                    buttonHeldIterations = ITERATIONS_TO_HOLD;
                }
                buttonHeldIterations--;
            } else {
                buttonHeldIterations = ITERATIONS_TO_HOLD;
            }
        }
        if (doneCenterOnAction) {
            if (Joystick_checkIfPressedUp()) {
                (joystickActions.upAction)();
            } else if (Joystick_checkIfPressedDown()) {
                (joystickActions.downAction)();
            } else if (Joystick_checkIfPressedLeft()) {
                (joystickActions.leftAction)();
            } else if (Joystick_checkIfPressedRight()) {
                (joystickActions.rightAction)();
            } else {
                // do nothing
            }
        }
    }
    return NULL;
}