// SSD1327 Gray Oled Driver Library

// Credit: Visweswara R; Seeed Technology Inc.
// [https://github.com/Seeed-Studio/OLED_Display_96X96]

#ifndef OLED_H
#define OLED_H

#include <stdbool.h>

#define VERTICAL_MODE              01
#define HORIZONTAL_MODE            02

#define Oled_Address               0x3c
#define Oled_Command_Mode          0x80
#define Oled_Data_Mode             0x40

#define Oled_Display_Off_Cmd       0xAE
#define Oled_Display_On_Cmd        0xAF

#define Oled_Normal_Display_Cmd    0xA4
#define Oled_Inverse_Display_Cmd   0xA7
#define Oled_Activate_Scroll_Cmd   0x2F
#define Oled_Dectivate_Scroll_Cmd  0x2E
#define Oled_Set_ContrastLevel_Cmd 0x81

#define Scroll_Left                0x00
#define Scroll_Right               0x01

#define Scroll_2Frames             0x7
#define Scroll_3Frames             0x4
#define Scroll_4Frames             0x5
#define Scroll_5Frames             0x0
#define Scroll_25Frames            0x6
#define Scroll_64Frames            0x1
#define Scroll_128Frames           0x2
#define Scroll_256Frames           0x3

void Oled_init();
void Oled_cleanup();
int initI2cBus(char *bus, int address);

void Oled_setNormalDisplay();
void Oled_setInverseDisplay();

void Oled_sendCommand(unsigned char command);
void Oled_sendData(unsigned char Data);
void Oled_setGrayLevel(unsigned char grayLevel);

void Oled_setVerticalMode();
void Oled_setHorizontalMode();

void Oled_setTextXY(unsigned char Row, unsigned char Column);
void Oled_clearDisplay();
void Oled_setContrastLevel(unsigned char ContrastLevel);
void Oled_putChar(unsigned char c);
void Oled_putString(const char *String);
unsigned char Oled_putNumber(long n);

void Oled_drawBitmap(const unsigned char *bitmaparray, int bytes);

void Oled_setHorizontalScrollProperties(bool direction, unsigned char startRow, unsigned char endRow, unsigned char startColumn, unsigned char endColumn, unsigned char scrollSpeed);
void Oled_activateScroll();
void Oled_deactivateScroll();

#endif
