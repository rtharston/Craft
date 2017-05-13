#include "gen.h"

GLuint gen_crosshair_buffer(Model *g) {
    int x = g->width / 2;
    int y = g->height / 2;
    int p = 10 * g->scale;
    float data[] = {
        x, y - p, x, y + p,
        x - p, y, x + p, y
    };
    return gen_buffer(sizeof(data), data);
}

GLuint gen_wireframe_buffer(float x, float y, float z, float n) {
    float data[72];
    make_cube_wireframe(data, x, y, z, n);
    return gen_buffer(sizeof(data), data);
}

GLuint gen_sky_buffer() {
    float data[12288];
    make_sphere(data, 1, 3);
    return gen_buffer(sizeof(data), data);
}

GLuint gen_cube_buffer(float x, float y, float z, float n, int w) {
    GLfloat *data = malloc_faces(10, 6);
    float ao[6][4] = {0};
    float light[6][4] = {
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5, 0.5}
    };
    make_cube(data, ao, light, 1, 1, 1, 1, 1, 1, x, y, z, n, w);
    return gen_faces(10, 6, data);
}

GLuint gen_plant_buffer(float x, float y, float z, float n, int w) {
    GLfloat *data = malloc_faces(10, 4);
    float ao = 0;
    float light = 1;
    make_plant(data, ao, light, x, y, z, n, w, 45);
    return gen_faces(10, 4, data);
}

GLuint gen_player_buffer(float x, float y, float z, float rx, float ry) {
    GLfloat *data = malloc_faces(10, 6);
    make_player(data, x, y, z, rx, ry);
    return gen_faces(10, 6, data);
}

GLuint gen_text_buffer(float x, float y, float n, char *text) {
    int length = strlen(text);
    GLfloat *data = malloc_faces(4, length);
    for (int i = 0; i < length; i++) {
        make_character(data + i * 24, x, y, n / 2, n, text[i]);
        x += n;
    }
    return gen_faces(4, length, data);
}


int _gen_sign_buffer(
                     GLfloat *data, float x, float y, float z, int face, const char *text)
{
    static const int glyph_dx[8] = {0, 0, -1, 1, 1, 0, -1, 0};
    static const int glyph_dz[8] = {1, -1, 0, 0, 0, -1, 0, 1};
    static const int line_dx[8] = {0, 0, 0, 0, 0, 1, 0, -1};
    static const int line_dy[8] = {-1, -1, -1, -1, 0, 0, 0, 0};
    static const int line_dz[8] = {0, 0, 0, 0, 1, 0, -1, 0};
    if (face < 0 || face >= 8) {
        return 0;
    }
    int count = 0;
    float max_width = 64;
    float line_height = 1.25;
    char lines[1024];
    int rows = wrap(text, max_width, lines, 1024);
    rows = MIN(rows, 5);
    int dx = glyph_dx[face];
    int dz = glyph_dz[face];
    int ldx = line_dx[face];
    int ldy = line_dy[face];
    int ldz = line_dz[face];
    float n = 1.0 / (max_width / 10);
    float sx = x - n * (rows - 1) * (line_height / 2) * ldx;
    float sy = y - n * (rows - 1) * (line_height / 2) * ldy;
    float sz = z - n * (rows - 1) * (line_height / 2) * ldz;
    char *key;
    char *line = tokenize(lines, "\n", &key);
    while (line) {
        int length = strlen(line);
        int line_width = string_width(line);
        line_width = MIN(line_width, max_width);
        float rx = sx - dx * line_width / max_width / 2;
        float ry = sy;
        float rz = sz - dz * line_width / max_width / 2;
        for (int i = 0; i < length; i++) {
            int width = char_width(line[i]);
            line_width -= width;
            if (line_width < 0) {
                break;
            }
            rx += dx * width / max_width / 2;
            rz += dz * width / max_width / 2;
            if (line[i] != ' ') {
                make_character_3d(
                                  data + count * 30, rx, ry, rz, n / 2, face, line[i]);
                count++;
            }
            rx += dx * width / max_width / 2;
            rz += dz * width / max_width / 2;
        }
        sx += n * line_height * ldx;
        sy += n * line_height * ldy;
        sz += n * line_height * ldz;
        line = tokenize(NULL, "\n", &key);
        rows--;
        if (rows <= 0) {
            break;
        }
    }
    return count;
}

void gen_sign_buffer(Chunk *chunk) {
    SignList *signs = &chunk->signs;
    
    // first pass - count characters
    int max_faces = 0;
    for (int i = 0; i < signs->size; i++) {
        Sign *e = signs->data + i;
        max_faces += strlen(e->text);
    }
    
    // second pass - generate geometry
    GLfloat *data = malloc_faces(5, max_faces);
    int faces = 0;
    for (int i = 0; i < signs->size; i++) {
        Sign *e = signs->data + i;
        faces += _gen_sign_buffer(
                                  data + faces * 30, e->x, e->y, e->z, e->face, e->text);
    }
    
    del_buffer(chunk->sign_buffer);
    chunk->sign_buffer = gen_faces(5, faces, data);
    chunk->sign_faces = faces;
}
