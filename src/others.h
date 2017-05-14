#ifndef _others_h_
#define _others_h_

#include <math.h>
#include "chunk.h"
#include "collide.h"
#include "item.h"
#include "matrix.h"
#include "parse.h"
#include "struct.h"
#include "util.h"

float time_of_day(Model *g);

float get_daylight(Model *g);

void check_workers(Model *g);

void ensure_chunks_worker(Player *player, Worker *worker, Model *g);

void ensure_chunks(Player *player, Model *g);

int worker_run(void *arg);

void unset_sign(int x, int y, int z, Model *g);

void unset_sign_face(int x, int y, int z, int face, Model *g);

void _set_sign(int p, int q, int x, int y, int z, int face, const char *text, int dirty, Model *g);

void set_sign(int x, int y, int z, int face, const char *text, Model *g);

void toggle_light(int x, int y, int z, Model *g);

void set_light(int p, int q, int x, int y, int z, int w, Model *g);

void handle_mouse_input(Model *g);

void handle_movement(double dt, Model *g);

#endif
