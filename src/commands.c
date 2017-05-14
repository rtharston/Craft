#include "commands.h"

void parse_command(const char *buffer, int forward, Model *g) {
    char username[128] = {0};
    char token[128] = {0};
    char server_addr[MAX_ADDR_LENGTH];
    int server_port = DEFAULT_PORT;
    char filename[MAX_PATH_LENGTH];
    int radius, count, xc, yc, zc;
    if (sscanf(buffer, "/identity %128s %128s", username, token) == 2) {
        db_auth_set(username, token);
        add_message("Successfully imported identity token!", g);
        login();
    }
    else if (strcmp(buffer, "/logout") == 0) {
        db_auth_select_none();
        login();
    }
    else if (sscanf(buffer, "/login %128s", username) == 1) {
        if (db_auth_select(username)) {
            login();
        }
        else {
            add_message("Unknown username.", g);
        }
    }
    else if (sscanf(buffer,
                    "/online %128s %d", server_addr, &server_port) >= 1)
    {
        g->mode_changed = 1;
        g->mode = MODE_ONLINE;
        strncpy(g->server_addr, server_addr, MAX_ADDR_LENGTH);
        g->server_port = server_port;
        snprintf(g->db_path, MAX_PATH_LENGTH,
                 "cache.%s.%d.db", g->server_addr, g->server_port);
    }
    else if (sscanf(buffer, "/offline %128s", filename) == 1) {
        g->mode_changed = 1;
        g->mode = MODE_OFFLINE;
        snprintf(g->db_path, MAX_PATH_LENGTH, "%s.db", filename);
    }
    else if (strcmp(buffer, "/offline") == 0) {
        g->mode_changed = 1;
        g->mode = MODE_OFFLINE;
        snprintf(g->db_path, MAX_PATH_LENGTH, "%s", DB_PATH);
    }
    else if (sscanf(buffer, "/view %d", &radius) == 1) {
        if (radius >= 1 && radius <= 24) {
            g->create_radius = radius;
            g->render_radius = radius;
            g->delete_radius = radius + 4;
        }
        else {
            add_message("Viewing distance must be between 1 and 24.", g);
        }
    }
    else if (strcmp(buffer, "/copy") == 0) {
        copy(g);
    }
    else if (strcmp(buffer, "/paste") == 0) {
        paste(g);
    }
    else if (strcmp(buffer, "/tree") == 0) {
        tree(&g->block0, g);
    }
    else if (sscanf(buffer, "/array %d %d %d", &xc, &yc, &zc) == 3) {
        array(&g->block1, &g->block0, xc, yc, zc, g);
    }
    else if (sscanf(buffer, "/array %d", &count) == 1) {
        array(&g->block1, &g->block0, count, count, count, g);
    }
    else if (strcmp(buffer, "/fcube") == 0) {
        cube(&g->block0, &g->block1, 1, g);
    }
    else if (strcmp(buffer, "/cube") == 0) {
        cube(&g->block0, &g->block1, 0, g);
    }
    else if (sscanf(buffer, "/fsphere %d", &radius) == 1) {
        sphere(&g->block0, radius, 1, 0, 0, 0, g);
    }
    else if (sscanf(buffer, "/sphere %d", &radius) == 1) {
        sphere(&g->block0, radius, 0, 0, 0, 0, g);
    }
    else if (sscanf(buffer, "/fcirclex %d", &radius) == 1) {
        sphere(&g->block0, radius, 1, 1, 0, 0, g);
    }
    else if (sscanf(buffer, "/circlex %d", &radius) == 1) {
        sphere(&g->block0, radius, 0, 1, 0, 0, g);
    }
    else if (sscanf(buffer, "/fcircley %d", &radius) == 1) {
        sphere(&g->block0, radius, 1, 0, 1, 0, g);
    }
    else if (sscanf(buffer, "/circley %d", &radius) == 1) {
        sphere(&g->block0, radius, 0, 0, 1, 0, g);
    }
    else if (sscanf(buffer, "/fcirclez %d", &radius) == 1) {
        sphere(&g->block0, radius, 1, 0, 0, 1, g);
    }
    else if (sscanf(buffer, "/circlez %d", &radius) == 1) {
        sphere(&g->block0, radius, 0, 0, 0, 1, g);
    }
    else if (sscanf(buffer, "/fcylinder %d", &radius) == 1) {
        cylinder(&g->block0, &g->block1, radius, 1, g);
    }
    else if (sscanf(buffer, "/cylinder %d", &radius) == 1) {
        cylinder(&g->block0, &g->block1, radius, 0, g);
    }
    else if (forward) {
        client_talk(buffer);
    }
}

void add_message(const char *text, Model *g) {
    printf("%s\n", text);
    snprintf(
             g->messages[g->message_index], MAX_TEXT_LENGTH, "%s", text);
    g->message_index = (g->message_index + 1) % MAX_MESSAGES;
}

void copy(Model *g) {
    memcpy(&g->copy0, &g->block0, sizeof(Block));
    memcpy(&g->copy1, &g->block1, sizeof(Block));
}

void paste(Model *g) {
    Block *c1 = &g->copy1;
    Block *c2 = &g->copy0;
    Block *p1 = &g->block1;
    Block *p2 = &g->block0;
    int scx = SIGN(c2->x - c1->x);
    int scz = SIGN(c2->z - c1->z);
    int spx = SIGN(p2->x - p1->x);
    int spz = SIGN(p2->z - p1->z);
    int oy = p1->y - c1->y;
    int dx = ABS(c2->x - c1->x);
    int dz = ABS(c2->z - c1->z);
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x <= dx; x++) {
            for (int z = 0; z <= dz; z++) {
                int w = get_block(c1->x + x * scx, y, c1->z + z * scz, g);
                builder_block(p1->x + x * spx, y + oy, p1->z + z * spz, w, g);
            }
        }
    }
}

void array(Block *b1, Block *b2, int xc, int yc, int zc, Model *g) {
    if (b1->w != b2->w) {
        return;
    }
    int w = b1->w;
    int dx = b2->x - b1->x;
    int dy = b2->y - b1->y;
    int dz = b2->z - b1->z;
    xc = dx ? xc : 1;
    yc = dy ? yc : 1;
    zc = dz ? zc : 1;
    for (int i = 0; i < xc; i++) {
        int x = b1->x + dx * i;
        for (int j = 0; j < yc; j++) {
            int y = b1->y + dy * j;
            for (int k = 0; k < zc; k++) {
                int z = b1->z + dz * k;
                builder_block(x, y, z, w, g);
            }
        }
    }
}

void cube(Block *b1, Block *b2, int fill, Model *g) {
    if (b1->w != b2->w) {
        return;
    }
    int w = b1->w;
    int x1 = MIN(b1->x, b2->x);
    int y1 = MIN(b1->y, b2->y);
    int z1 = MIN(b1->z, b2->z);
    int x2 = MAX(b1->x, b2->x);
    int y2 = MAX(b1->y, b2->y);
    int z2 = MAX(b1->z, b2->z);
    int a = (x1 == x2) + (y1 == y2) + (z1 == z2);
    for (int x = x1; x <= x2; x++) {
        for (int y = y1; y <= y2; y++) {
            for (int z = z1; z <= z2; z++) {
                if (!fill) {
                    int n = 0;
                    n += x == x1 || x == x2;
                    n += y == y1 || y == y2;
                    n += z == z1 || z == z2;
                    if (n <= a) {
                        continue;
                    }
                }
                builder_block(x, y, z, w, g);
            }
        }
    }
}

void sphere(Block *center, int radius, int fill, int fx, int fy, int fz, Model *g) {
    static const float offsets[8][3] = {
        {-0.5, -0.5, -0.5},
        {-0.5, -0.5, 0.5},
        {-0.5, 0.5, -0.5},
        {-0.5, 0.5, 0.5},
        {0.5, -0.5, -0.5},
        {0.5, -0.5, 0.5},
        {0.5, 0.5, -0.5},
        {0.5, 0.5, 0.5}
    };
    int cx = center->x;
    int cy = center->y;
    int cz = center->z;
    int w = center->w;
    for (int x = cx - radius; x <= cx + radius; x++) {
        if (fx && x != cx) {
            continue;
        }
        for (int y = cy - radius; y <= cy + radius; y++) {
            if (fy && y != cy) {
                continue;
            }
            for (int z = cz - radius; z <= cz + radius; z++) {
                if (fz && z != cz) {
                    continue;
                }
                int inside = 0;
                int outside = fill;
                for (int i = 0; i < 8; i++) {
                    float dx = x + offsets[i][0] - cx;
                    float dy = y + offsets[i][1] - cy;
                    float dz = z + offsets[i][2] - cz;
                    float d = sqrtf(dx * dx + dy * dy + dz * dz);
                    if (d < radius) {
                        inside = 1;
                    }
                    else {
                        outside = 1;
                    }
                }
                if (inside && outside) {
                    builder_block(x, y, z, w, g);
                }
            }
        }
    }
}

void cylinder(Block *b1, Block *b2, int radius, int fill, Model *g) {
    if (b1->w != b2->w) {
        return;
    }
    int w = b1->w;
    int x1 = MIN(b1->x, b2->x);
    int y1 = MIN(b1->y, b2->y);
    int z1 = MIN(b1->z, b2->z);
    int x2 = MAX(b1->x, b2->x);
    int y2 = MAX(b1->y, b2->y);
    int z2 = MAX(b1->z, b2->z);
    int fx = x1 != x2;
    int fy = y1 != y2;
    int fz = z1 != z2;
    if (fx + fy + fz != 1) {
        return;
    }
    Block block = {x1, y1, z1, w};
    if (fx) {
        for (int x = x1; x <= x2; x++) {
            block.x = x;
            sphere(&block, radius, fill, 1, 0, 0, g);
        }
    }
    if (fy) {
        for (int y = y1; y <= y2; y++) {
            block.y = y;
            sphere(&block, radius, fill, 0, 1, 0, g);
        }
    }
    if (fz) {
        for (int z = z1; z <= z2; z++) {
            block.z = z;
            sphere(&block, radius, fill, 0, 0, 1, g);
        }
    }
}

void tree(Block *block, Model *g) {
    int bx = block->x;
    int by = block->y;
    int bz = block->z;
    for (int y = by + 3; y < by + 8; y++) {
        for (int dx = -3; dx <= 3; dx++) {
            for (int dz = -3; dz <= 3; dz++) {
                int dy = y - (by + 4);
                int d = (dx * dx) + (dy * dy) + (dz * dz);
                if (d < 11) {
                    builder_block(bx + dx, y, bz + dz, 15, g);
                }
            }
        }
    }
    for (int y = by; y < by + 7; y++) {
        builder_block(bx, y, bz, 5, g);
    }
}

void builder_block(int x, int y, int z, int w, Model *g) {
    if (y <= 0 || y >= 256) {
        return;
    }
    if (is_destructable(get_block(x, y, z, g))) {
        set_block(x, y, z, 0, g);
    }
    if (w) {
        set_block(x, y, z, w, g);
    }
}
