#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define FLOAT_MAX_MANTISSA 16777215LL
#define DOUBLE_MAX_MANTISSA 4503599627370495LL

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

void FloatView_ToggleBit(FloatView* f, uint8_t bit_num) {
    assert(bit_num < 32);

    f->i32 = f->i32 ^ (1 << bit_num);    
}

void DoubleView_ToggleBit(DoubleView* d, uint8_t bit_num) {
    assert(bit_num < 64);

    d->i64 = d->i64 ^ ((uint64_t)1 << bit_num);    
}

void DoubleView_PrintBits(DoubleView d, uint16_t decimal_places) {
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

    int16_t exponent_value_biased = d.exponent ? d.exponent - 1023 : -1022;

    printf("   2^%d     ", exponent_value_biased);
    double two_to_the_negative_52 = 0.00000000000000022204460492503130808472633361816;
    double decimal_part = (double)d.mantissa * two_to_the_negative_52;
    if(d.exponent != 0) {
        decimal_part += 1.0f;
    }
    printf("%.52f", decimal_part);

    printf("\n");
}

void FloatView_PrintBits(FloatView f, uint16_t decimal_places) {
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

    int8_t exponent_value_biased = f.exponent ? f.exponent - 127 : -126;

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

int ScinoteString_Increment(char* num, int index) {
    if(num[index] == '.') {
        return ScinoteString_Increment(num, index - 1);
    }
    else if(num[index] == '9') {
        if(index == 0) {
            return 0;
        }
        else {
            int ret = ScinoteString_Increment(num, index - 1);
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

char* ScinoteString_Create(int num_digits, int exponent) {
    assert(num_digits > 0);

    // Figure out how many digits are in the exponent
    int temp_exponent = exponent;
    int num_exponent_digits = 0;
    do {
        ++num_exponent_digits;
        temp_exponent /= 10;
    } while(temp_exponent);
    
    // Allocate 3 extra chars for ., e, null term, and maybe one for negative
    int exponent_is_negative = exponent < 0;
    int num_chars = num_digits + num_exponent_digits + 3 + exponent_is_negative;
    char* s = calloc(num_chars, sizeof(char));
    if(!s) {
        return NULL;
    }

    // Fill entire string with '0' char to start
    memset(s, '0', num_chars - 1);
    s[0] = '1';
    s[1] = '.';

    // Subtract 2 to move past null terminator
    num_chars -= 2;
    // Negative exponent needs to be changed to positive
    // so the ascii shift in the loop works properly
    if(exponent_is_negative) {
        exponent *= -1;
    }
    do {
        s[num_chars--] = (exponent % 10) + '0';
        exponent /= 10;
    } while(exponent);
    if(exponent_is_negative) {
        s[num_chars--] = '-';
    }
    s[num_chars] = 'e';

    return s;
}

int ScinoteString_GetIncrementIndex(char* num) {
    assert(num);

    int i = 0;
    while(num[i] != 'e') {
        ++i;
    }
    return i-1;
}

void TestPrecision(int num_digits, int exponent) {
    char* scinote_string = ScinoteString_Create(num_digits, exponent);
    if(!scinote_string) {
        printf("Couldn't calloc for scinote_string\n");
        return;
    }
    int scinote_string_len = strlen(scinote_string);
    char* prev_scinote_string = calloc(scinote_string_len + 1, sizeof(char));
    if(!prev_scinote_string) {
        printf("Couldn't calloc for prev_scinote_string\n");
        return;
    }
    memcpy(prev_scinote_string, scinote_string, scinote_string_len);

    FloatView prev_f = {0};
    prev_f.f32 = strtof(scinote_string, NULL);
    FloatView f = {0};

    int increment_index = ScinoteString_GetIncrementIndex(scinote_string);
    printf("Testing %d digits of precision from %s...\n\n", num_digits, scinote_string);
    while(ScinoteString_Increment(scinote_string, increment_index)) {
        f.f32 = strtof(scinote_string, NULL);
        if(f.f32 == prev_f.f32) {
            printf("Both %s and %s map to:\n", prev_scinote_string, scinote_string);
            FloatView_PrintBits(f, 5);
            break;
        }

        memcpy(prev_scinote_string, scinote_string, scinote_string_len);
        prev_f.f32 = f.f32;
    }

    printf("\ndone...\n\n");

    free(scinote_string);
    free(prev_scinote_string);
}

int main() {

    // TestPrecision(7, 10);
    // for(int i = 0; i < 5; ++i) {
    //     TestPrecision(7, i);
    // }

    // Look at upper part of k ranges to 
    // see the largest diff between floats
    // char* s = "9.999900e10";
    // FloatView f = {0};
    // f.f32 = strtof(s, NULL);
    // printf("%s\n", s);
    // PrintFloatBits(f, 5);
    // while(IncrementStringNum(s, GetIncrementIndex(s))) {
    //     printf("%s\n", s);
    //     f.f32 = strtof(s, NULL);
    //     PrintFloatBits(f, 5);
    // }

    return 0;
}