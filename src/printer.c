#include "../include/printer.h"
#include "../include/shared_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

static int write_all(int fd, const void *buffer, size_t size)
{
    const char *ptr = (const char*)buffer;
    size_t written_total = 0;

    while (written_total < size) {
        ssize_t written = write(fd, ptr + written_total, size - written_total);

        if (written <= 0) {
            return -1;
        }

        written_total += written;
    }

    return 0;
}

static int read_all(int fd, void *buffer, size_t size)
{
    char *ptr = (char*)buffer;
    size_t read_total = 0;

    while (read_total < size) {
        ssize_t bytes_read = read(fd, ptr + read_total, size - read_total);

        if (bytes_read <= 0) {
            return -1;
        }

        read_total += bytes_read;
    }

    return 0;
}

int start_printer_process(const PrintedTicket *ticket, MachineState *state, sem_t *sem)
{
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return -1;
    }

    fflush(NULL);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }

    if (pid == 0) {
        close(pipe_fd[1]);

        PrintedTicket received_ticket;

        if (read_all(pipe_fd[0], &received_ticket, sizeof(PrintedTicket)) == -1) {
            perror("read");
            close(pipe_fd[0]);
            exit(1);
        }

        close(pipe_fd[0]);

        printf("[DRUKARKA %d] Odebrano dane biletu.\n", getpid());
        printf("[DRUKARKA %d] Drukowanie...\n", getpid());

        sleep(2);

        state_lock(sem);

        if (state->paper_count <= 0) {
            state_unlock(sem);
            printf("[DRUKARKA %d] Brak papieru.\n", getpid());
            exit(2);
        }

        state->paper_count--;

        state_unlock(sem);

        mkdir("tickets", 0777);

        FILE *file = fopen("tickets/wydrukowane_bilety.txt", "a");

        if (file == NULL) {
            perror("fopen tickets/wydrukowane_bilety.txt");
            exit(3);
        }

        time_t now = time(NULL);
        struct tm *time_info = localtime(&now);
        char date_buffer[64];

        strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d %H:%M:%S", time_info);

        fprintf(file, "Data wydruku: %s\n", date_buffer);
        write_printed_ticket(file, &received_ticket);

        fclose(file);

        printf("[DRUKARKA %d] Bilet zostal wydrukowany.\n", getpid());

        exit(0);
    }

    close(pipe_fd[0]);

    if (write_all(pipe_fd[1], ticket, sizeof(PrintedTicket)) == -1) {
        perror("write");
        close(pipe_fd[1]);
        return -1;
    }

    close(pipe_fd[1]);

    int status;

    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }

    return -1;
}