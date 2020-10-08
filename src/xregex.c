#include "xregex.h"
#define FIRST_BIT (0x80)
#define FIRST_TWO_BITS (0xc0)
int utf8ToInt(unsigned char *c, int *result);


int utf8ToInt(unsigned char *c, int *result) {
    int byte_num = 0;
    int tmp = *c;
    *result = 0;
    while (0x80 == (tmp & FIRST_BIT)) {
        byte_num++;
        tmp <<= 1;
    }
    switch (byte_num)
    {
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

    for (int i = byte_num - 1; i > 0; i--){
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