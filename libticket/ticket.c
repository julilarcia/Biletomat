#include "ticket.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int load_tickets(const char *filename, Ticket **tickets_out)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Nie mozna otworzyc pliku z cennikiem");
        return -1;
    }

    int capacity = 4;
    int count = 0;

    Ticket *tickets = malloc(capacity * sizeof(Ticket));

    if (tickets == NULL) {
        fclose(file);
        return -1;
    }

    char line[256];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n' || line[0] == '#' || line[0] == '\r') {
            continue;
        }

        char *fields[5];
        int field_count = 0;

        char *token = strtok(line, ";");

        while (token != NULL && field_count < 5) {
            fields[field_count] = token;
            field_count++;
            token = strtok(NULL, ";");
        }

        if (field_count < 5) {
            continue;
        }

        fields[4][strcspn(fields[4], "\r\n")] = '\0';

        if (count >= capacity) {
            capacity *= 2;

            Ticket *new_tickets = realloc(tickets, capacity * sizeof(Ticket));

            if (new_tickets == NULL) {
                free(tickets);
                fclose(file);
                return -1;
            }

            tickets = new_tickets;
        }

        tickets[count].id = atoi(fields[0]);

        strncpy(tickets[count].name, fields[1], TICKET_NAME_LEN - 1);
        tickets[count].name[TICKET_NAME_LEN - 1] = '\0';

        tickets[count].base_price = strtof(fields[2], NULL);
        tickets[count].valid_minutes = atoi(fields[3]);
        tickets[count].discount = atoi(fields[4]);

        count++;
    }

    fclose(file);

    *tickets_out = tickets;

    return count;
}

void free_tickets(Ticket *tickets)
{
    free(tickets);
}

Ticket* find_ticket_by_id(Ticket *tickets, int count, int id)
{
    int i;

    for (i = 0; i < count; i++) {
        if (tickets[i].id == id) {
            return &tickets[i];
        }
    }

    return NULL;
}

void generate_ticket_code(char *buffer, int size)
{
    long timestamp = time(NULL);

    snprintf(buffer, size, "TCK-%ld-%04d", timestamp % 100000, rand() % 10000);
}

void print_ticket_info(const Ticket *ticket)
{
    if (ticket == NULL) {
        return;
    }

    printf("Bilet: %s\n", ticket->name);
    printf("Cena bazowa: %.2f zl\n", ticket->base_price);
    printf("Czas waznosci: %d minut\n", ticket->valid_minutes);
    printf("Ulgowy: %s\n", ticket->discount ? "tak" : "nie");
}

void write_printed_ticket(FILE *file, const PrintedTicket *ticket)
{
    if (file == NULL || ticket == NULL) {
        return;
    }

    fprintf(file, "BILET KOMUNIKACJI MIEJSKIEJ\n");
    fprintf(file, "Kod: %s\n", ticket->code);
    fprintf(file, "Nazwa: %s\n", ticket->ticket_name);
    fprintf(file, "Cena: %.2f zl\n", ticket->price);
    fprintf(file, "Wazny: %d minut\n", ticket->valid_minutes);
}