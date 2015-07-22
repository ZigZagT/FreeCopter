//
//  raspberry_pi.cpp
//  Free-Copter
//
//  Created by Master on 7/5/15.
//
//

#include "raspberry_pi.h"

#define gpio_input_pin_mask  (0b11111111 << 2)
#define gpio_output_pin_mask (0b11111 << 20)

struct bcm2708_peripheral gpio = {GPIO_BASE};
struct gpio_status gpio_in;
struct gpio_status gpio_out;
volatile double input_pwm_min = 1;
volatile double input_pwm_max = 2;

void module_init(void *);
void module_term(void *);
void module_output_loop(void *);
void module_input_loop(void *);
bool module_command(const char*);

static int map_peripheral(struct bcm2708_peripheral *p);
static void unmap_peripheral(struct bcm2708_peripheral *p);
static void gpio_in_generate(unsigned gpio_bits_8); // A E T R U and Aux1 to Aux3
static void gpio_out_generate();



// Exposes the physical address defined in the passed structure using mmap on /dev/mem

static int map_peripheral(struct bcm2708_peripheral *p) {
    // Open /dev/mem
    if ((p->mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        printf("Failed to open /dev/mem, try checking permissions.\n");
        return -1;
    }

    p->map = mmap(
            NULL,
            BLOCK_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            p->mem_fd, // File descriptor to physical memory virtual file '/dev/mem'
            p->addr_p // Address in physical map that we want this memory block to expose
            );

    if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    p->addr = (volatile unsigned int *) p->map;
    return 0;
}

static void unmap_peripheral(struct bcm2708_peripheral *p) {
    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}

void module_init(void*) {
    map_peripheral(&gpio);
    // input pin is gpio 02-09, as A E T R U Aux1 Aux2 Aux3
    const unsigned init_input_pin_init = ~(0b111111111111111111111111 << (2 * 3));
    // output pin is gpio 20-24 as A E T R U
    const unsigned init_output_pin_init = ~(0b111111111111111);
    const unsigned init_output_pin_set = 0b001001001001001;
    gpio.addr[0] &= init_input_pin_init;
    gpio.addr[2] &= init_output_pin_init;
    gpio.addr[2] |= init_output_pin_set;
    
    
}

void module_input_loop(void*) {
    gpio_in_generate(gpio.addr[13] >> 2);
    gpio_out_generate();
}

static void gpio_in_generate(unsigned gpio_bits_8) {
    return;
    /*static unsigned ch_count[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    static bool ch_clr[8] = {false, false, false, false, false, false, false, false};
    for (int i = 0; i < 8; ++i) {
        if (gpio_bits_8 & (1 << i)) {
            ++ch_count[i];
            ch_clr[i] = false;
        } else if (ch_count[i] != 0) {
            if (ch_clr[i]) {
                unsigned level = (((double) ch_count[i] / 1000 - input_pwm_min) / (input_pwm_max - input_pwm_min)) * 2000;
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
    }*/
}

static void gpio_out_generate() {
    /*volatilestatic deque<gpio_status> list(5, gpio_in);
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

    // gpio_out = gpio_in;
    return;

    /*route_in_to_out(gpio_in.A, gpio_out.A);
    route_in_to_out(gpio_in.E, gpio_out.E);
    route_in_to_out(gpio_in.T, gpio_out.T);
    route_in_to_out(gpio_in.R, gpio_out.R);
    route_in_to_out(gpio_in.U, gpio_out.U);
    route_in_to_out(gpio_in.Aux1, gpio_out.Aux1);
    route_in_to_out(gpio_in.Aux2, gpio_out.Aux2);
    route_in_to_out(gpio_in.Aux3, gpio_out.Aux3);*/

}

static inline double level_2_us(volatile unsigned level) {
    return ((((double) level / 2000) * (input_pwm_max - input_pwm_min)) + input_pwm_min) * 1000;
}

volatile unsigned ddcount = 0;

void module_output_loop(void*) {
    //printf("outcount: %7d\n", outcount);
    //volatile static unsigned count = 0;
    ++ddcount;
    if (ddcount % 1000 == 0) {
        printf("count: loop for %d times\n", ddcount);
        fflush(stdout);
        ddcount = 0;
    }
    /*static unsigned output_count = 0;
    static volatile double A = level_2_us(gpio_out.A);
    static volatile double E = level_2_us(gpio_out.E);
    static volatile double T = level_2_us(gpio_out.T);
    static volatile double R = level_2_us(gpio_out.R);
    static volatile double U = level_2_us(gpio_out.U);
    ++output_count;
    // printf("output_count: %d\n", output_count);
    if (output_count >= 10 * 1000) {
        output_count = 0;
        gpio.addr[7] = (0b11111 << 20);
        A = level_2_us(gpio_out.A);
        E = level_2_us(gpio_out.E);
        T = level_2_us(gpio_out.T);
        R = level_2_us(gpio_out.R);
        U = level_2_us(gpio_out.U);
        printf("refresh output: A=%f, E=%f, T=%f, R=%f, U=%f\n", A, E, T, R, U);
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
    }*/
}

void module_term(void*) {
    unmap_peripheral(&gpio);
}

static inline void dump_gpio_status(const struct gpio_status &status, const char* title) {
    printf("dump for %s: \n"
            "A:    %5d       E: %5d\n"
            "T:    %5d       R: %5d\n"
            "U:    %5d\n"
            "Aux1: %5d    Aux2: %5d\n"
            "Aux3: %5d\n"
            , title, status.A, status.E, status.T, status.R, status.U, status.Aux1, status.Aux2, status.Aux3);
}

bool module_command(const char* str) {
    if (strcmp(str, "dumpin") == 0) {
        dump_gpio_status(gpio_in, "gpio_in");
    } else if (strcmp(str, "dumpout") == 0) {
        dump_gpio_status(gpio_out, "gpio_out");
    } else if (strcmp(str, "dump") == 0) {
        dump_gpio_status(gpio_in, "gpio_in");
        dump_gpio_status(gpio_out, "gpio_out");
    } else if (strcmp(str, "count") == 0) {
        printf("count: %7d\n", ddcount);
    } else {
        return false;
    }
    return true;
}