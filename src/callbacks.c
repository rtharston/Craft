#include "callbacks.h"

Model *gameModel;

void setGameModel(Model *g) {
    gameModel = g;
}

void on_key(GLFWwindow *window, int key, int scancode, int action, int mods) {
    int control = mods & (GLFW_MOD_CONTROL | GLFW_MOD_SUPER);
    int exclusive =
    glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    if (action == GLFW_RELEASE) {
        return;
    }
    if (key == GLFW_KEY_BACKSPACE) {
        if (gameModel->typing) {
            int n = strlen(gameModel->typing_buffer);
            if (n > 0) {
                gameModel->typing_buffer[n - 1] = '\0';
            }
        }
    }
    if (action != GLFW_PRESS) {
        return;
    }
    if (key == GLFW_KEY_ESCAPE) {
        if (gameModel->typing) {
            gameModel->typing = 0;
        }
        else if (exclusive) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    if (key == GLFW_KEY_ENTER) {
        if (gameModel->typing) {
            if (mods & GLFW_MOD_SHIFT) {
                int n = strlen(gameModel->typing_buffer);
                if (n < MAX_TEXT_LENGTH - 1) {
                    gameModel->typing_buffer[n] = '\r';
                    gameModel->typing_buffer[n + 1] = '\0';
                }
            }
            else {
                gameModel->typing = 0;
                if (gameModel->typing_buffer[0] == CRAFT_KEY_SIGN) {
                    Player *player = gameModel->players;
                    int x, y, z, face;
                    if (hit_test_face(player, &x, &y, &z, &face, gameModel)) {
                        set_sign(x, y, z, face, gameModel->typing_buffer + 1, gameModel);
                    }
                }
                else if (gameModel->typing_buffer[0] == '/') {
                    parse_command(gameModel->typing_buffer, 1, gameModel);
                }
                else {
                    client_talk(gameModel->typing_buffer);
                }
            }
        }
        else {
            if (control) {
                on_right_click(gameModel);
            }
            else {
                on_left_click(gameModel);
            }
        }
    }
    if (control && key == 'V') {
        const char *buffer = glfwGetClipboardString(window);
        if (gameModel->typing) {
            gameModel->suppress_char = 1;
            strncat(gameModel->typing_buffer, buffer,
                    MAX_TEXT_LENGTH - strlen(gameModel->typing_buffer) - 1);
        }
        else {
            parse_command(buffer, 0, gameModel);
        }
    }
    if (!gameModel->typing) {
        if (key == CRAFT_KEY_FLY) {
            gameModel->flying = !gameModel->flying;
        }
        if (key >= '1' && key <= '9') {
            gameModel->item_index = key - '1';
        }
        if (key == '0') {
            gameModel->item_index = 9;
        }
        if (key == CRAFT_KEY_ITEM_NEXT) {
            gameModel->item_index = (gameModel->item_index + 1) % item_count;
        }
        if (key == CRAFT_KEY_ITEM_PREV) {
            gameModel->item_index--;
            if (gameModel->item_index < 0) {
                gameModel->item_index = item_count - 1;
            }
        }
        if (key == CRAFT_KEY_OBSERVE) {
            gameModel->observe1 = (gameModel->observe1 + 1) % gameModel->player_count;
        }
        if (key == CRAFT_KEY_OBSERVE_INSET) {
            gameModel->observe2 = (gameModel->observe2 + 1) % gameModel->player_count;
        }
    }
}

void on_char(GLFWwindow *window, unsigned int u) {
    if (gameModel->suppress_char) {
        gameModel->suppress_char = 0;
        return;
    }
    if (gameModel->typing) {
        if (u >= 32 && u < 128) {
            char c = (char)u;
            int n = strlen(gameModel->typing_buffer);
            if (n < MAX_TEXT_LENGTH - 1) {
                gameModel->typing_buffer[n] = c;
                gameModel->typing_buffer[n + 1] = '\0';
            }
        }
    }
    else {
        if (u == CRAFT_KEY_CHAT) {
            gameModel->typing = 1;
            gameModel->typing_buffer[0] = '\0';
        }
        if (u == CRAFT_KEY_COMMAND) {
            gameModel->typing = 1;
            gameModel->typing_buffer[0] = '/';
            gameModel->typing_buffer[1] = '\0';
        }
        if (u == CRAFT_KEY_SIGN) {
            gameModel->typing = 1;
            gameModel->typing_buffer[0] = CRAFT_KEY_SIGN;
            gameModel->typing_buffer[1] = '\0';
        }
    }
}

void on_scroll(GLFWwindow *window, double xdelta, double ydelta) {
    static double ypos = 0;
    ypos += ydelta;
    if (ypos < -SCROLL_THRESHOLD) {
        gameModel->item_index = (gameModel->item_index + 1) % item_count;
        ypos = 0;
    }
    if (ypos > SCROLL_THRESHOLD) {
        gameModel->item_index--;
        if (gameModel->item_index < 0) {
            gameModel->item_index = item_count - 1;
        }
        ypos = 0;
    }
}

void on_mouse_button(GLFWwindow *window, int button, int action, int mods) {
    int control = mods & (GLFW_MOD_CONTROL | GLFW_MOD_SUPER);
    int exclusive =
    glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
    if (action != GLFW_PRESS) {
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (exclusive) {
            if (control) {
                on_right_click(gameModel);
            }
            else {
                on_left_click(gameModel);
            }
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (exclusive) {
            if (control) {
                on_light(gameModel);
            }
            else {
                on_right_click(gameModel);
            }
        }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (exclusive) {
            on_middle_click(gameModel);
        }
    }
}
