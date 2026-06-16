#ifndef BILETOMAT_SHARED_STATE_H
#define BILETOMAT_SHARED_STATE_H

#include <semaphore.h>
#include "machine.h"

MachineState* init_shared_state(sem_t **sem);
void cleanup_shared_state(MachineState *state, sem_t *sem);

void state_lock(sem_t *sem);
void state_unlock(sem_t *sem);

#endif