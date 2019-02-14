// Timing code based on https://stackoverflow.com/questions/14682824/measuring-elapsed-time-in-linux-for-a-c-program

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <float.h>

#include "System.h"
#include "LightSensor.h"
#include "Utils.h"
#include "AccelDrv.h"

#define MAX_PEAK_HEIGHT_THRESHOLD 0.25
#define MIN_PEAK_HEIGHT_THRESHOLD -0.23
#define WINDOW_SIZE 7
#define MARKER_INTERVAL_IN_M 5
#define SAMPLING_DELAY_IN_MS 50

// https://www.cs.sfu.ca/CourseCentral/433/bfraser/other/2015-student-howtos/ReadPhotoresistors.pdf
typedef struct {
    double values[WINDOW_SIZE];
    int position;
    pthread_mutex_t mutex;
} ringQueue_t;

typedef struct {
    double minHeight;
    double maxHeight;
    enum {RISING_EDGE, FALLING_EDGE} peakEdgeType;
} peak_t;

static ringQueue_t rawLightReadings;

static unsigned int currentMarkerNum = 0;

void *monitorPosition();
static _Bool stopping;
static pthread_t monitorPositionTID;
static _Bool doneInit = false;
static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;

void initQueue(ringQueue_t *queue)
{
    pthread_mutex_init(&queue->mutex, NULL);
    queue->position = 0;
}

void PositionTracker_init()
{
    pthread_mutex_lock(&libLock);
    if (doneInit) {
        System_exitError("Cannot initialize PositionTracker more than once!");
    }

    assert(MARKER_INTERVAL_IN_M > 0 && WINDOW_SIZE > 1);
    initQueue(&rawLightReadings);
    stopping = false;
    // LightSensor_init();
    pthread_create(&monitorPositionTID, NULL, monitorPosition, NULL);

    doneInit = true;
    pthread_mutex_unlock(&libLock);
}

void PositionTracker_cleanup()
{
    stopping = true;
    pthread_join(monitorPositionTID, NULL);
}

static void putIntoQueue(ringQueue_t *queue, double newReading)
{
    pthread_mutex_lock(&(queue->mutex));
    queue->values[queue->position] = newReading;
    if (++queue->position >= WINDOW_SIZE) {
        queue->position = 0;
    }
    pthread_mutex_unlock(&(queue->mutex));
}

unsigned int PositionTracker_getCurrentMarkerNum()
{
    assert(doneInit);
    unsigned int returnVal;
    pthread_mutex_lock(&libLock);
    returnVal = currentMarkerNum;
    pthread_mutex_unlock(&libLock);
    return returnVal;
}

double PositionTracker_getCurrentHeight()
{
    assert(doneInit);
    double height;
    pthread_mutex_lock(&libLock);
    height = currentMarkerNum * MARKER_INTERVAL_IN_M;
    pthread_mutex_unlock(&libLock);
    return height;
}

static double sumArray(double array[], int size)
{
    double sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += array[i];
    }
    return sum;
}

static _Bool isAboveThreshold(double reading)
{
    return (reading < MIN_PEAK_HEIGHT_THRESHOLD)
           || (reading > MAX_PEAK_HEIGHT_THRESHOLD);
}

static void handleMarkerFound()
{
    pthread_mutex_lock(&libLock);
    zState_t zState = AccelDrv_getZState();
    if (zState == MOVING_UP) {
        // Passed marker and device is going up
        currentMarkerNum++;
    } else if (zState == MOVING_DOWN) {
        // Passed marker and device is going down
        currentMarkerNum--;
    } else {
        // Passed marker and device is stationary
        // do nothing
    }
    pthread_mutex_unlock(&libLock);
}

void *monitorPosition()
{
    assert(doneInit);

    static const double NANOSECONDS_IN_A_SECOND = 1e9;

    struct timespec startTime, stopTime, peakFoundTime;
    double currentSlope = -1;
    double lastAverageLight = -1;
    double timeDeltaInSec = 0.0;

    _Bool isInsidePeak = false;
    peak_t peak = {.minHeight = DBL_MAX, .maxHeight = -1};

    // Fill queue with initial values
    for (int queuePos = 0; queuePos < WINDOW_SIZE; queuePos++) {
        putIntoQueue(&rawLightReadings, LightSensor_getVoltage());
    }

    while (!stopping) {
        clock_gettime(CLOCK_MONOTONIC, &startTime);
        double newReading = LightSensor_getVoltage();
        putIntoQueue(&rawLightReadings, newReading);
        double currentAverageLight = sumArray(rawLightReadings.values, WINDOW_SIZE)
                                     / WINDOW_SIZE;

        if (lastAverageLight != -1
                && currentAverageLight != -1 && !Utils_isDoubleZero(timeDeltaInSec)) {
            currentSlope = (currentAverageLight - lastAverageLight) / timeDeltaInSec;
            if (isAboveThreshold(currentSlope)) {
                if (currentSlope > 0.0) {
                    // Exiting dark spot on LED strip
                    if (currentSlope >= peak.maxHeight) {
                        clock_gettime(CLOCK_MONOTONIC, &peakFoundTime);
                        peak.maxHeight = currentSlope;
                    }
                    peak.peakEdgeType = RISING_EDGE;
                } else {
                    // Entered dark spot on LED strip
                    if (currentSlope <= peak.minHeight) {
                        clock_gettime(CLOCK_MONOTONIC, &peakFoundTime);
                        peak.minHeight = currentSlope;
                    }
                    peak.peakEdgeType = FALLING_EDGE;
                }
                isInsidePeak = true;
            } else if (isInsidePeak) {
                // FIXME: This assumes that the detection is clean enough so that
                //        every RISING_EDGE is preceded by a FALLING_EDGE.
                //        A simple checksum can be added that adds 1 when
                //        a FALLING_EDGE is detected and subtracts 1 when a
                //        RISING_EDGE is detected. This will allow errors to be
                //        detected when the sum is neither 0 nor 1
                if (peak.peakEdgeType == RISING_EDGE) {
                    handleMarkerFound();
                }
                peak.maxHeight = -1;
                peak.minHeight = DBL_MAX;
                isInsidePeak = false;
            }
        }
        lastAverageLight = currentAverageLight;

        Utils_sleepMilliseconds(SAMPLING_DELAY_IN_MS);

        clock_gettime(CLOCK_MONOTONIC, &stopTime);
        timeDeltaInSec = (stopTime.tv_sec - startTime.tv_sec) +
                         (stopTime.tv_nsec - startTime.tv_nsec) / NANOSECONDS_IN_A_SECOND;
    }
    return NULL;
}