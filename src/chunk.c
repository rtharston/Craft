//#include "chunk.h"

//int chunked(float x) {
//    return floorf(roundf(x) / CHUNK_SIZE);
//}
//
//Chunk *find_chunk(int p, int q) {
//    for (int i = 0; i < g->chunk_count; i++) {
//        Chunk *chunk = g->chunks + i;
//        if (chunk->p == p && chunk->q == q) {
//            return chunk;
//        }
//    }
//    return 0;
//}
//
//int chunk_distance(Chunk *chunk, int p, int q) {
//    int dp = ABS(chunk->p - p);
//    int dq = ABS(chunk->q - q);
//    return MAX(dp, dq);
//}
//
//int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy) {
//    int x = p * CHUNK_SIZE - 1;
//    int z = q * CHUNK_SIZE - 1;
//    int d = CHUNK_SIZE + 1;
//    float points[8][3] = {
//        {x + 0, miny, z + 0},
//        {x + d, miny, z + 0},
//        {x + 0, miny, z + d},
//        {x + d, miny, z + d},
//        {x + 0, maxy, z + 0},
//        {x + d, maxy, z + 0},
//        {x + 0, maxy, z + d},
//        {x + d, maxy, z + d}
//    };
//    int n = g->ortho ? 4 : 6;
//    for (int i = 0; i < n; i++) {
//        int in = 0;
//        int out = 0;
//        for (int j = 0; j < 8; j++) {
//            float d =
//            planes[i][0] * points[j][0] +
//            planes[i][1] * points[j][1] +
//            planes[i][2] * points[j][2] +
//            planes[i][3];
//            if (d < 0) {
//                out++;
//            }
//            else {
//                in++;
//            }
//            if (in && out) {
//                break;
//            }
//        }
//        if (in == 0) {
//            return 0;
//        }
//    }
//    return 1;
//}
//
//int highest_block(float x, float z) {
//    int result = -1;
//    int nx = roundf(x);
//    int nz = roundf(z);
//    int p = chunked(x);
//    int q = chunked(z);
//    Chunk *chunk = find_chunk(p, q);
//    if (chunk) {
//        Map *map = &chunk->map;
//        MAP_FOR_EACH(map, ex, ey, ez, ew) {
//            if (is_obstacle(ew) && ex == nx && ez == nz) {
//                result = MAX(result, ey);
//            }
//        } END_MAP_FOR_EACH;
//    }
//    return result;
//}
//
//void dirty_chunk(Chunk *chunk) {
//    chunk->dirty = 1;
//    if (has_lights(chunk)) {
//        for (int dp = -1; dp <= 1; dp++) {
//            for (int dq = -1; dq <= 1; dq++) {
//                Chunk *other = find_chunk(chunk->p + dp, chunk->q + dq);
//                if (other) {
//                    other->dirty = 1;
//                }
//            }
//        }
//    }
//}
//
//
//void create_chunk(Chunk *chunk, int p, int q) {
//    init_chunk(chunk, p, q);
//    
//    WorkerItem _item;
//    WorkerItem *item = &_item;
//    item->p = chunk->p;
//    item->q = chunk->q;
//    item->block_maps[1][1] = &chunk->map;
//    item->light_maps[1][1] = &chunk->lights;
//    load_chunk(item);
//    
//    request_chunk(p, q);
//}
//
//void delete_chunks() {
//    int count = g->chunk_count;
//    State *s1 = &g->players->state;
//    State *s2 = &(g->players + g->observe1)->state;
//    State *s3 = &(g->players + g->observe2)->state;
//    State *states[3] = {s1, s2, s3};
//    for (int i = 0; i < count; i++) {
//        Chunk *chunk = g->chunks + i;
//        int delete = 1;
//        for (int j = 0; j < 3; j++) {
//            State *s = states[j];
//            int p = chunked(s->x);
//            int q = chunked(s->z);
//            if (chunk_distance(chunk, p, q) < g->delete_radius) {
//                delete = 0;
//                break;
//            }
//        }
//        if (delete) {
//            map_free(&chunk->map);
//            map_free(&chunk->lights);
//            sign_list_free(&chunk->signs);
//            del_buffer(chunk->buffer);
//            del_buffer(chunk->sign_buffer);
//            Chunk *other = g->chunks + (--count);
//            memcpy(chunk, other, sizeof(Chunk));
//        }
//    }
//    g->chunk_count = count;
//}
//
//void delete_all_chunks() {
//    for (int i = 0; i < g->chunk_count; i++) {
//        Chunk *chunk = g->chunks + i;
//        map_free(&chunk->map);
//        map_free(&chunk->lights);
//        sign_list_free(&chunk->signs);
//        del_buffer(chunk->buffer);
//        del_buffer(chunk->sign_buffer);
//    }
//    g->chunk_count = 0;
//}
//

//void compute_chunk(WorkerItem *item) {
//    char *opaque = (char *)calloc(XZ_SIZE * XZ_SIZE * Y_SIZE, sizeof(char));
//    char *light = (char *)calloc(XZ_SIZE * XZ_SIZE * Y_SIZE, sizeof(char));
//    char *highest = (char *)calloc(XZ_SIZE * XZ_SIZE, sizeof(char));
//    
//    int ox = item->p * CHUNK_SIZE - CHUNK_SIZE - 1;
//    int oy = -1;
//    int oz = item->q * CHUNK_SIZE - CHUNK_SIZE - 1;
//    
//    // check for lights
//    int has_light = 0;
//    if (SHOW_LIGHTS) {
//        for (int a = 0; a < 3; a++) {
//            for (int b = 0; b < 3; b++) {
//                Map *map = item->light_maps[a][b];
//                if (map && map->size) {
//                    has_light = 1;
//                }
//            }
//        }
//    }
//    
//    // populate opaque array
//    for (int a = 0; a < 3; a++) {
//        for (int b = 0; b < 3; b++) {
//            Map *map = item->block_maps[a][b];
//            if (!map) {
//                continue;
//            }
//            MAP_FOR_EACH(map, ex, ey, ez, ew) {
//                int x = ex - ox;
//                int y = ey - oy;
//                int z = ez - oz;
//                int w = ew;
//                // TODO: this should be unnecessary
//                if (x < 0 || y < 0 || z < 0) {
//                    continue;
//                }
//                if (x >= XZ_SIZE || y >= Y_SIZE || z >= XZ_SIZE) {
//                    continue;
//                }
//                // END TODO
//                opaque[XYZ(x, y, z)] = !is_transparent(w);
//                if (opaque[XYZ(x, y, z)]) {
//                    highest[XZ(x, z)] = MAX(highest[XZ(x, z)], y);
//                }
//            } END_MAP_FOR_EACH;
//        }
//    }
//    
//    // flood fill light intensities
//    if (has_light) {
//        for (int a = 0; a < 3; a++) {
//            for (int b = 0; b < 3; b++) {
//                Map *map = item->light_maps[a][b];
//                if (!map) {
//                    continue;
//                }
//                MAP_FOR_EACH(map, ex, ey, ez, ew) {
//                    int x = ex - ox;
//                    int y = ey - oy;
//                    int z = ez - oz;
//                    light_fill(opaque, light, x, y, z, ew, 1);
//                } END_MAP_FOR_EACH;
//            }
//        }
//    }
//    
//    Map *map = item->block_maps[1][1];
//    
//    // count exposed faces
//    int miny = 256;
//    int maxy = 0;
//    int faces = 0;
//    MAP_FOR_EACH(map, ex, ey, ez, ew) {
//        if (ew <= 0) {
//            continue;
//        }
//        int x = ex - ox;
//        int y = ey - oy;
//        int z = ez - oz;
//        int f1 = !opaque[XYZ(x - 1, y, z)];
//        int f2 = !opaque[XYZ(x + 1, y, z)];
//        int f3 = !opaque[XYZ(x, y + 1, z)];
//        int f4 = !opaque[XYZ(x, y - 1, z)] && (ey > 0);
//        int f5 = !opaque[XYZ(x, y, z - 1)];
//        int f6 = !opaque[XYZ(x, y, z + 1)];
//        int total = f1 + f2 + f3 + f4 + f5 + f6;
//        if (total == 0) {
//            continue;
//        }
//        if (is_plant(ew)) {
//            total = 4;
//        }
//        miny = MIN(miny, ey);
//        maxy = MAX(maxy, ey);
//        faces += total;
//    } END_MAP_FOR_EACH;
//    
//    // generate geometry
//    GLfloat *data = malloc_faces(10, faces);
//    int offset = 0;
//    MAP_FOR_EACH(map, ex, ey, ez, ew) {
//        if (ew <= 0) {
//            continue;
//        }
//        int x = ex - ox;
//        int y = ey - oy;
//        int z = ez - oz;
//        int f1 = !opaque[XYZ(x - 1, y, z)];
//        int f2 = !opaque[XYZ(x + 1, y, z)];
//        int f3 = !opaque[XYZ(x, y + 1, z)];
//        int f4 = !opaque[XYZ(x, y - 1, z)] && (ey > 0);
//        int f5 = !opaque[XYZ(x, y, z - 1)];
//        int f6 = !opaque[XYZ(x, y, z + 1)];
//        int total = f1 + f2 + f3 + f4 + f5 + f6;
//        if (total == 0) {
//            continue;
//        }
//        char neighbors[27] = {0};
//        char lights[27] = {0};
//        float shades[27] = {0};
//        int index = 0;
//        for (int dx = -1; dx <= 1; dx++) {
//            for (int dy = -1; dy <= 1; dy++) {
//                for (int dz = -1; dz <= 1; dz++) {
//                    neighbors[index] = opaque[XYZ(x + dx, y + dy, z + dz)];
//                    lights[index] = light[XYZ(x + dx, y + dy, z + dz)];
//                    shades[index] = 0;
//                    if (y + dy <= highest[XZ(x + dx, z + dz)]) {
//                        for (int oy = 0; oy < 8; oy++) {
//                            if (opaque[XYZ(x + dx, y + dy + oy, z + dz)]) {
//                                shades[index] = 1.0 - oy * 0.125;
//                                break;
//                            }
//                        }
//                    }
//                    index++;
//                }
//            }
//        }
//        float ao[6][4];
//        float light[6][4];
//        occlusion(neighbors, lights, shades, ao, light);
//        if (is_plant(ew)) {
//            total = 4;
//            float min_ao = 1;
//            float max_light = 0;
//            for (int a = 0; a < 6; a++) {
//                for (int b = 0; b < 4; b++) {
//                    min_ao = MIN(min_ao, ao[a][b]);
//                    max_light = MAX(max_light, light[a][b]);
//                }
//            }
//            float rotation = simplex2(ex, ez, 4, 0.5, 2) * 360;
//            make_plant(
//                       data + offset, min_ao, max_light,
//                       ex, ey, ez, 0.5, ew, rotation);
//        }
//        else {
//            make_cube(
//                      data + offset, ao, light,
//                      f1, f2, f3, f4, f5, f6,
//                      ex, ey, ez, 0.5, ew);
//        }
//        offset += total * 60;
//    } END_MAP_FOR_EACH;
//    
//    free(opaque);
//    free(light);
//    free(highest);
//    
//    item->miny = miny;
//    item->maxy = maxy;
//    item->faces = faces;
//    item->data = data;
//}
//
//void generate_chunk(Chunk *chunk, WorkerItem *item) {
//    chunk->miny = item->miny;
//    chunk->maxy = item->maxy;
//    chunk->faces = item->faces;
//    del_buffer(chunk->buffer);
//    chunk->buffer = gen_faces(10, item->faces, item->data);
//    gen_sign_buffer(chunk);
//}
//
//void gen_chunk_buffer(Chunk *chunk) {
//    WorkerItem _item;
//    WorkerItem *item = &_item;
//    item->p = chunk->p;
//    item->q = chunk->q;
//    for (int dp = -1; dp <= 1; dp++) {
//        for (int dq = -1; dq <= 1; dq++) {
//            Chunk *other = chunk;
//            if (dp || dq) {
//                other = find_chunk(chunk->p + dp, chunk->q + dq);
//            }
//            if (other) {
//                item->block_maps[dp + 1][dq + 1] = &other->map;
//                item->light_maps[dp + 1][dq + 1] = &other->lights;
//            }
//            else {
//                item->block_maps[dp + 1][dq + 1] = 0;
//                item->light_maps[dp + 1][dq + 1] = 0;
//            }
//        }
//    }
//    compute_chunk(item);
//    generate_chunk(chunk, item);
//    chunk->dirty = 0;
//}
//
//void load_chunk(WorkerItem *item) {
//    int p = item->p;
//    int q = item->q;
//    Map *block_map = item->block_maps[1][1];
//    Map *light_map = item->light_maps[1][1];
//    create_world(p, q, map_set_func, block_map);
//    db_load_blocks(block_map, p, q);
//    db_load_lights(light_map, p, q);
//}
//
//void request_chunk(int p, int q) {
//    int key = db_get_key(p, q);
//    client_chunk(p, q, key);
//}
//
//void init_chunk(Chunk *chunk, int p, int q) {
//    chunk->p = p;
//    chunk->q = q;
//    chunk->faces = 0;
//    chunk->sign_faces = 0;
//    chunk->buffer = 0;
//    chunk->sign_buffer = 0;
//    dirty_chunk(chunk);
//    SignList *signs = &chunk->signs;
//    sign_list_alloc(signs, 16);
//    db_load_signs(signs, p, q);
//    Map *block_map = &chunk->map;
//    Map *light_map = &chunk->lights;
//    int dx = p * CHUNK_SIZE - 1;
//    int dy = 0;
//    int dz = q * CHUNK_SIZE - 1;
//    map_alloc(block_map, dx, dy, dz, 0x7fff);
//    map_alloc(light_map, dx, dy, dz, 0xf);
//}
