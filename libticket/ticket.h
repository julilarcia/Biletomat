#ifndef BILETOMAT_LIBTICKET_TICKET_H
#define BILETOMAT_LIBTICKET_TICKET_H

#include <stdio.h>

#define TICKET_NAME_LEN 64
#define TICKET_CODE_LEN 32

typedef struct {
    int id;
    char name[TICKET_NAME_LEN];
    float base_price;
    int valid_minutes;
    int discount;
} Ticket;

typedef struct {
    int ticket_id;
    char ticket_name[TICKET_NAME_LEN];
    float price;
    int valid_minutes;
    char code[TICKET_CODE_LEN];
} PrintedTicket;

int load_tickets(const char *filename, Ticket **tickets_out);
void free_tickets(Ticket *tickets);

Ticket* find_ticket_by_id(Ticket *tickets, int count, int id);

void generate_ticket_code(char *buffer, int size);
void print_ticket_info(const Ticket *ticket);
void write_printed_ticket(FILE *file, const PrintedTicket *ticket);

#endif