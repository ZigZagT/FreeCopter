//
//  main.cpp
//  Free-Copter
//
//  Created by Master on 6/26/15.
//
//

#include <iostream>
#include "wiringPi_control.h"
#include "RPI_peripheral.h"

int main(const int argc, const char** argv) {
    std::cout << "starting gpio route..." << std::endl;
    gpio_control_start();
    //wiringPi_start();
    std::cout << "Hello Copter" << std::endl;
    return 0;
}