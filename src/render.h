#ifndef _render_h_
#define _render_h_

#include <string.h>
#include "collide.h"
#include "draw.h"
#include "gen.h"
#include "item.h"
#include "struct.h"
#include "handlers.h"

int render_chunks(Attrib *attrib, Player *player, Model *g);

void render_signs(Attrib *attrib, Player *player, Model *g);

void render_sign(Attrib *attrib, Player *player, Model *g);

void render_players(Attrib *attrib, Player *player, Model *g);

void render_sky(Attrib *attrib, Player *player, GLuint buffer, Model *g);

void render_wireframe(Attrib *attrib, Player *player, Model *g);

void render_crosshairs(Attrib *attrib, Model *g);

void render_item(Attrib *attrib, Model *g);

void render_text(Attrib *attrib, int justify, float x, float y, float n, char *text, Model *g);

float time_of_day(Model *g);

float get_daylight(Model *g);

void check_workers(Model *g);

void ensure_chunks_worker(Player *player, Worker *worker, Model *g);

void ensure_chunks(Player *player, Model *g);

#endif
