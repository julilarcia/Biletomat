#include "shared_state.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define SHM_NAME "/biletomat_shared_memory"
#define SEM_NAME "/biletomat_semaphore"

MachineState* init_shared_state(sem_t **sem)
{
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

    if (shm_fd == -1) {
        perror("shm_open");
        return NULL;
    }

    if (ftruncate(shm_fd, sizeof(MachineState)) == -1) {
        perror("ftruncate");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return NULL;
    }

    MachineState *state = mmap(
        NULL,
        sizeof(MachineState),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_fd,
        0
    );

    close(shm_fd);

    if (state == MAP_FAILED) {
        perror("mmap");
        shm_unlink(SHM_NAME);
        return NULL;
    }

    state->paper_count = 10;
    state->cash_inside = 0.0f;
    state->sold_tickets = 0;
    state->machine_active = 1;

    *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    if (*sem == SEM_FAILED) {
        perror("sem_open");
        munmap(state, sizeof(MachineState));
        shm_unlink(SHM_NAME);
        return NULL;
    }

    return state;
}

void cleanup_shared_state(MachineState *state, sem_t *sem)
{
    if (state != NULL) {
        munmap(state, sizeof(MachineState));
    }

    if (sem != NULL && sem != SEM_FAILED) {
        sem_close(sem);
    }

    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);
}

void state_lock(sem_t *sem)
{
    if (sem != NULL && sem != SEM_FAILED) {
        sem_wait(sem);
    }
}

void state_unlock(sem_t *sem)
{
    if (sem != NULL && sem != SEM_FAILED) {
        sem_post(sem);
    }
}