#include <stdlib.h>
#include <string.h>
#include "utils.h"

// a key-value implementation

#define FNV32_BASIS ((unsigned) 0x811c9dc5)
#define FNV32_PRIME ((unsigned) 0x01000193)

#define MAP_INIT_SIZE       64
#define MAP_GROW_FACTOR     80
#define MAP_RESIZE_BITS     2

struct map_entry {
    const char *key;
    void *value;
    struct map_entry *next;
};

struct map {
    unsigned size, tablesize;
    unsigned grow_at, shrink_at;
    struct map_entry **table;
};

// FNV-1a
unsigned strhash(const char *s)
{
    unsigned hash = FNV32_BASIS;
    for (; *s; s++) {
        hash ^= *s;
        hash *= FNV32_PRIME;
    }
    return hash;
}

static struct map * alloc_map(struct map *map, unsigned size)
{
    map->table = zmalloc(size * sizeof(struct map_entry *));
    map->tablesize = size;
    map->grow_at = (unsigned) (size * MAP_GROW_FACTOR / 100);
    map->shrink_at = map->grow_at / ((1<<MAP_RESIZE_BITS) + 1);
    return map;
}

static unsigned bucket(struct map *map, const char *key)
{
    return strhash(key) & (map->tablesize - 1);
}

static void rehash(struct map *map, unsigned newsize)
{
    unsigned oldsize = map->tablesize;
    struct map_entry **oldtable = map->table;
    
    alloc_map(map, newsize);
    for (int i = 0; i < oldsize; i++) {
        struct map_entry *entry = oldtable[i];
        while (entry) {
            struct map_entry *next = entry->next;
            unsigned b = bucket(map, entry->key);
            entry->next = map->table[b];
            map->table[b] = entry;
            entry = next;
        }
    }
    free(oldtable);
}

static int eqentry(struct map_entry *entry, const char *key)
{
    return entry->key == key || !strcmp(entry->key, key);
}

static struct map_entry ** find_entry(struct map *map, const char *key)
{
    struct map_entry **entry = &map->table[bucket(map, key)];
    while (*entry && !eqentry(*entry, key))
        entry = &(*entry)->next;
    return entry;
}

static void map_remove(struct map *map, const char *key)
{
    struct map_entry **entry = find_entry(map, key);
    if (!*entry)
        return;
    
    struct map_entry *old = *entry;
    *entry = old->next;
    free(old);

    map->size--;
    if (map->size < map->shrink_at)
        rehash(map, map->tablesize >> MAP_RESIZE_BITS);
}

static void map_add(struct map *map, const char *key, void *value)
{
    unsigned b = bucket(map, key);
    struct map_entry *entry = zmalloc(sizeof(struct map_entry));
    entry->key = key;
    entry->value = value;
    entry->next = map->table[b];
    map->table[b] = entry;
    map->size++;
    if (map->size > map->grow_at)
        rehash(map, map->tablesize << MAP_RESIZE_BITS);
}

struct map * new_map()
{
    unsigned size = MAP_INIT_SIZE;
    struct map *map = zmalloc(sizeof(struct map));
    map->size = 0;
    return alloc_map(map, size);
}

void free_map(struct map *map)
{
    if (!map)
        return;
    for (int i = 0; i < map->tablesize; i++) {
        struct map_entry *entry = map->table[i];
        while (entry) {
            struct map_entry *next = entry->next;
            free(entry);
            entry = next;
        }
    }
    free(map->table);
    free(map);
}

void *map_get(struct map *map, const char *key)
{
    struct map_entry *entry = *find_entry(map, key);
    if (entry)
        return entry->value;
    else
        return NULL;
}

void map_put(struct map *map, const char *key, void *value)
{
    map_remove(map, key);
    if (value)
        map_add(map, key, value);
}
