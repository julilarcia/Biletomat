#include <stdio.h>
#include <semaphore.h>

#include "menu.h"
#include "ticket.h"
#include "machine.h"
#include "shared_state.h"

static void clear_input(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void print_main_menu(const Ticket *tickets, int count)
{
    int i;

    printf("\n");
    printf("====================================\n");
    printf("        BILETOMAT MIEJSKI\n");
    printf("====================================\n");

    for (i = 0; i < count; i++) {
        float display_price = tickets[i].discount
            ? tickets[i].base_price * 0.5f
            : tickets[i].base_price;

        printf("%d. %s - %.2f zl\n",
               tickets[i].id,
               tickets[i].name,
               display_price);
    }

    printf("------------------------------------\n");
    printf("90. Pokaz stan biletomatu\n");
    printf("91. Doladuj papier - tryb serwisowy\n");
    printf("0. Wyjscie\n");
    printf("====================================\n");
}

int read_int_from_console(const char *prompt)
{
    int value;

    printf("%s", prompt);

    while (scanf("%d", &value) != 1) {
        printf("Niepoprawna wartosc. Sprobuj jeszcze raz: ");
        clear_input();
    }

    clear_input();

    return value;
}

void print_machine_state(MachineState *state, sem_t *sem)
{
    MachineState copy;

    state_lock(sem);

    copy.paper_count = state->paper_count;
    copy.cash_inside = state->cash_inside;
    copy.sold_tickets = state->sold_tickets;
    copy.machine_active = state->machine_active;

    state_unlock(sem);

    printf("\n");
    printf("===== STAN BILETOMATU =====\n");
    printf("Papier: %d szt.\n", copy.paper_count);
    printf("Gotowka w automacie: %.2f zl\n", copy.cash_inside);
    printf("Sprzedane bilety: %d\n", copy.sold_tickets);
    printf("Status: %s\n", copy.machine_active ? "aktywny" : "wylaczony");
    printf("===========================\n");
}