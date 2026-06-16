#include "tariff.h"

float calculate_price(float base_price, int discount)
{
    if (base_price < 0.0f) {
        return 0.0f;
    }

    if (discount) {
        return base_price * 0.5f;
    }

    return base_price;
}
