#ifndef BILETOMAT_MENU_H
#define BILETOMAT_MENU_H

#include <semaphore.h>
#include "ticket.h"
#include "machine.h"

void print_main_menu(const Ticket *tickets, int count);
int read_int_from_console(const char *prompt);
void print_machine_state(MachineState *state, sem_t *sem);

#endif