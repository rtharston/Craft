#include "others.h"

float time_of_day(Model *g) {
    if (g->day_length <= 0) {
        return 0.5;
    }
    float t;
    t = glfwGetTime();
    t = t / g->day_length;
    t = t - (int)t;
    return t;
}

float get_daylight(Model *g) {
    float timer = time_of_day(g);
    if (timer < 0.5) {
        float t = (timer - 0.25) * 100;
        return 1 / (1 + powf(2, -t));
    }
    else {
        float t = (timer - 0.85) * 100;
        return 1 - 1 / (1 + powf(2, -t));
    }
}

void check_workers(Model *g) {
    for (int i = 0; i < WORKERS; i++) {
        Worker *worker = g->workers + i;
        mtx_lock(&worker->mtx);
        if (worker->state == WORKER_DONE) {
            WorkerItem *item = &worker->item;
            Chunk *chunk = find_chunk(item->p, item->q, g);
            if (chunk) {
                if (item->load) {
                    Map *block_map = item->block_maps[1][1];
                    Map *light_map = item->light_maps[1][1];
                    map_free(&chunk->map);
                    map_free(&chunk->lights);
                    map_copy(&chunk->map, block_map);
                    map_copy(&chunk->lights, light_map);
                    request_chunk(item->p, item->q);
                }
                generate_chunk(chunk, item);
            }
            for (int a = 0; a < 3; a++) {
                for (int b = 0; b < 3; b++) {
                    Map *block_map = item->block_maps[a][b];
                    Map *light_map = item->light_maps[a][b];
                    if (block_map) {
                        map_free(block_map);
                        free(block_map);
                    }
                    if (light_map) {
                        map_free(light_map);
                        free(light_map);
                    }
                }
            }
            worker->state = WORKER_IDLE;
        }
        mtx_unlock(&worker->mtx);
    }
}

void ensure_chunks_worker(Player *player, Worker *worker, Model *g) {
    State *s = &player->state;
    float matrix[16];
    set_matrix_3d(
                  matrix, g->width, g->height,
                  s->x, s->y, s->z, s->rx, s->ry, g->fov, g->ortho, g->render_radius);
    float planes[6][4];
    frustum_planes(planes, g->render_radius, matrix);
    int p = chunked(s->x);
    int q = chunked(s->z);
    int r = g->create_radius;
    int start = 0x0fffffff;
    int best_score = start;
    int best_a = 0;
    int best_b = 0;
    for (int dp = -r; dp <= r; dp++) {
        for (int dq = -r; dq <= r; dq++) {
            int a = p + dp;
            int b = q + dq;
            int index = (ABS(a) ^ ABS(b)) % WORKERS;
            if (index != worker->index) {
                continue;
            }
            Chunk *chunk = find_chunk(a, b, g);;
            if (chunk && !chunk->dirty) {
                continue;
            }
            int distance = MAX(ABS(dp), ABS(dq));
            int invisible = !chunk_visible(planes, a, b, 0, 256, g);
            int priority = 0;
            if (chunk) {
                priority = chunk->buffer && chunk->dirty;
            }
            int score = (invisible << 24) | (priority << 16) | distance;
            if (score < best_score) {
                best_score = score;
                best_a = a;
                best_b = b;
            }
        }
    }
    if (best_score == start) {
        return;
    }
    int a = best_a;
    int b = best_b;
    int load = 0;
    Chunk *chunk = find_chunk(a, b, g);;
    if (!chunk) {
        load = 1;
        if (g->chunk_count < MAX_CHUNKS) {
            chunk = g->chunks + g->chunk_count++;
            init_chunk(chunk, a, b, g);
        }
        else {
            return;
        }
    }
    WorkerItem *item = &worker->item;
    item->p = chunk->p;
    item->q = chunk->q;
    item->load = load;
    for (int dp = -1; dp <= 1; dp++) {
        for (int dq = -1; dq <= 1; dq++) {
            Chunk *other = chunk;
            if (dp || dq) {
                other = find_chunk(chunk->p + dp, chunk->q + dq, g);
            }
            if (other) {
                Map *block_map = malloc(sizeof(Map));
                map_copy(block_map, &other->map);
                Map *light_map = malloc(sizeof(Map));
                map_copy(light_map, &other->lights);
                item->block_maps[dp + 1][dq + 1] = block_map;
                item->light_maps[dp + 1][dq + 1] = light_map;
            }
            else {
                item->block_maps[dp + 1][dq + 1] = 0;
                item->light_maps[dp + 1][dq + 1] = 0;
            }
        }
    }
    chunk->dirty = 0;
    worker->state = WORKER_BUSY;
    cnd_signal(&worker->cnd);
}

void ensure_chunks(Player *player, Model *g) {
    check_workers(g);
    force_chunks(player, g);
    for (int i = 0; i < WORKERS; i++) {
        Worker *worker = g->workers + i;
        mtx_lock(&worker->mtx);
        if (worker->state == WORKER_IDLE) {
            ensure_chunks_worker(player, worker, g);
        }
        mtx_unlock(&worker->mtx);
    }
}

int worker_run(void *arg) {
    Worker *worker = (Worker *)arg;
    int running = 1;
    while (running) {
        mtx_lock(&worker->mtx);
        while (worker->state != WORKER_BUSY) {
            cnd_wait(&worker->cnd, &worker->mtx);
        }
        mtx_unlock(&worker->mtx);
        WorkerItem *item = &worker->item;
        if (item->load) {
            load_chunk(item);
        }
        compute_chunk(item);
        mtx_lock(&worker->mtx);
        worker->state = WORKER_DONE;
        mtx_unlock(&worker->mtx);
    }
    return 0;
}

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

void handle_mouse_input(Model *g) {
    int exclusive =
    glfwGetInputMode(g->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    static double px = 0;
    static double py = 0;
    State *s = &g->players->state;
    if (exclusive && (px || py)) {
        double mx, my;
        glfwGetCursorPos(g->window, &mx, &my);
        float m = 0.0025;
        s->rx += (mx - px) * m;
        if (INVERT_MOUSE) {
            s->ry += (my - py) * m;
        }
        else {
            s->ry -= (my - py) * m;
        }
        if (s->rx < 0) {
            s->rx += RADIANS(360);
        }
        if (s->rx >= RADIANS(360)){
            s->rx -= RADIANS(360);
        }
        s->ry = MAX(s->ry, -RADIANS(90));
        s->ry = MIN(s->ry, RADIANS(90));
        px = mx;
        py = my;
    }
    else {
        glfwGetCursorPos(g->window, &px, &py);
    }
}

void handle_movement(double dt, Model *g) {
    static float dy = 0;
    State *s = &g->players->state;
    int sz = 0;
    int sx = 0;
    if (!g->typing) {
        float m = dt * 1.0;
        g->ortho = glfwGetKey(g->window, CRAFT_KEY_ORTHO) ? 64 : 0;
        g->fov = glfwGetKey(g->window, CRAFT_KEY_ZOOM) ? 15 : 65;
        if (glfwGetKey(g->window, CRAFT_KEY_FORWARD)) sz--;
        if (glfwGetKey(g->window, CRAFT_KEY_BACKWARD)) sz++;
        if (glfwGetKey(g->window, CRAFT_KEY_LEFT)) sx--;
        if (glfwGetKey(g->window, CRAFT_KEY_RIGHT)) sx++;
        if (glfwGetKey(g->window, GLFW_KEY_LEFT)) s->rx -= m;
        if (glfwGetKey(g->window, GLFW_KEY_RIGHT)) s->rx += m;
        if (glfwGetKey(g->window, GLFW_KEY_UP)) s->ry += m;
        if (glfwGetKey(g->window, GLFW_KEY_DOWN)) s->ry -= m;
    }
    float vx, vy, vz;
    get_motion_vector(g->flying, sz, sx, s->rx, s->ry, &vx, &vy, &vz);
    if (!g->typing) {
        if (glfwGetKey(g->window, CRAFT_KEY_JUMP)) {
            if (g->flying) {
                vy = 1;
            }
            else if (dy == 0) {
                dy = 8;
            }
        }
    }
    float speed = g->flying ? 20 : 5;
    int estimate = roundf(sqrtf(
                                powf(vx * speed, 2) +
                                powf(vy * speed + ABS(dy) * 2, 2) +
                                powf(vz * speed, 2)) * dt * 8);
    int step = MAX(8, estimate);
    float ut = dt / step;
    vx = vx * ut * speed;
    vy = vy * ut * speed;
    vz = vz * ut * speed;
    for (int i = 0; i < step; i++) {
        if (g->flying) {
            dy = 0;
        }
        else {
            dy -= ut * 25;
            dy = MAX(dy, -250);
        }
        s->x += vx;
        s->y += vy + dy * ut;
        s->z += vz;
        if (collide(2, &s->x, &s->y, &s->z, g)) {
            dy = 0;
        }
    }
    if (s->y < 0) {
        s->y = highest_block(s->x, s->z, g) + 2;
    }
}
