#ifndef _block_h_
#define _block_h_

#include "client.h"
#include "chunk.h"
#include "handlers.h"
#include "set.h"
#include "struct.h"

void _set_block(int p, int q, int x, int y, int z, int w, int dirty, Model *g);

void set_block(int x, int y, int z, int w, Model *g);

void record_block(int x, int y, int z, int w, Model *g);

int get_block(int x, int y, int z, Model *g);

#endif
