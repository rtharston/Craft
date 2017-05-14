#include "set.h"

void unset_sign(int x, int y, int z, Model *g) {
    int p = chunked(x);
    int q = chunked(z);
    Chunk *chunk = find_chunk(p, q, g);
    if (chunk) {
        SignList *signs = &chunk->signs;
        if (sign_list_remove_all(signs, x, y, z)) {
            chunk->dirty = 1;
            db_delete_signs(x, y, z);
        }
    }
    else {
        db_delete_signs(x, y, z);
    }
}

void unset_sign_face(int x, int y, int z, int face, Model *g) {
    int p = chunked(x);
    int q = chunked(z);
    Chunk *chunk = find_chunk(p, q, g);
    if (chunk) {
        SignList *signs = &chunk->signs;
        if (sign_list_remove(signs, x, y, z, face)) {
            chunk->dirty = 1;
            db_delete_sign(x, y, z, face);
        }
    }
    else {
        db_delete_sign(x, y, z, face);
    }
}

void _set_sign(
               int p, int q, int x, int y, int z, int face, const char *text, int dirty, Model *g)
{
    if (strlen(text) == 0) {
        unset_sign_face(x, y, z, face, g);
        return;
    }
    Chunk *chunk = find_chunk(p, q, g);
    if (chunk) {
        SignList *signs = &chunk->signs;
        sign_list_add(signs, x, y, z, face, text);
        if (dirty) {
            chunk->dirty = 1;
        }
    }
    db_insert_sign(p, q, x, y, z, face, text);
}

void set_sign(int x, int y, int z, int face, const char *text, Model *g) {
    int p = chunked(x);
    int q = chunked(z);
    _set_sign(p, q, x, y, z, face, text, 1, g);
    client_sign(x, y, z, face, text);
}

void set_light(int p, int q, int x, int y, int z, int w, Model *g) {
    Chunk *chunk = find_chunk(p, q, g);
    if (chunk) {
        Map *map = &chunk->lights;
        if (map_set(map, x, y, z, w)) {
            dirty_chunk(chunk, g);
            db_insert_light(p, q, x, y, z, w);
        }
    }
    else {
        db_insert_light(p, q, x, y, z, w);
    }
}
