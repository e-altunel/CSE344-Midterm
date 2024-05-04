#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <signal.h>

int handle_signal(int signal, void (*handler)(int)) {
  struct sigaction sa;

  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  return sigaction(signal, &sa, 0);
}

int handle_signal_detail(int signal, void (*handler)(int, siginfo_t *, void *), int flags) {
  struct sigaction sa;

  sa.sa_sigaction = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = flags | SA_SIGINFO;

  return sigaction(signal, &sa, 0);
}