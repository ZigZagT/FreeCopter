//
//  config.h
//  Free-Copter
//
//  Created by Master on 7/17/15.
//
//

#ifndef CONFIG_H
#define CONFIG_H

#include <pthread.h>

#define CLOCKID CLOCK_BOOTTIME
#define CLOCK_TICK_NANOSECS (1000*1000)

#define CLOCK_THREAD_SIG SIGRTMIN
#define OUTPUT_THREAD_SIG (SIGRTMIN+1)
#define INPUT_THREAD_SIG (SIGRTMIN+2)



extern pthread_t clock_thread;
extern pthread_t output_thread;
extern pthread_t input_thread;

// Interface for other module. Implement theses function in another lib for linking.
// modules are special tool kits for different hardwares.

// initialize devices, and do other works.
extern void module_init(void *);
// terminate devices.
extern void module_term(void *);
extern void module_output_loop(void *);
extern void module_input_loop(void *);
// special module commands passed by CLI. Return false if command is not exists.
extern bool module_command(const char*);

// if true, the threads except the main thread should exits.
extern bool exit_all_threads;

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

#endif
