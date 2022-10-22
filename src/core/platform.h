#ifndef RASTERIZER_PLATFORM_H_
#define RASTERIZER_PLATFORM_H_
/* reference:
 * https://github.com/zauonlok/renderer/blob/master/renderer/core/platform.h */

#include "graphics.h"
#include "utils/EventManager.h"

typedef struct window window_t;

/* platform initialization */
void platform_initialize(void);
void platform_terminate(void);

window_t *window_create(const char *title, int width, int height);
void window_query_size(window_t* window, int* width, int* height);
void window_destroy(window_t *window);
bool window_should_close(window_t *window);
void window_close(window_t* window);
void window_draw_buffer(window_t *window, framebuffer_t *buffer);
int window_get_id(window_t *window);


/* gui_related */
void *window_get_gui_context(window_t *window);

/* input related functions */
void input_poll_events(void);
int input_key_pressed(window_t *window, int key);
bool input_query_cursor(window_t *window, float *xpos, float *ypos);

/* misc platform functions */
float platform_get_time(void);

#endif  // RASTERIZER_PLATFORM_H_