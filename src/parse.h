#ifndef _parse_h_
#define _parse_h_

#include <stdio.h>
#include "block.h"
#include "chunk.h"
#include "commands.h"
#include "player.h"
#include "struct.h"
#include "others.h"

void parse_buffer(char *buffer, Model *g);

void force_chunks(Player *player, Model *g);

#endif
