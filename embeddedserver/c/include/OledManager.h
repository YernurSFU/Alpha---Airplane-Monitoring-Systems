#ifndef OLED_MANAGER_H
#define OLED_MANAGER_H

#include <stdbool.h>

void OledManager_init();
int OledManager_setTextOnRow(int rowIdx, char *formattedStr, ...);
void OledManager_redraw(_Bool shouldClearScreen);
_Bool OledManager_isOledOn();

#endif
