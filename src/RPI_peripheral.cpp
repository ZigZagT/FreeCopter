//
//  RPI_peripheral.cpp
//  Free-Copter
//
//  Created by Master on 7/5/15.
//
//

#include "RPI_peripheral.h"

struct bcm2708_peripheral gpio = {GPIO_BASE};

// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int map_peripheral(struct bcm2708_peripheral *p)
{
    // Open /dev/mem
    if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        printf("Failed to open /dev/mem, try checking permissions.\n");
        return -1;
    }
    
    p->map = mmap(
                  NULL,
                  BLOCK_SIZE,
                  PROT_READ|PROT_WRITE,
                  MAP_SHARED,
                  p->mem_fd,      // File descriptor to physical memory virtual file '/dev/mem'
                  p->addr_p       // Address in physical map that we want this memory block to expose
                  );
    
    if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
    }
    
    p->addr = (volatile unsigned int *)p->map;
    return 0;
}

void unmap_peripheral(struct bcm2708_peripheral *p) {
    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}

void gpio_control_start() {
    map_peripheral(&gpio);
    
#define route_pin_copter
    
#ifdef route_pin_copter
    // input pin is gpio 02-09, as A E T R U Aux1 Aux2 Aux3
    const unsigned init_input_pin_init =  ~(0b111111111111111111111111 << (2 * 3));
    // output pin is gpio 20-24 as A E T R U
    const unsigned init_output_pin_init = ~(0b111111111111111);
    const unsigned init_output_pin_set =    0b001001001001001;
    gpio.addr[0] &= init_input_pin_init;
    //gpio.addr[2] &= init_output_pin_init;
    //gpio.addr[2] |= init_output_pin_set;
    gpio.addr[2] &= ~0b111;
    gpio.addr[2] |= 1;

    const unsigned man_input_pin_mask = (0b11111111 << 2);
    const unsigned man_output_pin_mask = (0b11111 << 20);
    volatile unsigned temp = 0;
    while (1) {
        std::cout << std::endl;
        std::cout << "gpio13: \t" << unsigned_to_binary(gpio.addr[13]) << std::endl;
        temp = gpio.addr[13] & man_input_pin_mask;
        std::cout << "temp: \t\t" << unsigned_to_binary(temp) << std::endl;
        std::cout << "set temp: \t" << unsigned_to_binary((temp << 18) & man_output_pin_mask) << std::endl;
        std::cout << "clr temp: \t" << unsigned_to_binary((~temp << 18) & man_output_pin_mask) << std::endl;
        gpio.addr[7] = ((temp << 18) & man_output_pin_mask); // the SET,CLR register is read only, so you should use = instead of |=
        gpio.addr[10] = ((~(temp << 18)) & man_output_pin_mask);
        //gpio.addr[7] |= ((temp << 18) & (man_output_pin_mask & 1 << 20));
        //gpio.addr[10] |= ((~temp << 18) & (man_output_pin_mask & 1 << 20));
        //gpio.addr[7] |= ((gpio.addr[13] << 18) & man_output_pin_mask); // 7 is set unsigned, 13 is read unsigned.
        //gpio.addr[10] |= ((~gpio.addr[13] << 18) & man_output_pin_mask); // 10 is clear unsigned
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
#endif
    
#ifdef test_pin20_output
    gpio.addr[2] &= ~0b111;
    gpio.addr[2] |= 1;
    bool status = true;
    while (1) {
        if (status) {
            gpio.addr[7] |= 1 << 20;
            status = false;
        } else {
            gpio.addr[10] |= 1<<20;
            status = true;
        }
        std::this_thread::sleep_for (std::chrono::seconds(3));
    }
#endif
#ifdef route_pin17_to_pin27
    // const unsigned pin_mode_mask = 0xE00000; // pin 17
    // const unsigned pin17_read_mask = 0x20000;
    // const unsigned pin27_read_mask = 0x20000;
    
    gpio.addr[1] &= ~(7 << (7 * 3));
    gpio.addr[2] &= ~(7 << (7 * 3));
    gpio.addr[2] |= (1 << (7 * 3));
    
    gpio.addr[2] &= ~(7 << (6 * 3));
    gpio.addr[2] |= (1 << (6 * 3));
    unsigned t = 0;
    while (1) {
        t = (gpio.addr[13] & (1 << 17));
        std::cout << "t:" << t << std::endl;
        if (t != 0) {
            gpio.addr[7] |= (1 << 27);
        } else {
            gpio.addr[10] |= (1 << 27);
        }
        gpio_mem_dump();
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
#endif
    
    unmap_peripheral(&gpio);
}