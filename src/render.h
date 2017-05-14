#ifndef _render_h_
#define _render_h_

#include <string.h>
#include "collide.h"
#include "draw.h"
#include "gen.h"
#include "item.h"
#include "struct.h"
#include "others.h"

int render_chunks(Attrib *attrib, Player *player, Model *g);

void render_signs(Attrib *attrib, Player *player, Model *g);

void render_sign(Attrib *attrib, Player *player, Model *g);

void render_players(Attrib *attrib, Player *player, Model *g);

void render_sky(Attrib *attrib, Player *player, GLuint buffer, Model *g);

void render_wireframe(Attrib *attrib, Player *player, Model *g);

void render_crosshairs(Attrib *attrib, Model *g);

void render_item(Attrib *attrib, Model *g);

void render_text(Attrib *attrib, int justify, float x, float y, float n, char *text, Model *g);

#endif
