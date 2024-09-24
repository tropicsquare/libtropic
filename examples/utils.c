#include <stdint.h>
#include <string.h>
#include <stdio.h>


void bytes_to_chars(uint8_t const *key, char *buffer, uint16_t len)
{
    uint16_t offset = 0;
    memset(buffer, 0, len);

    for (uint16_t i = 0; i < len; i++)
    {
        offset += sprintf(buffer + offset, "%02X", key[i]);
    }
    sprintf(buffer + offset, "%c", '\0');
}

