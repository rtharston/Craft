#ifndef _callbacks_h_
#define _callbacks_h_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include "item.h"
#include "set.h"
#include "struct.h"
#include "on.h"

void setGameModel(Model *g);

void on_key(GLFWwindow *window, int key, int scancode, int action, int mods);

void on_char(GLFWwindow *window, unsigned int u);

void on_scroll(GLFWwindow *window, double xdelta, double ydelta);

void on_mouse_button(GLFWwindow *window, int button, int action, int mods);

#endif
