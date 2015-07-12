//
//  main.cpp
//  Free-Copter
//
//  Created by Master on 6/26/15.
//
//

#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <thread>
#include "raspberry_pi.h"

void SIGINThandler(int x) {
    raspberry_pi_term();
    std::cout << "user exiting... " << std::endl;
    exit(0);
}

int main(const int argc, const char** argv) {
    std::cout << "starting gpio route..." << std::endl;
    signal(SIGINT, SIGINThandler);
    signal(SIGTERM, SIGINThandler);
    
    raspberry_pi_start();
    std::this_thread::sleep_for(std::chrono::hours(1));
    return 0;
}