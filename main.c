#include <stdio.h>
#include <stdint.h>

typedef struct {
    union {
        uint32_t i;
        float f;
    };
} FloatView;

void ToggleBit(uint8_t bit_num, FloatView* f) {
    f->i = f->i ^ (1 << bit_num);    
}

void PrintFloatBits(FloatView f, uint8_t decimal_places) {
    // Print actual float number
    printf("%0.*f\n", decimal_places, f.f);

    uint32_t is_negative = f.i & (1 << 31);
    printf(is_negative ? "[1] " : "[0] ");

    printf("[");
    for(int i = 30; i > 22; --i) {
        printf(f.i & (1 << i) ? "1" : "0");
    }
    printf("] ");

    printf("[");
    for(int i = 22; i > -1; --i) {
        printf(f.i & (1 << i) ? "1" : "0");
    }
    printf("]\n");

    // // Print text
    printf(is_negative ? " -  " : " +  ");

    uint8_t exponent_value = f.i >> 23;
    int8_t exponent_value_biased = exponent_value == 0 ? -126 : exponent_value - 127;
    // Print different number of spaces based on number of decimal digits in biased exponent
    if(exponent_value_biased < 10) {
        printf("   2^%d     ", exponent_value_biased);
    }
    else if(exponent_value_biased < 100) {
        printf("   2^%d    ", exponent_value_biased);
    }
    else {
        printf("   2^%d   ", exponent_value_biased);
    }

    uint32_t mantissa_as_int = f.i & 8388607;
    float two_to_the_negative_23 = 0.00000011920928955078125f;
    float decimal_part = (float)mantissa_as_int * two_to_the_negative_23;
    if(exponent_value != 0) {
        decimal_part += 1.0f;
    }
    printf("%.23f", decimal_part);

    printf("\n");
}

int main() {
    FloatView f1;
    f1.f = 0.1f;
    PrintFloatBits(f1, 100);

    return 0;
}