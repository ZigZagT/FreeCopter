//
//  gpio_mem_dump.cpp
//  Free-Copter
//
//  Created by Master on 7/6/15.
//
//

#include "gpio_mem_dump.h"

char* unsigned_to_binary(unsigned num) {
    unsigned int flag = 1;
    char* str = new char[33];
    for (int bit = 0; bit < 32; ++bit) {
        if ((num & flag)) {
            str[31 - bit] = '1';
        } else {
            str[31 - bit] = '0';
        }
        flag = flag << 1;
    }
    str[32] = '\0';
    return str;
}

void gpio_mem_dump () {
    std::cout << "gpio memory dump: " << std::endl;
    for (int i = 0; i < 15; ++i) {
        std::cout << i << ":\t" << unsigned_to_binary(gpio.addr[i]) << std::endl;
    }
    std::cout << "LEV:\t" << unsigned_to_binary(gpio.addr[13]) << std::endl;
}
