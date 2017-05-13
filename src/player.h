#ifndef _player_h_
#define _player_h_

#include <math.h>
#include <string.h>
#include "collide.h"
//#include "model.h"
#include "struct.h"
#include "util.h"
#include "gen.h"

Player *find_player(int id, Model *g);

void update_player(Player *player,
                   float x, float y, float z, float rx, float ry, int interpolate);

void interpolate_player(Player *player);

void delete_player(int pid, Model *g);

void delete_all_players(Model *g);

float player_player_distance(Player *p1, Player *p2);

float player_crosshair_distance(Player *p1, Player *p2);

Player *player_crosshair(Player *player, Model *g);

void get_sight_vector(float rx, float ry, float *vx, float *vy, float *vz);

void get_motion_vector(int flying, int sz, int sx, float rx, float ry, float *vx, float *vy, float *vz);

#endif
