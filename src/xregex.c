#include "xregex.h"
#define FIRST_BIT (0x80)       //1000 0000b
#define FIRST_TWO_BITS (0xc0)  //1100 0000b
#define LAST_SIX_BITS (0x3f)   //0011 1111b
int utf8ToInt(unsigned char *c, int *result);
int intToUtf8(int number, char *result);

int utf8ToInt(unsigned char *c, int *result) {
    int byte_num = 0;
    int tmp = *c;
    *result = 0;
    while (0x80 == (tmp & FIRST_BIT)) {
        byte_num++;
        tmp <<= 1;
    }
    switch (byte_num) {
        case 0:
            *result = *c;
            return 1;
        case 1:
            return 0;
        case 2:
            *result += (int)(*c - 0xc0);
            break;
        case 3:
            *result += (int)(*c - 0xe0);
            break;
        case 4:
            *result += (int)(*c - 0xf0);
            break;
        default:
            return 0;
    }

    for (int i = byte_num - 1; i > 0; i--) {
        c++;
        *result *= 0x40;
        if (0xc0 != (*c) & FIRST_TWO_BITS) {
            return 0;
        } else {
            *result += (*c - 0x80);
        }
    }
    return byte_num;
}

int intToUtf8(int number, char *result) {
    if (number < 0) {
        return 0;
    } else if (number < 0x80) {
        *result = number;
        return 1;
    } else if (number < 0x800) {
        *result = 0xc0 + (number >> 6);
        result++;
        *result = FIRST_BIT + (number & LAST_SIX_BITS);
        return 2;
    } else if (number < 0x10000) {
        *result = 0xe0 + (number >> 12);
        result++;
        *result = FIRST_BIT + ((number >> 6) & LAST_SIX_BITS);
        result++;
        *result = FIRST_BIT + (number & LAST_SIX_BITS);
        return 3;
    } else if (number <= 0xf48fbfbf) {
        *result = 0xf0 + (number >> 18);
        result++;
        *result = FIRST_BIT + ((number >> 12) & LAST_SIX_BITS);
        result++;
        *result = FIRST_BIT + ((number >> 6) & LAST_SIX_BITS);
        result++;
        *result = FIRST_BIT + (number & LAST_SIX_BITS);
        return 4;
    } else {
        return 0;
    }
}