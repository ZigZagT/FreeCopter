//
//  main.cpp
//  Free-Copter
//
//  Created by Master on 6/26/15.
//
//

#ifndef	__WIRING_PI_CONTROL_H__
#define	__WIRING_PI_CONTROL_H__

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

enum GPIO_PIN_MAP_T {
    A_IN = 2,
    E_IN = 3,
    T_IN = 4,
    R_IN = 5,
    U_IN = 6,
    A_OUT = 20,
    E_OUT = 21,
    T_OUT = 22,
    R_OUT = 23,
    U_OUT = 24
};
enum GPIO_PIN_AUX_T {
    AUX1 = 7,
    AUX2 = 8,
    AUX3 = 9
};

void wiringPi_start();

#endif