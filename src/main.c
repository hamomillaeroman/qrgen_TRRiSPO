#include <stdio.h>

#ifndef APP_VERSION
#define APP_VERSION "0.0.0"
#endif

int main(void)
{
    printf("qrgen %s\n", APP_VERSION);
    return 0;
}
