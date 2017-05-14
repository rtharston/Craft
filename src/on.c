#include "on.h"

void on_light(Model *g) {
    State *s = &g->players->state;
    int hx, hy, hz;
    int hw = hit_test(0, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz, g);
    if (hy > 0 && hy < 256 && is_destructable(hw)) {
        toggle_light(hx, hy, hz, g);
    }
}

void on_left_click(Model *g) {
    State *s = &g->players->state;
    int hx, hy, hz;
    int hw = hit_test(0, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz, g);
    if (hy > 0 && hy < 256 && is_destructable(hw)) {
        set_block(hx, hy, hz, 0, g);
        record_block(hx, hy, hz, 0, g);
        if (is_plant(get_block(hx, hy + 1, hz, g))) {
            set_block(hx, hy + 1, hz, 0, g);
        }
    }
}

void on_right_click(Model *g) {
    State *s = &g->players->state;
    int hx, hy, hz;
    int hw = hit_test(1, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz, g);
    if (hy > 0 && hy < 256 && is_obstacle(hw)) {
        if (!player_intersects_block(2, s->x, s->y, s->z, hx, hy, hz)) {
            set_block(hx, hy, hz, items[g->item_index], g);
            record_block(hx, hy, hz, items[g->item_index], g);
        }
    }
}

void on_middle_click(Model *g) {
    State *s = &g->players->state;
    int hx, hy, hz;
    int hw = hit_test(0, s->x, s->y, s->z, s->rx, s->ry, &hx, &hy, &hz, g);
    for (int i = 0; i < item_count; i++) {
        if (items[i] == hw) {
            g->item_index = i;
            break;
        }
    }
}

void toggle_light(int x, int y, int z, Model *g) {
    int p = chunked(x);
    int q = chunked(z);
    Chunk *chunk = find_chunk(p, q, g);
    if (chunk) {
        Map *map = &chunk->lights;
        int w = map_get(map, x, y, z) ? 0 : 15;
        map_set(map, x, y, z, w);
        db_insert_light(p, q, x, y, z, w);
        client_light(x, y, z, w);
        dirty_chunk(chunk, g);
    }
}
