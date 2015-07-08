//
//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program
//  15-January-2012
//  Dom and Gert
//  Revised: 15-Feb-2013

// Access from ARM Running Linux

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

typedef enum {
    GPIO_SUCCESS = 0,
    GPIO_ERROR
} GPIO_STATUS_T;

inline void gpio_init_io();

inline void gpio_init_pin(unsigned pin);
inline void gpio_set_input(unsigned pin);
inline void gpio_set_output(unsigned pin);
