#include <stdio.h>
#include <stdint.h>

void ToggleBit(uint8_t bit_num, int* value) {
    *value = *value ^ ((uint32_t)1 << bit_num);    
}

void PrintFloatBits(int* value) {
    // Print actual float number
    printf("%0.17f\n", *(float*)value);

    uint32_t is_negative = *value & (uint32_t)1 << 31;
    printf(is_negative ? "[1]        " : "[0]        ");

    printf("[");
    for(int8_t i = 30; i > 22; --i) {
        printf(*value & (uint32_t)1 << i ? "1" : "0");
    }
    printf("]        ");

    printf("[");
    for(int8_t i = 22; i > -1; --i) {
        printf(*value & (uint32_t)1 << i ? "1" : "0");
    }
    printf("]\n");

    // Print text
    printf(is_negative ? " -  " : " +  ");

    uint8_t exponent_value = *value >> 23;
    int8_t exponent_value_shifted = exponent_value - 127;
    printf("   exp: %d - 127 = %d   \n\n", exponent_value, exponent_value_shifted);

    // uint32_t significand = 0x7FFFFF & *value;
    // printf("     sig: %u\n", significand);
}

int main() {
    float f = 0.0f;
    PrintFloatBits((int*)&f);
    ToggleBit(23, (int*)&f);
    ToggleBit(24, (int*)&f);
    // ToggleBit(25, (int*)&f);
    // ToggleBit(26, (int*)&f);
    // ToggleBit(27, (int*)&f);
    // ToggleBit(28, (int*)&f);
    // ToggleBit(29, (int*)&f);
    // ToggleBit(30, (int*)&f);
    // ToggleBit(21, (int*)&f);
    PrintFloatBits((int*)&f);

    return 0;
}