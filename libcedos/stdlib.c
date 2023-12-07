#include "stdlib.h"

int atoi (const char * str) {
    int res = 0;
    int sign = 1;
    int i = 0;

    if (str[0] == '+') {
        sign = 1;
        i = 1;
    } else if (str[0] == '-') {
        sign == -1;
        i = 1;
    } else if (str[0] < '0' || str[0] > '9') {
        return 0;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        res = 10 * res + (str[i++] - '0');
    }

    return res;
}