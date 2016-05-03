#include <atomic>
#include <cstring>
#include <signal.h>

#include "interruption.h"

using namespace std;

volatile sig_atomic_t interruption_requested = 0;

static void HandleInterrupt(int sig) {
  interruption_requested++;
}

bool InterruptRequested() {
  if (interruption_requested > 0) {
    interruption_requested--;
    return true;
  }
  return false;
}

void InitSignals() {
  struct sigaction int_handler;
  memset(&int_handler, 0, sizeof(int_handler));

  int_handler.sa_handler = HandleInterrupt;

  sigaction(SIGINT, &int_handler, 0);
}
