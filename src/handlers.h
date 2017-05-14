#ifndef _handlers_h_
#define _handlers_h_

#include <math.h>
#include "chunk.h"
#include "collide.h"
#include "item.h"
#include "matrix.h"
#include "parse.h"
#include "struct.h"
#include "util.h"

void handle_mouse_input(Model *g);

void handle_movement(double dt, Model *g);

#endif
