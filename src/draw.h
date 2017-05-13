#ifndef _draw_h_
#define _draw_h_

#include "model.h"
#include "struct.h"

void draw_triangles_3d_ao(Attrib *attrib, GLuint buffer, int count);

void draw_triangles_3d_text(Attrib *attrib, GLuint buffer, int count);

void draw_triangles_3d(Attrib *attrib, GLuint buffer, int count);

void draw_triangles_2d(Attrib *attrib, GLuint buffer, int count);

void draw_lines(Attrib *attrib, GLuint buffer, int components, int count);

void draw_chunk(Attrib *attrib, Chunk *chunk);

void draw_item(Attrib *attrib, GLuint buffer, int count);

void draw_text(Attrib *attrib, GLuint buffer, int length);

void draw_signs(Attrib *attrib, Chunk *chunk);

void draw_sign(Attrib *attrib, GLuint buffer, int length);

void draw_cube(Attrib *attrib, GLuint buffer);

void draw_plant(Attrib *attrib, GLuint buffer);

void draw_player(Attrib *attrib, Player *player);

#endif
