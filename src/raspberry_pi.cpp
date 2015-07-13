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
static volatile bool gpio_route = true; // if false, gpio out could be adjusted outside this file.
#define gpio_input_pin_mask  (0b11111111 << 2)
#define gpio_output_pin_mask (0b11111 << 20)

struct bcm2708_peripheral gpio = {GPIO_BASE};
struct gpio_status gpio_in;
struct gpio_status gpio_out;
volatile double input_pwm_min = 1;
volatile double input_pwm_max = 2;


static int map_peripheral(struct bcm2708_peripheral *p);
static void unmap_peripheral(struct bcm2708_peripheral *p);

static void raspberry_pi_init();
static void raspberry_pi_clock();
static void raspberry_pi_gpio_route();
static inline void do_gpio_route();
static inline void gpio_in_generate(unsigned gpio_bits_8); // A E T R U and Aux1 to Aux3
static inline void gpio_out_generate();
static void raspberry_pi_gpio_pwm();
static inline void do_gpio_pwm();
static void raspberry_pi_gpio_output();
static inline void do_gpio_output();

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
    std::cout << "raspberry_pi_init" << std::endl;
    map_peripheral(&gpio);
    // input pin is gpio 02-09, as A E T R U Aux1 Aux2 Aux3
    const unsigned init_input_pin_init =  ~(0b111111111111111111111111 << (2 * 3));
    // output pin is gpio 20-24 as A E T R U
    const unsigned init_output_pin_init = ~(0b111111111111111);
    const unsigned init_output_pin_set =    0b001001001001001;
    gpio.addr[0] &= init_input_pin_init;
    gpio.addr[2] &= init_output_pin_init;
    gpio.addr[2] |= init_output_pin_set;
    
    std::cout << "raspberry_pi_init: thread start" << std::endl;

    std::thread clock_tick_thread(raspberry_pi_clock);
    std::thread gpio_route_thread(raspberry_pi_gpio_route);
    std::thread gpio_output_thread(raspberry_pi_gpio_output);
    
    clock_tick_thread.detach();
    gpio_route_thread.detach();
    gpio_output_thread.detach();
    
    std::cout << "raspberry_pi_init: return" << std::endl;
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
    // gpio.addr[7] = ((gpio.addr[13] << 18) & gpio_output_pin_mask);
    // gpio.addr[10] = ((~gpio.addr[13] << 18) & gpio_output_pin_mask);
    
    gpio_in_generate(gpio.addr[13] >> 2);
    gpio_out_generate();
}

static inline void gpio_in_generate(unsigned gpio_bits_8) {
    static unsigned ch_count[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    static bool ch_clr[8] = {false, false, false, false, false, false, false, false};
    for (int i = 0; i < 8; ++i) {
        if (gpio_bits_8 & (1 << i)) {
            ++ch_count[i];
            ch_clr[i] = false;
        } else if (ch_count[i] != 0) {
            if (ch_clr[i]) {
                unsigned level = (((double)ch_count[i] / 1000 - input_pwm_min) / (input_pwm_max - input_pwm_min)) * 2000;
                ch_count[i] = 0;
                ch_clr[i] = false;
                switch (i) {
                    case 0:
                        gpio_in.A = level;
                        break;
                    case 1:
                        gpio_in.E = level;
                        break;
                    case 2:
                        gpio_in.T = level;
                        break;
                    case 3:
                        gpio_in.R = level;
                        break;
                    case 4:
                        gpio_in.U = level;
                        break;
                    case 5:
                        gpio_in.Aux1 = level;
                        break;
                    case 6:
                        gpio_in.Aux2 = level;
                        break;
                    case 7:
                        gpio_in.Aux3 = level;
                        break;
                    default:
                        break;
                }
            } else {
                ch_clr[i] = true;
            }
        }
    }
}

static inline void gpio_out_generate() {
    /*static deque<gpio_status> list(5, gpio_in);
    static unsigned count = 0;
    static bool check[5] = {false, false, false, false, false};
    
    list.pop_front();
    list.push_back(gpio_in);
    
    unsigned diff[5][2] = {
        {list[4].A - list[0].A, list[4].A - list[3].A},
        {list[4].E - list[0].E, list[4].E - list[3].E},
        {list[4].T - list[0].T, list[4].T - list[3].T},
        {list[4].R - list[0].R, list[4].R - list[3].R},
        {list[4].U - list[0].U, list[4].U - list[3].U}
    };
    
    for (int i = 0; i < 5; ++i) {
        if (diff[i][1] - diff[i][0] >= 50) {
            check[i] = true;
        }
    }*/
    
    gpio_out = gpio_in;
    
    /*route_in_to_out(gpio_in.A, gpio_out.A);
    route_in_to_out(gpio_in.E, gpio_out.E);
    route_in_to_out(gpio_in.T, gpio_out.T);
    route_in_to_out(gpio_in.R, gpio_out.R);
    route_in_to_out(gpio_in.U, gpio_out.U);
    route_in_to_out(gpio_in.Aux1, gpio_out.Aux1);
    route_in_to_out(gpio_in.Aux2, gpio_out.Aux2);
    route_in_to_out(gpio_in.Aux3, gpio_out.Aux3);*/
    
}

static inline void do_gpio_pwm() {
    return;
}

static void raspberry_pi_gpio_output() {
    std::mutex mtx;
    for (;;) {
        std::unique_lock<std::mutex> lck(mtx);
        clock_tick.wait(lck);
        do_gpio_output();
        if (!ready) {
            return;
        }
    }
    
}

static inline double level_2_us(volatile unsigned level) {
    return ((((double)level / 2000) * (input_pwm_max - input_pwm_min)) + input_pwm_min) * 1000;
}

static inline void do_gpio_output() {
    static unsigned output_count = 0;
    static double A = level_2_us(gpio_out.A);
    static double E = level_2_us(gpio_out.E);
    static double T = level_2_us(gpio_out.T);
    static double R = level_2_us(gpio_out.R);
    static double U = level_2_us(gpio_out.U);
    
    ++output_count;
    if (output_count >= 10 * 1000) {
        output_count = 0;
        gpio.addr[7] = (0b11111 << 20);
        A = level_2_us(gpio_out.A);
        E = level_2_us(gpio_out.E);
        T = level_2_us(gpio_out.T);
        R = level_2_us(gpio_out.R);
        U = level_2_us(gpio_out.U);
    // gpio.addr[7] = ((gpio.addr[13] << 18) & gpio_output_pin_mask);
    // gpio.addr[10] = ((~gpio.addr[13] << 18) & gpio_output_pin_mask);
    } else {
        if (output_count >= A) {
            gpio.addr[10] = 0b1 << 20;
        }
        if (output_count >= E) {
            gpio.addr[10] = 0b10 << 20;
        }
        if (output_count >= T) {
            gpio.addr[10] = 0b100 << 20;
        }
        if (output_count >= R) {
            gpio.addr[10] = 0b1000 << 20;
        }
        if (output_count >= U) {
            gpio.addr[10] = 0b10000 << 20;
        }
    }
}

void raspberry_pi_start() {
    std::cout << "raspberry_pi_start" << std::endl;
    raspberry_pi_init();
    ready = true;
}

void raspberry_pi_term() {
    ready = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    unmap_peripheral(&gpio);
}