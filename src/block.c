#include "block.h"

// Goes with left/right click
void _set_block(int p, int q, int x, int y, int z, int w, int dirty, Model *g) {
    Chunk *chunk = find_chunk(p, q, g);
    if (chunk) {
        Map *map = &chunk->map;
        if (map_set(map, x, y, z, w)) {
            if (dirty) {
                dirty_chunk(chunk, g);
            }
            db_insert_block(p, q, x, y, z, w);
        }
    }
    else {
        db_insert_block(p, q, x, y, z, w);
    }
    if (w == 0 && chunked(x) == p && chunked(z) == q) {
        unset_sign(x, y, z, g);
        set_light(p, q, x, y, z, 0, g);
    }
}

// Goes with left/right click
void set_block(int x, int y, int z, int w, Model *g) {
    int p = chunked(x);
    int q = chunked(z);
    _set_block(p, q, x, y, z, w, 1, g);
    for (int dx = -1; dx <= 1; dx++) {
        for (int dz = -1; dz <= 1; dz++) {
            if (dx == 0 && dz == 0) {
                continue;
            }
            if (dx && chunked(x + dx) == p) {
                continue;
            }
            if (dz && chunked(z + dz) == q) {
                continue;
            }
            _set_block(p + dx, q + dz, x, y, z, -w, 1, g);
        }
    }
    client_block(x, y, z, w);
}

// Goes with the left/right click
void record_block(int x, int y, int z, int w, Model *g) {
    memcpy(&g->block1, &g->block0, sizeof(Block));
    g->block0.x = x;
    g->block0.y = y;
    g->block0.z = z;
    g->block0.w = w;
}

int get_block(int x, int y, int z, Model *g) {
    int p = chunked(x);
    int q = chunked(z);
    Chunk *chunk = find_chunk(p, q, g);
    if (chunk) {
        Map *map = &chunk->map;
        return map_get(map, x, y, z);
    }
    return 0;
}
