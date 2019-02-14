// Simple fast memory-mapped GPIO library for the Beaglebone Black.
// Allows reading and writing GPIO at very high speeds, up to ~2.6mhz!

// Credit: Tony DiCola; Adafruit Industries.
// [https://github.com/adafruit/Adafruit_Python_DHT]

/*
  // Example usage:

  #include <stdio.h>
  #include "bbb_mmio.h"

  int main(int argc, char* argv[]) {
    // Get GPIO pin.
    // See the giant table of of pins in the system reference manual for details
    // on the base and number for a given GPIO:
    //  https://github.com/CircuitCo/BeagleBone-Black/blob/master/BBB_SRM.pdf?raw=true
    // Section 7 Connectors, table 12 shows P8_11 maps to GPIO1_13, so 1 is the
    // gpio base and 13 is the gpio number.
    gpio_t p8_11;
    if (Mmio_getGPIO(1, 13, &p8_11) < 0) {
      printf("Couldn't get requested GPIO pin!\n");
      return 1;
    }
    // Set pin as output.
    Mmio_setOutput(p8_11);
    // Toggle the pin high and low as fast as possible.
    // This generates a signal at about 2.6mhz in my tests.
    // Each pulse high/low is only about 200 nanoseconds long!
    while (1) {
      Mmio_setHigh(p8_11);
      Mmio_setLow(p8_11);
    }
    return 0;
  }

*/

#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>

#define MMIO_SUCCESS 0
#define MMIO_ERROR_ARGUMENT -1
#define MMIO_ERROR_DEVMEM -2
#define MMIO_ERROR_MMAP -3

#define MMIO_OE_ADDR 0x134
#define MMIO_GPIO_DATAOUT 0x13C
#define MMIO_GPIO_DATAIN 0x138
#define MMIO_GPIO_CLEARDATAOUT 0x190
#define MMIO_GPIO_SETDATAOUT 0x194

// Define struct to represent a GPIO pin based on its base memory address and number.
typedef struct {
    volatile uint32_t* base;
    int number;
} gpio_t;

int Mmio_getGPIO(int base, int number, gpio_t* gpio);

static inline void Mmio_setOutput(gpio_t gpio)
{
    gpio.base[MMIO_OE_ADDR / 4] &= (0xFFFFFFFF ^ (1 << gpio.number));
}

static inline void Mmio_setInput(gpio_t gpio)
{
    gpio.base[MMIO_OE_ADDR / 4] |= (1 << gpio.number);
}

static inline void Mmio_setHigh(gpio_t gpio)
{
    gpio.base[MMIO_GPIO_SETDATAOUT / 4] = 1 << gpio.number;
}

static inline void Mmio_setLow(gpio_t gpio)
{
    gpio.base[MMIO_GPIO_CLEARDATAOUT / 4] = 1 << gpio.number;
}

static inline uint32_t Mmio_input(gpio_t gpio)
{
    return gpio.base[MMIO_GPIO_DATAIN / 4] & (1 << gpio.number);
}

#endif