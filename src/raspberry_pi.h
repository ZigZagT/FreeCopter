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
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include <iostream>
#include <string.h>

#include "gpio_mem_dump.h"
#include "config.h"
#include "rpi_rpio_pwm.h"

// for raspberry pi 1
//define BCM2708_PERI_BASE          0x20000000
// for raspberry pi 2
#define BCM2708_PERI_BASE           0x3F000000

// Memory Addresses
#define DMA_BASE                    (BCM2708_PERI_BASE + 0x00007000)
#define DMA_CHANNEL_INC             0x100
#define DMA_LEN                     0x24
#define PWM_BASE                    (BCM2708_PERI_BASE + 0x0020C000)
#define PWM_LEN                     0x28
#define CLK_BASE                    (BCM2708_PERI_BASE + 0x00101000)
#define CLK_LEN                     0xA8
#define GPIO_BASE                   (BCM2708_PERI_BASE + 0x00200000)
#define GPIO_LEN                    0x100
#define PCM_BASE                    (BCM2708_PERI_BASE + 0x00203000)
#define PCM_LEN                     0x24

// Standard page sizes
#define BLOCK_SIZE                  (4*1024)
#define PAGE_SIZE                   (4*1024)
#define PAGE_SHIFT                  12



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

// Interface implementation
extern void module_init(void *);
extern void module_term(void *);
extern void module_output_loop(void *);
extern void module_input_loop(void *);

#endif /* defined(__RASPBERRY_PI_H__) */
