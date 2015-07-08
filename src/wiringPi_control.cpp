//
//  main.cpp
//  Free-Copter
//
//  Created by Master on 6/26/15.
//
//

#include "wiringPi_control.h"

static void gpio_pin_init() {
    if (wiringPiSetupGpio() != 0) { // Using BCM PINOUT. For wiringpi virsion 2, this function will always returns 0.
        exit(1);
    }
    
    pinMode(A_IN,INPUT);
    pinMode(E_IN,INPUT);
    pinMode(T_IN,INPUT);
    pinMode(R_IN,INPUT);
    pinMode(U_IN,INPUT);
    pinMode(A_OUT,OUTPUT);
    pinMode(E_OUT,OUTPUT);
    pinMode(T_OUT,OUTPUT);
    pinMode(R_OUT,OUTPUT);
    pinMode(U_OUT,OUTPUT);
    
    pinMode(AUX1,INPUT);
    pinMode(AUX2,INPUT);
    pinMode(AUX3,INPUT);
}

void wiringPi_start() {
    gpio_pin_init();
    while (1) {
        digitalWrite(A_OUT, digitalRead(A_IN));
        digitalWrite(E_OUT, digitalRead(E_IN));
        digitalWrite(T_OUT, digitalRead(T_IN));
        digitalWrite(R_OUT, digitalRead(R_IN));
        digitalWrite(U_OUT, digitalRead(U_IN));
    }
}