//
//  raspberry_pi.h
//  Free-Copter
//
//  Created by Master on 7/5/15.
//
//

#ifndef __RASPBERRY_PI_H__
#define __RASPBERRY_PI_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <deque>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "gpio_mem_dump.h"


// raspberry_pi_clock_type_interrupt, raspberry_pi_clock_type_chrono
// raspberry_pi_clock_type_interrupt for use system clock interrupts or raspberry_pi_clock_type_chrono for use cpp header <chrono>
#define raspberry_pi_clock_type_chrono

// raspberry_pi_gpio_poll_type_poll or raspberry_pi_gpio_poll_type_interrupt
// raspberry_pi_gpio_poll_type_poll for polling gpio pins every clock tick, while raspberry_pi_gpio_poll_type_interrupt uses gpio interrupts.
#define raspberry_pi_gpio_poll_type_poll

struct bcm2708_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};

struct gpio_status {
    // from 0 to 2000
    volatile unsigned A = 1000;
    volatile unsigned E = 1000;
    volatile unsigned T = 0;
    volatile unsigned R = 1000;
    volatile unsigned U = 1000;
    volatile unsigned Aux1 = 1000;
    volatile unsigned Aux2 = 1000;
    volatile unsigned Aux3 = 1000;
};


extern struct bcm2708_peripheral gpio;
extern struct gpio_status gpio_in;
extern struct gpio_status gpio_out;

// in ms.
extern volatile double input_pwm_min;
extern volatile double input_pwm_max;

void raspberry_pi_start();

// call this function before exit. (in the SIGTERM or SIGINT handler)
void raspberry_pi_term();
#endif /* defined(__RASPBERRY_PI_H__) */
