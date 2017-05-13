#ifndef _chunk_h_
#define _chunk_h_

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "cube.h"
#include "db.h"
#include "gen.h"
#include "item.h"
#include "light.h"
#include "map.h"
#include "model.h"
#include "noise.h"
#include "struct.h"
#include "util.h"
#include "world.h"

int chunked(float x);

Chunk *find_chunk(int p, int q, Model *g);

int chunk_distance(Chunk *chunk, int p, int q);

int chunk_visible(float planes[6][4], int p, int q, int miny, int maxy, Model *g);

int highest_block(float x, float z, Model *g);

void dirty_chunk(Chunk *chunk, Model *g);

void create_chunk(Chunk *chunk, int p, int q, Model *g);

void delete_chunks(Model *g);

void delete_all_chunks(Model *g);

void compute_chunk(WorkerItem *item);

void load_chunk(WorkerItem *item);

void request_chunk(int p, int q);

void init_chunk(Chunk *chunk, int p, int q, Model *g);


void generate_chunk(Chunk *chunk, WorkerItem *item);

void gen_chunk_buffer(Chunk *chunk, Model *g);

#endif
