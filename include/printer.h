#ifndef BILETOMAT_PRINTER_H
#define BILETOMAT_PRINTER_H

#include <semaphore.h>
#include "ticket.h"
#include "machine.h"

int start_printer_process(const PrintedTicket *ticket, MachineState *state, sem_t *sem);

#endif
