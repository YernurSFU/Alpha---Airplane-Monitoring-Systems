#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdbool.h>

int Joystick_init(void);
_Bool Joystick_checkIfPressedUp(void);
_Bool Joystick_checkIfPressedDown(void);
_Bool Joystick_checkIfPressedLeft(void);
_Bool Joystick_checkIfPressedRight(void);
_Bool Joystick_checkIfPressedCenter(void);

#endif
