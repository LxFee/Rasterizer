#pragma once
#include "SDL2/SDL.h"
#include "cassert"
#include "Shader.h"

#define MGL_COLOR (1 << 0)
#define MGL_DEPTH (1 << 1)

void mgl_init(const char *title, int w, int h);

void mgl_quit();

void mgl_draw(int vbo_ind, int ebo_ind, Shader* shader);

void mgl_set_init_zbuffer(float z);

void mgl_set_init_color(vec4 col);

void mgl_clear(int flag);

void mgl_update();

int mgl_create_vbo();

int mgl_create_ebo();

int mgl_vertex_index_pointer(int ebo_ind, int length, int* ind);

int mgl_vertex_attrib_pointer(int vbo_ind, int size, float* data);

float* mgl_query_vbo(int vbo_ind, int index, int location, int* size);