#ifndef _collide_h_
#define _collide_h_

#include <math.h>
#include "chunk.h"
#include "map.h"
#include "model.h"
#include "player.h"
#include "struct.h"
#include "util.h"

int _hit_test(Map *map, float max_distance, int previous,
              float x, float y, float z,
              float vx, float vy, float vz,
              int *hx, int *hy, int *hz, Model *g);

int hit_test(int previous, float x, float y, float z, float rx, float ry, int *bx, int *by, int *bz, Model *g);

int hit_test_face(Player *player, int *x, int *y, int *z, int *face, Model *g);

int collide(int height, float *x, float *y, float *z, Model *g);

int player_intersects_block(int height, float x, float y, float z, int hx, int hy, int hz);

#endif
