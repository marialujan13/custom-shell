#include <signal.h>
#include <stdio.h>

/**
 * @brief This function manages the signals.
 */
void manage_signals(void);

/**
 * @brief This function handles the signals.
 * @param signum1 the signal number.
 */
void signal_interrupt_handler(int signum1);

/**
 * @brief This function handles the signals.
 * @param signum2 the signal number.
 */
void signal_stop_handler(int signum2);

/**
 * @brief This function handles the signals.
 * @param signum3 the signal number.
 */
void signal_quit_handler(int signum3);
