#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <pthread.h>

#include "JoystickMonitor.h"
#include "Oled.h"

#define DEFAULT_GRAY_LEVEL 10
#define MIN_GRAY_LEVEL 1
#define MAX_GRAY_LEVEL 12
#define GRAY_LEVEL_STEP 4

#define MIN_DISPLAY_ROW_COL 0
#define MAX_DISPLAY_COLS 11
#define MAX_DISPLAY_ROWS 10
static char textBuffer[MAX_DISPLAY_ROWS][MAX_DISPLAY_COLS + 1];

static void increaseGrayLevel();
static void decreaseGreyLevel();
static void startDisplay();
static void stopDisplay();
static void doNothing();
static _Bool doneInit = false;
static _Bool isOledOn = false;
static unsigned char grayLevel;

static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t textBufferLock = PTHREAD_MUTEX_INITIALIZER;

static void lockLibrary()
{
    pthread_mutex_lock(&libLock);
}

static void unlockLibrary()
{
    pthread_mutex_unlock(&libLock);
}

void OledManager_init()
{
    lockLibrary();
    assert(!doneInit);
    joystickActions_t actions = {
        .upAction = increaseGrayLevel,
        .downAction = decreaseGreyLevel,
        .leftAction = doNothing,
        .rightAction = doNothing,
        .centerOnAction = startDisplay,
        .centerOffAction = stopDisplay,
    };
    JoystickMonitor_init(actions);
    doneInit = true;
    unlockLibrary();
}

_Bool OledManager_isOledOn()
{
    return isOledOn;
}

static int setTextOnRow(int rowIdx, char *formattedStr, va_list args)
{
    const int MAX_PRINTED_COLS = MAX_DISPLAY_COLS + 1;
    // Fill row buffer with spaces and null-terminate it.
    pthread_mutex_lock(&textBufferLock);
    for (int i = 0; i <= MAX_DISPLAY_COLS; ++i) {
        if (i != MAX_DISPLAY_COLS) {
            textBuffer[rowIdx][i] = ' ';
        } else {
            textBuffer[rowIdx][i] = '\0';
        }
    }
    int potentialBytesWritten = vsnprintf(textBuffer[rowIdx], MAX_PRINTED_COLS, formattedStr, args);
    pthread_mutex_unlock(&textBufferLock);
    return (potentialBytesWritten > MAX_DISPLAY_COLS) ?
           (potentialBytesWritten - MAX_DISPLAY_COLS) : 0; // returns number of unwritten bytes
}

int OledManager_setTextOnRow(int rowIdx, char *formattedStr, ...)
{
    lockLibrary();
    assert(formattedStr != NULL && doneInit);
    va_list(args);
    va_start(args, formattedStr);
    int returnVal = setTextOnRow(rowIdx, formattedStr, args);
    va_end(args);
    unlockLibrary();
    return returnVal;
}

void OledManager_redraw(_Bool shouldClearScreen)
{
    lockLibrary();
    assert(doneInit && isOledOn);
    if (shouldClearScreen) {
        Oled_clearDisplay();
    }
    pthread_mutex_lock(&textBufferLock);
    for (int rowIdx = 0; rowIdx <= MAX_DISPLAY_ROWS; rowIdx++) {
        Oled_setTextXY(rowIdx, 0);
        Oled_putString(textBuffer[rowIdx]);
    }
    pthread_mutex_unlock(&textBufferLock);
    unlockLibrary();
}

static void increaseGrayLevel()
{
    if (isOledOn) {
        unsigned char oldGrayLevel = grayLevel;
        if ((grayLevel + GRAY_LEVEL_STEP) > MAX_GRAY_LEVEL) {
            grayLevel = MAX_GRAY_LEVEL;
        } else {
            grayLevel += GRAY_LEVEL_STEP;
        }
        if (grayLevel != oldGrayLevel) {
            Oled_setGrayLevel(grayLevel);
            OledManager_redraw(true);
        }
    }
}

static void decreaseGreyLevel()
{
    if (isOledOn) {
        unsigned char oldGrayLevel = grayLevel;
        if ((grayLevel - GRAY_LEVEL_STEP) < MIN_GRAY_LEVEL) {
            grayLevel = MIN_GRAY_LEVEL;
        } else {
            grayLevel -= GRAY_LEVEL_STEP;
        }
        if (grayLevel != oldGrayLevel) {
            Oled_setGrayLevel(grayLevel);
            OledManager_redraw(true);
        }
    }
}

static void startDisplay()
{
    Oled_init();
    Oled_setVerticalMode();
    Oled_clearDisplay();
    grayLevel = DEFAULT_GRAY_LEVEL;
    Oled_setGrayLevel(grayLevel);
    isOledOn = true;
    OledManager_redraw(false);
}

static void stopDisplay()
{
    Oled_cleanup();
    isOledOn = false;
}

static void doNothing()
{
    // voila!
}
