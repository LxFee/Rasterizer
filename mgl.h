#pragma once
#include "SDL2/SDL.h"
#include "Framebuffer.h"
#include "cassert"
#include "Shader.h"

void mgl_init(const char *title, int w, int h);

void mgl_quit();

void mgl_draw(int vbo, int count, Shader* shader, Framebuffer* fb);

void mgl_clear(vec4 col, Framebuffer* fb = NULL);

void mgl_update(Framebuffer* fb = NULL);

int mgl_create_vbo(int component_sz, int points_num);