#ifndef INC_SIGNAL_HELPER
#define INC_SIGNAL_HELPER

#include <signal.h>

int handle_signal(int signal, void (*handler)(int));
int handle_signal_detail(int signal, void (*handler)(int, siginfo_t *, void *), int flags);

#endif /* INC_SIGNAL_HELPER */
