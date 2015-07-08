//
//  RPI_peripheral.h
//  Free-Copter
//
//  Created by Master on 7/5/15.
//
//

#ifndef __Free_Copter__RPI_peripheral__
#define __Free_Copter__RPI_peripheral__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>
#include "gpio_mem_dump.h"

#define BCM2708_PERI_BASE       0x3F000000
#define GPIO_BASE               (BCM2708_PERI_BASE + 0x00200000)	// GPIO controller

#define BLOCK_SIZE 		(4*1024)

// IO Acces
struct bcm2708_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};


extern struct bcm2708_peripheral gpio;  // They have to be found somewhere, but can't be in the header


int map_peripheral(struct bcm2708_peripheral *p);
void unmap_peripheral(struct bcm2708_peripheral *p);
void gpio_control_start();

#endif /* defined(__Free_Copter__RPI_peripheral__) */
