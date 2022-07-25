#pragma once
#include "SDL2/SDL.h"
#include "cassert"
#include "Shader.h"

#define MGL_COLOR (1 << 0)
#define MGL_DEPTH (1 << 1)

void mgl_init(const char *title, int w, int h);

void mgl_quit();

void mgl_draw(int vbo, int count, Shader* shader);

void mgl_setinitz(float z);

void mgl_setinitcol(vec4 col);

void mgl_clear(int flag);

void mgl_update();

int mgl_create_vbo(int component_sz, int points_num);