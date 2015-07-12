//
//  raspberry_pi.cpp
//  Free-Copter
//
//  Created by Master on 7/5/15.
//
//

#include "raspberry_pi.h"

// for raspberry pi 1
//define BCM2708_PERI_BASE          0x20000000
// for raspberry pi 2
#define BCM2708_PERI_BASE           0x3F000000
#define GPIO_BASE                   (BCM2708_PERI_BASE + 0x00200000)	// GPIO controller

#define BLOCK_SIZE                  (4*1024)
#define PAGE_SIZE                   (4*1024)

static std::condition_variable clock_tick;
static volatile bool ready = false;
struct bcm2708_peripheral gpio = {GPIO_BASE};
#define gpio_input_pin_mask  (0b11111111 << 2)
#define gpio_output_pin_mask (0b11111 << 20)


static int map_peripheral(struct bcm2708_peripheral *p);
static void unmap_peripheral(struct bcm2708_peripheral *p);

static void raspberry_pi_init();
static void raspberry_pi_clock();
static void raspberry_pi_gpio_route();
static void raspberry_pi_gpio_pwm();

static inline void do_gpio_route();
static inline void do_gpio_pwm();

void raspberry_pi_start();
void raspberry_pi_term();




// Exposes the physical address defined in the passed structure using mmap on /dev/mem
static int map_peripheral(struct bcm2708_peripheral *p) {
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

static void unmap_peripheral(struct bcm2708_peripheral *p) {
    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}

static void raspberry_pi_init() {
    map_peripheral(&gpio);
    // input pin is gpio 02-09, as A E T R U Aux1 Aux2 Aux3
    const unsigned init_input_pin_init =  ~(0b111111111111111111111111 << (2 * 3));
    // output pin is gpio 20-24 as A E T R U
    const unsigned init_output_pin_init = ~(0b111111111111111);
    const unsigned init_output_pin_set =    0b001001001001001;
    gpio.addr[0] &= init_input_pin_init;
    gpio.addr[2] &= init_output_pin_init;
    gpio.addr[2] |= init_output_pin_set;
    
     std::thread clock_tick_thread(raspberry_pi_clock);
     std::thread gpio_route_thread(raspberry_pi_gpio_route);
    
    clock_tick_thread.detach();
    gpio_route_thread.detach();
}

static void raspberry_pi_clock() {
    while (!ready) {
        ;
    }
    for (;;) {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        clock_tick.notify_all();
        if (!ready) {
            clock_tick.notify_all();
            return;
        }
    }
}

static void raspberry_pi_gpio_route() {
    std::mutex mtx;
    for (;;) {
        std::unique_lock<std::mutex> lck(mtx);
        clock_tick.wait(lck);
        do_gpio_route();
        if (!ready) {
            return;
        }
    }
}

static void raspberry_pi_gpio_pwm() {
    std::mutex mtx;
    for (;;) {
        std::unique_lock<std::mutex> lck(mtx);
        clock_tick.wait(lck);
        do_gpio_pwm();
        if (!ready) {
            return;
        }
    }
}

static inline void do_gpio_route() {
    // the SET,CLR register is read only, so you should use = instead of "|=".
    gpio.addr[7] = ((gpio.addr[13] << 18) & gpio_output_pin_mask);
    gpio.addr[10] = ((~gpio.addr[13] << 18) & gpio_output_pin_mask);
}

static inline void do_gpio_pwm() {
    return;
}

void raspberry_pi_start() {
    raspberry_pi_init();
    ready = true;
}

void raspberry_pi_term() {
    ready = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    unmap_peripheral(&gpio);
}