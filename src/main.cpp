//
//  main.cpp
//  Free-Copter
//
//  Created by Master on 6/26/15.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#include "raspberry_pi.h"
#include "config.h"

bool exit_all_threads = false;
pthread_t clock_thread;
pthread_t output_thread;
pthread_t input_thread;

void* clock_thread_func(void *) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, CLOCK_THREAD_SIG);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
        errExit("sigprocmask: clock thread");
    
    while(!exit_all_threads) {
        pause();
        // below cannot work here, might because of NULL parameter
        // sigwait(&mask,NULL);         // cause systemd-coredump error.
        // sigwaitinfo(&mask, NULL);    // cause block forever
    }
    pthread_exit(NULL);
    return NULL;
}
void* output_thread_func(void *) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, OUTPUT_THREAD_SIG);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
        errExit("sigprocmask: output thread");
    
    while(!exit_all_threads) {
        pause();
    }
    pthread_exit(NULL);
    return NULL;
}
void* input_thread_func(void *) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, INPUT_THREAD_SIG);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
        errExit("sigprocmask: input thread");
    
    while(!exit_all_threads) {
        pause();
    }
    pthread_exit(NULL);
    return NULL;
}
void sa_sigaction_clock(int, siginfo_t * info, void *) {
    // debug use only
    // printf("clock thread get signal %d\n", info->si_signo);
    // fflush(stdout);
    pthread_kill(output_thread, OUTPUT_THREAD_SIG);
    pthread_kill(input_thread, INPUT_THREAD_SIG);
}
void sa_sigaction_output(int, siginfo_t * info, void *) {
    // debug only
    // printf("output thread get signal %d\n", info->si_signo);
    // fflush(stdout);
    module_output_loop(NULL);
}
void sa_sigaction_input(int, siginfo_t * info, void *) {
    // debug only
    // printf("input thread get signal %d\n", info->si_signo);
    // fflush(stdout);
    module_input_loop(NULL);
}

int main(const int argc, const char** argv) {
    timer_t timerid;
    struct itimerspec its;
    struct sigevent sigev;
    sigset_t mask;
    struct sigaction sa_clock_sig;
    struct sigaction sa_output_thread_sig;
    struct sigaction sa_input_thread_sig;

    // Using printf rather than cout in multi threads evironment.
    printf("fc starting...\n");
    // And flush stdout to make sure the msg displayed.
    fflush(stdout);
    
    // init signal action and handler func
    sa_clock_sig.sa_flags = SA_SIGINFO;
    sigemptyset(&sa_clock_sig.sa_mask);
    sa_clock_sig.sa_sigaction = sa_sigaction_clock;
    sa_output_thread_sig = sa_clock_sig;
    sa_input_thread_sig = sa_clock_sig;
    sa_output_thread_sig.sa_sigaction = sa_sigaction_output;
    sa_input_thread_sig.sa_sigaction = sa_sigaction_input;
    
    if (sigaction(CLOCK_THREAD_SIG, &sa_clock_sig, NULL) == -1)
        errExit("sigaction: clock");
    if (sigaction(OUTPUT_THREAD_SIG, &sa_output_thread_sig, NULL) == -1)
        errExit("sigaction: output");
    if (sigaction(INPUT_THREAD_SIG, &sa_input_thread_sig, NULL) == -1)
        errExit("sigaction: input");
    
    // block all signals
    sigfillset(&mask);
    if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
        errExit("sigprocmask: main");
    
    // init timer settings and start timer
    memset(&sigev, 0, sizeof(sigev));
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = CLOCK_THREAD_SIG;
    sigev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCKID, &sigev, &timerid) == -1)
        errExit("timer_create");
    its.it_value.tv_sec = CLOCK_TICK_NANOSECS / (1000 * 1000 * 1000);
    its.it_value.tv_nsec = CLOCK_TICK_NANOSECS % (1000 * 1000 * 1000);
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    if (timer_settime(timerid, 0, &its, NULL) == -1)
        errExit("timer_settime");
    
    // init module. IMPORTANT: module_init() should be called before threads start to initialize devices.
    module_init(NULL);
    
    // start threads
    pthread_create(&clock_thread, NULL, clock_thread_func, NULL);
    pthread_create(&output_thread, NULL, output_thread_func, NULL);
    pthread_create(&input_thread, NULL, input_thread_func, NULL);
    pthread_detach(clock_thread);
    pthread_detach(output_thread);
    pthread_detach(input_thread);
    
    // start CLI User interface
    for(;;) {
        char input[500];
        printf("fc> ");
        scanf("%s", input);
        if(strcmp(input, "input") == 0) {
            kill(getpid(), INPUT_THREAD_SIG);
        } else if(strcmp(input, "output") == 0) {
            kill(getpid(), OUTPUT_THREAD_SIG);
        } else if(strcmp(input, "clock") == 0) {
            kill(getpid(), CLOCK_THREAD_SIG);
        } else if(strcmp(input, "sig") == 0) {
            kill(getpid(), INPUT_THREAD_SIG);
            kill(getpid(), OUTPUT_THREAD_SIG);
        } else if(strcmp(input, "timer") == 0) {
            // Always returns 0, don not know the reason.
            printf("timer overrun = %d\n", timer_getoverrun(timerid));
        } else if(strcmp(input, "exit") == 0) {
            printf("User exiting...\n");
            fflush(stdout);
            
            // threads must be terminated before module_term.
            exit_all_threads = true;
            usleep(5000);
            module_term(NULL);
            usleep(5000);
            exit(0);
        } else {
            if (!module_command(input))
                printf("invalid command.\n");
        }
        fflush(stdout);
        usleep(5000);
    }
    
    return 0;
}