#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define MAX_MANTISSA 16777215LL

typedef struct {
    union {
        struct {
            unsigned int mantissa : 23;
            unsigned int exponent :  8;
            unsigned int sign     :  1;
        };
        uint32_t i32;
        float f32;
    };
} FloatView;

typedef struct {
    union {
        struct {
            uint64_t mantissa : 52;
            uint64_t exponent : 11;
            uint64_t sign     :  1;
        };
        uint64_t i64;
        double f64;
    };
} DoubleView;

void ToggleBitf(uint8_t bit_num, FloatView* f) {
    assert(bit_num < 32);
    f->i32 = f->i32 ^ (1 << bit_num);    
}

void ToggleBitd(uint64_t bit_num, DoubleView* d) {
    assert(bit_num < 64);
    d->i64 = d->i64 ^ ((uint64_t)1 << bit_num);    
}

void PrintDoubleBits(DoubleView d, uint16_t decimal_places) {
    // Print actual float number
    printf("%0.*f\n", decimal_places, d.f64);

    // Print bits
    printf(d.sign ? "[1] " : "[0] ");

    printf("[");
    for(int8_t i = 62; i > 51; --i) {
        printf(d.i64 & ((uint64_t)1 << i) ? "1" : "0");
    }
    printf("] ");

    printf("[");
    for(int8_t i = 51; i > -1; --i) {
        printf(d.i64 & ((uint64_t)1 << i) ? "1" : "0");
    }
    printf("]\n");

    // Print text
    printf(d.sign ? " -  " : " +  ");

    int16_t exponent_value_biased = d.exponent - 1023;
    if(d.exponent == 0) {
        exponent_value_biased = -1022;
    }

    // Print different number of spaces based on number of decimal digits in biased exponent
    printf("   2^%d     ", exponent_value_biased);
    // if(exponent_value_biased < 10) {
    //     printf("   2^%d     ", exponent_value_biased);
    // }
    // else if(exponent_value_biased < 100) {
    //     printf("   2^%d    ", exponent_value_biased);
    // }
    // else {
    //     printf("   2^%d   ", exponent_value_biased);
    // }
    double two_to_the_negative_52 = 0.00000000000000022204460492503130808472633361816;
    double decimal_part = (double)d.mantissa * two_to_the_negative_52;
    if(d.exponent != 0) {
        decimal_part += 1.0f;
    }
    printf("%.52f", decimal_part);

    printf("\n");
}

void PrintFloatBits(FloatView f, uint16_t decimal_places) {
    // Print actual float number
    printf("%0.*f\n", decimal_places, f.f32);

    printf(f.sign ? "[1] " : "[0] ");

    printf("[");
    for(uint8_t i = 30; i > 22; --i) {
        printf(f.i32 & (1 << i) ? "1" : "0");
    }
    printf("] ");

    printf("[");
    for(int8_t i = 22; i > -1; --i) {
        printf(f.i32 & (1 << i) ? "1" : "0");
    }
    printf("]\n");

    // // Print text
    printf(f.sign ? " -  " : " +  ");

    int8_t exponent_value_biased = f.exponent - 127;
    if(f.exponent == 0) {
        exponent_value_biased = -126;
    }

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

    float two_to_the_negative_23 = 0.00000011920928955078125f;
    float decimal_part = (float)f.mantissa * two_to_the_negative_23;
    if(f.exponent != 0) {
        decimal_part += 1.0f;
    }
    printf("%.25f", decimal_part);

    printf("\n");
}

int IncrementStringNum(char* num, int index) {
    if(num[index] == '.') {
        return IncrementStringNum(num, index - 1);
    }
    else if(num[index] == '9') {
        if(index == 0) {
            return 0;
        }
        else {
            int ret = IncrementStringNum(num, index - 1);
            if(ret) {
                num[index] = '0';
            }
            return ret;
        }
    }
    else {
        num[index]++;
        return 1;
    }
}

char* CreateExpString(int num_digits, int exp) {
    assert(num_digits > 1);

    int temp_exp = exp;
    int num_exp_digits = 0;
    while(temp_exp) {
        ++num_exp_digits;
        temp_exp /= 10;
    }
    
    int num_chars = num_digits + num_exp_digits + 3;
    char* s = malloc(sizeof(char) * num_chars);
    if(!s) {
        return NULL;
    }

    s[--num_chars] = '\0';
    num_chars--;
    while(exp) {
        s[num_chars--] = (exp % 10) + '0';
        exp /= 10;
    }
    s[num_chars] = 'e';

    s[0] = '1';
    s[1] = '.';
    int i = 2;
    while(s[i] != 'e') {
        s[i] = '0';
        ++i;
    }

    return s;
}

int GetIncrementIndex(char* num) {
    assert(num);

    int i = 0;
    while(num[i] != 'e') {
        ++i;
    }
    return i-1;
}

int main() {
    int num_digits = 8;
    int exp = 9;
    char* num_string = CreateExpString(num_digits, exp);
    int num_string_len = strlen(num_string);

    FloatView f = {0};
    f.f32 = strtof(num_string, NULL);
    FloatView f1 = {0};
    int increment_index = GetIncrementIndex(num_string);
    while(IncrementStringNum(num_string, increment_index)) {
        f1.f32 = strtof(num_string, NULL);
        int8_t same = (f.f32 == f1.f32);
        if(same) {
            printf("%s is same as previous\n", num_string);
            PrintFloatBits(f, 5);
            break;
        }
        f.f32 = f1.f32;
    }
    printf("done...\n");

    return 0;
}