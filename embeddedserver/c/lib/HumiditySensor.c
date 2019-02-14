// Reads temperature and humidity from DHT22/RHT03

// Credit: Tony DiCola; Adafruit Industries.
// [https://github.com/adafruit/Adafruit_Python_DHT]

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "Utils.h"
#include "HumiditySensor.h"
#include "Mmio.h"

// Refer to doc/GPIO_to_base_number_table.txt
#define GPIO_BASE 0
#define GPIO_NUMBER 20

// This is the only processor specific magic value, the maximum amount of time to
// spin in a loop before bailing out and considering the read a timeout.  This should
// be a high value, but if you're running on a much faster platform than a Raspberry
// Pi or Beaglebone Black then it might need to be increased.
#define DHT_MAXCOUNT 32000

// Number of bit pulses to expect from the DHT.  Note that this is 41 because
// the first pulse is a constant 50 microsecond pulse, with 40 pulses to represent
// the data afterwards.
#define DHT_PULSES 41

static float cachedHumidity = 0;
static float cachedTemperature = 0;

static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;

static int readDHT(float* humidity, float* temperature)
{
    // Validate humidity and temperature arguments and set them to zero.
    if (humidity == NULL || temperature == NULL) {
        return DHT_ERROR_ARGUMENT;
    }
    *temperature = 0.0f;
    *humidity = 0.0f;

    // Store the count that each DHT bit pulse is low and high.
    // Make sure array is initialized to start at zero.
    int pulseCounts[DHT_PULSES * 2] = {0};

    // Get GPIO pin and set it as an output.
    gpio_t pin;
    if (Mmio_getGPIO(GPIO_BASE, GPIO_NUMBER, &pin) < 0) {
        return DHT_ERROR_GPIO;
    }
    Mmio_setOutput(pin);

    // Bump up process priority and change scheduler to try to try to make process more 'real time'.
    Utils_setMaxPriority();

    // Set pin high for ~500 milliseconds.
    Mmio_setHigh(pin);
    Utils_sleepMilliseconds(500);

    // The next calls are timing critical and care should be taken
    // to ensure no unnecssary work is done below.

    // Set pin low for ~20 milliseconds.
    Mmio_setLow(pin);
    Utils_busyWaitMilliseconds(20);

    // Set pin as input.
    Mmio_setInput(pin);

    // Wait for DHT to pull pin low.
    uint32_t count = 0;
    while (Mmio_input(pin)) {
        if (++count >= DHT_MAXCOUNT) {
            // Timeout waiting for response.
            Utils_setDefaultPriority();
            return DHT_ERROR_TIMEOUT;
        }
    }

    // Record pulse widths for the expected result bits.
    for (int i = 0; i < DHT_PULSES * 2; i += 2) {
        // Count how long pin is low and store in pulseCounts[i]
        while (!Mmio_input(pin)) {
            if (++pulseCounts[i] >= DHT_MAXCOUNT) {
                // Timeout waiting for response.
                Utils_setDefaultPriority();
                return DHT_ERROR_TIMEOUT;
            }
        }
        // Count how long pin is high and store in pulseCounts[i+1]
        while (Mmio_input(pin)) {
            if (++pulseCounts[i + 1] >= DHT_MAXCOUNT) {
                // Timeout waiting for response.
                Utils_setDefaultPriority();
                return DHT_ERROR_TIMEOUT;
            }
        }
    }

    // Done with timing critical code, now interpret the results.

    // Drop back to normal priority.
    Utils_setDefaultPriority();

    // Compute the average low pulse width to use as a 50 microsecond reference threshold.
    // Ignore the first two readings because they are a constant 80 microsecond pulse.
    uint32_t threshold = 0;
    for (int i = 2; i < DHT_PULSES * 2; i += 2) {
        threshold += pulseCounts[i];
    }
    threshold /= DHT_PULSES - 1;

    // Interpret each high pulse as a 0 or 1 by comparing it to the 50us reference.
    // If the count is less than 50us it must be a ~28us 0 pulse, and if it's higher
    // then it must be a ~70us 1 pulse.
    uint8_t data[5] = {0};
    for (int i = 3; i < DHT_PULSES * 2; i += 2) {
        int index = (i - 3) / 16;
        data[index] <<= 1;
        if (pulseCounts[i] >= threshold) {
            // One bit for long pulse.
            data[index] |= 1;
        }
        // Else zero bit for short pulse.
    }

    // Useful debug info:
    //printf("Data: 0x%x 0x%x 0x%x 0x%x 0x%x\n", data[0], data[1], data[2], data[3], data[4]);

    // Verify checksum of received data.
    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        // Calculate humidity and temp for DHT22 sensor.
        *humidity = (data[0] * 256 + data[1]) / 10.0f;
        *temperature = ((data[2] & 0x7F) * 256 + data[3]) / 10.0f;
        if (data[2] & 0x80) {
            *temperature *= -1.0f;
        }
        return DHT_SUCCESS;
    } else {
        return DHT_ERROR_CHECKSUM;
    }
}

int HumiditySensor_getHumidityAndTemperature(float* humidity, float* temperature)
{
    int returnVal = DHT_ERROR_TIMEOUT;
    pthread_mutex_lock(&libLock);
    returnVal = readDHT(humidity, temperature);
    if (returnVal == DHT_SUCCESS) {
        cachedHumidity = *humidity;
        cachedTemperature = *temperature;
    } else {
        *humidity = cachedHumidity;
        *temperature = cachedTemperature;
    }
    pthread_mutex_unlock(&libLock);
    return returnVal;
}