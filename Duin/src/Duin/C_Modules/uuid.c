#include "uuid.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

UUID uuid_create_random()
{
    UUID uuid;

    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    for (int i = 0; i < 16; i++) {
        uuid.bytes[i] = (uint8_t)rand();
    }

    uuid.bytes[6] = (uuid.bytes[6] & 0x0F) | 0x40; /* Version 4 (0100xxxx) */
    uuid.bytes[8] = (uuid.bytes[8] & 0x3F) | 0x80; /* Variant 1 (10xxxxxx) */

    return uuid;
}

