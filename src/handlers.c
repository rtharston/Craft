#include "handlers.h"

void handle_mouse_input(Model *g) {
    int exclusive =
    glfwGetInputMode(g->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    static double px = 0;
    static double py = 0;
    State *s = &g->players->state;
    if (exclusive && (px || py)) {
        double mx, my;
        glfwGetCursorPos(g->window, &mx, &my);
        float m = 0.0025;
        s->rx += (mx - px) * m;
        if (INVERT_MOUSE) {
            s->ry += (my - py) * m;
        }
        else {
            s->ry -= (my - py) * m;
        }
        if (s->rx < 0) {
            s->rx += RADIANS(360);
        }
        if (s->rx >= RADIANS(360)){
            s->rx -= RADIANS(360);
        }
        s->ry = MAX(s->ry, -RADIANS(90));
        s->ry = MIN(s->ry, RADIANS(90));
        px = mx;
        py = my;
    }
    else {
        glfwGetCursorPos(g->window, &px, &py);
    }
}

void handle_movement(double dt, Model *g) {
    static float dy = 0;
    State *s = &g->players->state;
    int sz = 0;
    int sx = 0;
    if (!g->typing) {
        float m = dt * 1.0;
        g->ortho = glfwGetKey(g->window, CRAFT_KEY_ORTHO) ? 64 : 0;
        g->fov = glfwGetKey(g->window, CRAFT_KEY_ZOOM) ? 15 : 65;
        if (glfwGetKey(g->window, CRAFT_KEY_FORWARD)) sz--;
        if (glfwGetKey(g->window, CRAFT_KEY_BACKWARD)) sz++;
        if (glfwGetKey(g->window, CRAFT_KEY_LEFT)) sx--;
        if (glfwGetKey(g->window, CRAFT_KEY_RIGHT)) sx++;
        if (glfwGetKey(g->window, GLFW_KEY_LEFT)) s->rx -= m;
        if (glfwGetKey(g->window, GLFW_KEY_RIGHT)) s->rx += m;
        if (glfwGetKey(g->window, GLFW_KEY_UP)) s->ry += m;
        if (glfwGetKey(g->window, GLFW_KEY_DOWN)) s->ry -= m;
    }
    float vx, vy, vz;
    get_motion_vector(g->flying, sz, sx, s->rx, s->ry, &vx, &vy, &vz);
    if (!g->typing) {
        if (glfwGetKey(g->window, CRAFT_KEY_JUMP)) {
            if (g->flying) {
                vy = 1;
            }
            else if (dy == 0) {
                dy = 8;
            }
        }
    }
    float speed = g->flying ? 20 : 5;
    int estimate = roundf(sqrtf(
                                powf(vx * speed, 2) +
                                powf(vy * speed + ABS(dy) * 2, 2) +
                                powf(vz * speed, 2)) * dt * 8);
    int step = MAX(8, estimate);
    float ut = dt / step;
    vx = vx * ut * speed;
    vy = vy * ut * speed;
    vz = vz * ut * speed;
    for (int i = 0; i < step; i++) {
        if (g->flying) {
            dy = 0;
        }
        else {
            dy -= ut * 25;
            dy = MAX(dy, -250);
        }
        s->x += vx;
        s->y += vy + dy * ut;
        s->z += vz;
        if (collide(2, &s->x, &s->y, &s->z, g)) {
            dy = 0;
        }
    }
    if (s->y < 0) {
        s->y = highest_block(s->x, s->z, g) + 2;
    }
}
