#ifndef _on_h_
#define _on_h_

// THIS CURRENTLY HAS EVERYTHING IN IT THAT WAS IN MAIN.C
// THIS STILL NEEDS TO BE SPLIT OUT

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <curl/curl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auth.h"
#include "callbacks.h"
#include "chunk.h"
#include "client.h"
#include "collide.h"
#include "config.h"
#include "cube.h"
#include "db.h"
#include "draw.h"
#include "gen.h"
#include "light.h"
#include "item.h"
#include "map.h"
#include "model.h"
#include "matrix.h"
#include "noise.h"
#include "player.h"
#include "sign.h"
#include "struct.h"
#include "tinycthread.h"
#include "util.h"
#include "world.h"

float time_of_day(Model *g);

float get_daylight(Model *g);

int get_scale_factor(Model *g);

void on_light(Model *g);

void on_left_click(Model *g);

void on_right_click(Model *g);

void on_middle_click(Model *g);

int _hit_test(Map *map, float max_distance, int previous,
              float x, float y, float z,
              float vx, float vy, float vz,
              int *hx, int *hy, int *hz, Model *g);

int hit_test(int previous, float x, float y, float z, float rx, float ry, int *bx, int *by, int *bz, Model *g);

int hit_test_face(Player *player, int *x, int *y, int *z, int *face, Model *g);
int collide(int height, float *x, float *y, float *z, Model *g);

int player_intersects_block(int height, float x, float y, float z, int hx, int hy, int hz);


void check_workers(Model *g);

void force_chunks(Player *player, Model *g);

void ensure_chunks_worker(Player *player, Worker *worker, Model *g);

void ensure_chunks(Player *player, Model *g);

int worker_run(void *arg);

void unset_sign(int x, int y, int z, Model *g);

void unset_sign_face(int x, int y, int z, int face, Model *g);

void _set_sign(int p, int q, int x, int y, int z, int face, const char *text, int dirty, Model *g);

void set_sign(int x, int y, int z, int face, const char *text, Model *g);

void toggle_light(int x, int y, int z, Model *g);

void set_light(int p, int q, int x, int y, int z, int w, Model *g);

void _set_block(int p, int q, int x, int y, int z, int w, int dirty, Model *g);

void set_block(int x, int y, int z, int w, Model *g);

void record_block(int x, int y, int z, int w, Model *g);

int get_block(int x, int y, int z, Model *g);

void builder_block(int x, int y, int z, int w, Model *g);

int render_chunks(Attrib *attrib, Player *player, Model *g);

void render_signs(Attrib *attrib, Player *player, Model *g);

void render_sign(Attrib *attrib, Player *player, Model *g);

void render_players(Attrib *attrib, Player *player, Model *g);

void render_sky(Attrib *attrib, Player *player, GLuint buffer, Model *g);

void render_wireframe(Attrib *attrib, Player *player, Model *g);

void render_crosshairs(Attrib *attrib, Model *g);

void render_item(Attrib *attrib, Model *g);

void render_text(Attrib *attrib, int justify, float x, float y, float n, char *text, Model *g);

void add_message(const char *text, Model *g);

void login();

void copy(Model *g);

void paste(Model *g);

void array(Block *b1, Block *b2, int xc, int yc, int zc, Model *g);

void cube(Block *b1, Block *b2, int fill, Model *g);

void sphere(Block *center, int radius, int fill, int fx, int fy, int fz, Model *g);

void cylinder(Block *b1, Block *b2, int radius, int fill, Model *g);

void tree(Block *block, Model *g);

void parse_command(const char *buffer, int forward, Model *g);

void create_window(Model *g);

void handle_mouse_input(Model *g);

void handle_movement(double dt, Model *g);

void parse_buffer(char *buffer, Model *g);

void reset_model(Model *g);

#endif
