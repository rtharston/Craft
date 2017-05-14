#ifndef _commands_h_
#define _commands_h_

#include "config.h"
#include "struct.h"
#include "block.h"
#include "util.h"

void parse_command(const char *buffer, int forward, Model *g);
void add_message(const char *text, Model *g);
void copy(Model *g);
void paste(Model *g);
void array(Block *b1, Block *b2, int xc, int yc, int zc, Model *g);
void cube(Block *b1, Block *b2, int fill, Model *g);
void sphere(Block *center, int radius, int fill, int fx, int fy, int fz, Model *g);
void cylinder(Block *b1, Block *b2, int radius, int fill, Model *g);
void tree(Block *block, Model *g);
void builder_block(int x, int y, int z, int w, Model *g);

#endif
