#include "signals.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief This function manages the signals.
 */
void manage_signals(void)
{
    signal(SIGINT, signal_interrupt_handler);
    signal(SIGTSTP, signal_stop_handler);
    signal(SIGQUIT, signal_quit_handler);
}

/**
 * @brief This function handles the signal SIGINT.
 */
void signal_interrupt_handler(int signum1)
{
    (void)signum1;
    printf("\n");
    fflush(stdout);
}

/**
 * @brief This function handles the signal SIGTSTP.
 */
void signal_stop_handler(int signum2)
{
    (void)signum2;
    printf("\n");
    fflush(stdout);
}

/**
 * @brief This function handles the signal SIGQUIT.
 */
void signal_quit_handler(int signum3)
{
    (void)signum3;
    printf("\n");
    fflush(stdout);
}
