#include "../include/payment.h"

#include <stdio.h>

static void clear_input(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int process_payment(float required_amount, float *paid_amount, float *change)
{
    float inserted = 0.0f;

    printf("\n");
    printf("Cena biletu: %.2f zl\n", required_amount);
    printf("Symulacja platnosci.\n");
    printf("Wpisuj wrzucane kwoty, np. 1, 2, 5.\n");
    printf("Wpisz 0, aby anulowac transakcje.\n");

    while (inserted + 0.001f < required_amount) {
        float coin;

        printf("Pozostalo do zaplaty: %.2f zl\n", required_amount - inserted);
        printf("> ");

        if (scanf("%f", &coin) != 1) {
            printf("Niepoprawna wartosc.\n");
            clear_input();
            continue;
        }

        clear_input();

        if (coin == 0.0f) {
            *paid_amount = inserted;
            *change = 0.0f;
            return 0;
        }

        if (coin < 0.0f) {
            printf("Kwota nie moze byc ujemna.\n");
            continue;
        }

        inserted += coin;
        printf("Wrzucono lacznie: %.2f zl\n", inserted);
    }

    *paid_amount = inserted;
    *change = inserted - required_amount;

    return 1;
}