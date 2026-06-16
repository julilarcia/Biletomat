#include "../include/logger.h"

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

void log_transaction(const char *ticket_name, float price, const char *status)
{
    mkdir("logs", 0777);

    FILE *file = fopen("logs/transakcje.log", "a");

    if (file == NULL) {
        perror("fopen logs/transakcje.log");
        return;
    }

    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);
    char date_buffer[64];

    strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d %H:%M:%S", time_info);

    fprintf(
        file,
        "%s | %s | %.2f zl | %s\n",
        date_buffer,
        ticket_name,
        price,
        status
    );

    fclose(file);
}