#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>
#include <semaphore.h>

#include "../include/ticket.h"
#include "../include/menu.h"
#include "../include/payment.h"
#include "../include/printer.h"
#include "../include/shared_state.h"
#include "../include/logger.h"

typedef float (*CalculatePriceFunction)(float, int);

typedef struct {
    MachineState *state;
    sem_t *sem;
    int *running;
    pthread_mutex_t *mutex;
} MonitorArgs;

static int is_monitor_running(MonitorArgs *args)
{
    int value;

    pthread_mutex_lock(args->mutex);
    value = *(args->running);
    pthread_mutex_unlock(args->mutex);

    return value;
}

static void* monitor_machine(void *arg)
{
    MonitorArgs *args = (MonitorArgs*)arg;

    while (is_monitor_running(args)) {
        MachineState copy;

        state_lock(args->sem);

        copy.paper_count = args->state->paper_count;
        copy.cash_inside = args->state->cash_inside;
        copy.sold_tickets = args->state->sold_tickets;
        copy.machine_active = args->state->machine_active;

        state_unlock(args->sem);

        if (!copy.machine_active) {
            break;
        }

        if (copy.paper_count <= 3) {
            printf("\n[MONITOR] Uwaga: niski poziom papieru: %d szt.\n", copy.paper_count);
        }

        sleep(5);
    }

    return NULL;
}

static void add_paper_service(MachineState *state, sem_t *sem)
{
    int amount = read_int_from_console("Ile sztuk papieru dodac? ");

    if (amount <= 0) {
        printf("Nie dodano papieru.\n");
        return;
    }

    state_lock(sem);
    state->paper_count += amount;
    state_unlock(sem);

    printf("Dodano %d szt. papieru.\n", amount);
}

static int machine_has_paper(MachineState *state, sem_t *sem)
{
    int result;

    state_lock(sem);
    result = state->paper_count > 0;
    state_unlock(sem);

    return result;
}

int main(void)
{
    srand((unsigned int)time(NULL));

    Ticket *tickets = NULL;
    int ticket_count = load_tickets("data/cennik.txt", &tickets);

    if (ticket_count <= 0) {
        printf("Nie udalo sie wczytac cennika.\n");
        return 1;
    }

    void *tariff_handle = dlopen("./build/libtariff.so", RTLD_LAZY);

    if (tariff_handle == NULL) {
        printf("Blad ladowania biblioteki dynamicznej: %s\n", dlerror());
        free_tickets(tickets);
        return 1;
    }

    dlerror();

    CalculatePriceFunction calculate_price =
        (CalculatePriceFunction)dlsym(tariff_handle, "calculate_price");

    char *error = dlerror();

    if (error != NULL) {
        printf("Blad pobierania funkcji calculate_price: %s\n", error);
        dlclose(tariff_handle);
        free_tickets(tickets);
        return 1;
    }

    sem_t *sem = NULL;
    MachineState *state = init_shared_state(&sem);

    if (state == NULL) {
        printf("Nie udalo sie utworzyc pamieci wspoldzielonej.\n");
        dlclose(tariff_handle);
        free_tickets(tickets);
        return 1;
    }

    pthread_t monitor_thread;
    pthread_mutex_t monitor_mutex;
    int monitor_running = 1;

    pthread_mutex_init(&monitor_mutex, NULL);

    MonitorArgs monitor_args;
    monitor_args.state = state;
    monitor_args.sem = sem;
    monitor_args.running = &monitor_running;
    monitor_args.mutex = &monitor_mutex;

    pthread_create(&monitor_thread, NULL, monitor_machine, &monitor_args);

    int running = 1;

    while (running) {
        print_main_menu(tickets, ticket_count);

        int choice = read_int_from_console("Wybierz opcje: ");

        if (choice == 0) {
            running = 0;
            continue;
        }

        if (choice == 90) {
            print_machine_state(state, sem);
            continue;
        }

        if (choice == 91) {
            add_paper_service(state, sem);
            continue;
        }

        Ticket *selected_ticket = find_ticket_by_id(tickets, ticket_count, choice);

        if (selected_ticket == NULL) {
            printf("Nie ma takiego biletu.\n");
            continue;
        }

        if (!machine_has_paper(state, sem)) {
            printf("Brak papieru. Automat nie moze wydrukowac biletu.\n");
            log_transaction(selected_ticket->name, 0.0f, "BRAK_PAPIERU");
            continue;
        }

        float price = calculate_price(
            selected_ticket->base_price,
            selected_ticket->discount
        );

        printf("\nWybrano bilet:\n");
        print_ticket_info(selected_ticket);
        printf("Cena do zaplaty: %.2f zl\n", price);

        float paid_amount = 0.0f;
        float change = 0.0f;

        int payment_ok = process_payment(price, &paid_amount, &change);

        if (!payment_ok) {
            printf("Transakcja anulowana. Zwrot: %.2f zl\n", paid_amount);
            log_transaction(selected_ticket->name, price, "ANULOWANO");
            continue;
        }

        PrintedTicket printed_ticket;

        printed_ticket.ticket_id = selected_ticket->id;

        strncpy(
            printed_ticket.ticket_name,
            selected_ticket->name,
            TICKET_NAME_LEN - 1
        );

        printed_ticket.ticket_name[TICKET_NAME_LEN - 1] = '\0';

        printed_ticket.price = price;
        printed_ticket.valid_minutes = selected_ticket->valid_minutes;

        generate_ticket_code(printed_ticket.code, TICKET_CODE_LEN);

        int print_status = start_printer_process(&printed_ticket, state, sem);

        if (print_status == 0) {
            state_lock(sem);
            state->cash_inside += price;
            state->sold_tickets++;
            state_unlock(sem);

            printf("Platnosc przyjeta.\n");

            if (change > 0.001f) {
                printf("Reszta: %.2f zl\n", change);
            }

            printf("Bilet wydrukowany poprawnie.\n");

            log_transaction(selected_ticket->name, price, "OK");
        } else {
            printf("Nie udalo sie wydrukowac biletu. Zwrot: %.2f zl\n", paid_amount);
            log_transaction(selected_ticket->name, price, "BLAD_DRUKU");
        }
    }

    state_lock(sem);
    state->machine_active = 0;
    state_unlock(sem);

    pthread_mutex_lock(&monitor_mutex);
    monitor_running = 0;
    pthread_mutex_unlock(&monitor_mutex);

    pthread_join(monitor_thread, NULL);

    pthread_mutex_destroy(&monitor_mutex);

    cleanup_shared_state(state, sem);

    dlclose(tariff_handle);
    free_tickets(tickets);

    printf("Biletomat zakonczyl dzialanie.\n");

    return 0;
}