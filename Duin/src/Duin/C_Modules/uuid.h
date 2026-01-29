#ifndef C_UUID_H
#define C_UUID_H

#include <stdint.h>
#include <string.h>

#define IS_UUID_VALID(_uuid) (memcmp(_uuid.bytes, UUID_NULL.bytes, 16) != 0)

#define UUID_EQUALS(_uuidA, _uuidB) (memcmp(_uuidA.bytes, _uuidB.bytes, 16) == 0)

/* 128 bit UUID */
typedef struct UUID
{
    uint8_t bytes[16];
} UUID;

const UUID UUID_NULL = {{0}};

#if defined(__cplusplus)
extern "C"
{ // Prevents name mangling of functions
#endif

    UUID uuid_create_random();

    /* Inline function to check if a UUID is initialized */
    inline int is_uuid_valid(const UUID uuid)
    {
        return memcmp(uuid.bytes, UUID_NULL.bytes, 16) != 0;
    }

    /* Inline function to compare UUIDs */
    inline int uuid_equals(const UUID a, const UUID b)
    {
        return memcmp(a.bytes, b.bytes, 16) == 0;
    }

    /* Inline function to generate a hash based on container size */
    inline size_t uuid_hash(const UUID uuid, size_t container_size)
    {
        size_t hash = 0;
        for (int i = 0; i < 16; ++i)
        {
            hash = hash * 31 + uuid.bytes[i];
        }

        return hash % container_size;
    }

#if defined(__cplusplus)
}
#endif

#endif /* C_UUID_H */
