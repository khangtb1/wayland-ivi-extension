#ifndef APP_EVENTS_H
#define APP_EVENTS_H
#include <wayland-client.h>

extern const struct wl_touch_listener touch_listener;
extern const struct wl_pointer_listener pointer_listener;
extern const struct wl_keyboard_listener keyboard_listener;
extern const struct wl_output_listener output_listener;
#endif