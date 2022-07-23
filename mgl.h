#pragma once
#include "SDL2/SDL.h"
#include "Framebuffer.h"
#include "cassert"

void mgl_init(const char *title, int w, int h);

void mgl_quit();

void mgl_draw(Framebuffer* fb, int vbo);

void mgl_clear(vec4 col);

void mgl_update(Framebuffer* fb);