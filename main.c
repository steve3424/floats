#include <stdio.h>
#include <stdint.h>

// void SetBit(uint8_t on_or_off, uint8_t bit_num, float* value) {

// }

void PrintFloatBits(int* value) {
    printf(*value & (uint32_t)1 << 31 ? "[1]" : "[0]");

    printf(" [");
    for(int8_t i = 30; i > 22; --i) {
        printf(*value & (uint32_t)1 << i ? "1" : "0");
    }
    printf("] ");

    printf("[");
    for(int8_t i = 22; i > -1; --i) {
        printf(*value & (uint32_t)1 << i ? "1" : "0");
    }
    printf("]");
    printf("\n");

    printf("%0.5f\n", *(float*)value);
}

int main() {

    float f = 0.15625f;
    PrintFloatBits((int*)&f);

    return 0;
}