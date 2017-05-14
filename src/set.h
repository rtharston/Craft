#ifndef _set_h_
#define _set_h_

#include "chunk.h"
#include "struct.h"

void unset_sign(int x, int y, int z, Model *g);

void unset_sign_face(int x, int y, int z, int face, Model *g);

void _set_sign(int p, int q, int x, int y, int z, int face, const char *text, int dirty, Model *g);

void set_sign(int x, int y, int z, int face, const char *text, Model *g);

void set_light(int p, int q, int x, int y, int z, int w, Model *g);

#endif
