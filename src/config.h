#ifndef _config_h_
#define _config_h_

// app parameters
#define DEBUG 0
#define FULLSCREEN 0
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define VSYNC 1
#define SCROLL_THRESHOLD 0.1
#define MAX_MESSAGES 4
#define DB_PATH "craft.db"
#define USE_CACHE 1
#define DAY_LENGTH 600
#define INVERT_MOUSE 0

// rendering options
#define SHOW_LIGHTS 1
#define SHOW_PLANTS 1
#define SHOW_CLOUDS 1
#define SHOW_TREES 1
#define SHOW_ITEM 1
#define SHOW_CROSSHAIRS 1
#define SHOW_WIREFRAME 1
#define SHOW_INFO_TEXT 1
#define SHOW_CHAT_TEXT 1
#define SHOW_PLAYER_NAMES 1

// key bindings
#define CRAFT_KEY_FORWARD 'W'
#define CRAFT_KEY_BACKWARD 'S'
#define CRAFT_KEY_LEFT 'A'
#define CRAFT_KEY_RIGHT 'D'
#define CRAFT_KEY_JUMP GLFW_KEY_SPACE
#define CRAFT_KEY_FLY GLFW_KEY_TAB
#define CRAFT_KEY_OBSERVE 'O'
#define CRAFT_KEY_OBSERVE_INSET 'P'
#define CRAFT_KEY_ITEM_NEXT 'E'
#define CRAFT_KEY_ITEM_PREV 'R'
#define CRAFT_KEY_ZOOM GLFW_KEY_LEFT_SHIFT
#define CRAFT_KEY_ORTHO 'F'
#define CRAFT_KEY_CHAT 't'
#define CRAFT_KEY_COMMAND '/'
#define CRAFT_KEY_SIGN '`'

// advanced parameters
#define CREATE_CHUNK_RADIUS 10
#define RENDER_CHUNK_RADIUS 10
#define RENDER_SIGN_RADIUS 4
#define DELETE_CHUNK_RADIUS 14
#define CHUNK_SIZE 32
#define COMMIT_INTERVAL 5

// stolen from main.c

#define MAX_CHUNKS 8192
#define MAX_PLAYERS 128
#define WORKERS 4
#define MAX_TEXT_LENGTH 256
#define MAX_NAME_LENGTH 32
#define MAX_PATH_LENGTH 256
#define MAX_ADDR_LENGTH 256

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

#define MODE_OFFLINE 0
#define MODE_ONLINE 1

#define WORKER_IDLE 0
#define WORKER_BUSY 1
#define WORKER_DONE 2

#define XZ_SIZE (CHUNK_SIZE * 3 + 2)
#define XZ_LO (CHUNK_SIZE)
#define XZ_HI (CHUNK_SIZE * 2 + 1)
#define Y_SIZE 258
#define XYZ(x, y, z) ((y) * XZ_SIZE * XZ_SIZE + (x) * XZ_SIZE + (z))
#define XZ(x, z) ((x) * XZ_SIZE + (z))

#endif
