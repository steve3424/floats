#include <stdio.h>
#include <stdint.h>

void ToggleBit(uint8_t bit_num, int* value) {
    *value = *value ^ ((uint32_t)1 << bit_num);    
}

void PrintFloatBits(int* value) {
    // Print actual float number
    printf("\n%0.17f\n", *(float*)value);

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
    printf("]");
    printf("\n");


    // Print text
    printf(is_negative ? " -  " : " +  ");

    int8_t exponent_value = *value >> 23;
    printf("   exp: %d - 127 = %d   ", exponent_value, exponent_value - 127);

    // uint32_t significand = 0x7FFFFF & *value;
    // printf("     sig: %u\n", significand);

}

int main() {
    float f = 0.0f;
    PrintFloatBits((int*)&f);
    ToggleBit(23, (int*)&f);
    ToggleBit(24, (int*)&f);
    ToggleBit(25, (int*)&f);
    ToggleBit(26, (int*)&f);
    ToggleBit(27, (int*)&f);
    ToggleBit(28, (int*)&f);
    ToggleBit(29, (int*)&f);
    PrintFloatBits((int*)&f);
    // ToggleBit(21, (int*)&f);
    // PrintFloatBits((int*)&f);

    return 0;
}