#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

char* serial_read_string(void)
{
    char c[256];
    uint8_t i = 0;

    while (1)
    {
        c[i] = getchar();

        if (c[i] == '\r')
        {
            c[i] = '\0';
            printf("\n");
            break;
        }
        printf("%c", c[i]);
        i++;
    }
    size_t len = strlen(c);
    char *ret = (char *)malloc(len);
    strcpy(ret, c);

    return ret;
}
