#include "app_events.h"
#include "app_wayland_egl.h"

static void touch_handle_down(void *p_data, struct wl_touch *p_touch, uint32_t serial, uint32_t time, struct wl_surface *p_surface, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
  LOG_INFO("touch_handle_down trigged with id: %d\n", id);
}

static void touch_handle_up(void *p_data, struct wl_touch *p_touch, uint32_t serial, uint32_t time, int32_t id)
{
  LOG_INFO("touch_handle_up trigged with id: %d\n", id);
}

static void touch_handle_motion(void *p_data, struct wl_touch *p_touch, uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
  LOG_INFO("touch_handle_motion trigged with id: %d\n", id);
}

static void touch_handle_frame(void *p_data, struct wl_touch *p_touch)
{
  LOG_INFO("touch_handle_frame trigged\n");
}

static void touch_handle_cancel(void *p_data, struct wl_touch *p_touch)
{
  LOG_INFO("touch_handle_cancel trigged\n");
}

static void touch_handle_shape(void *p_data, struct wl_touch *p_touch, int32_t id, wl_fixed_t major, wl_fixed_t minor)
{
  LOG_INFO("touch_handle_shape trigged with id: %d\n", id);
}

static void touch_handle_orientation(void *p_data, struct wl_touch *p_touch, int32_t id, wl_fixed_t orientation)
{
  LOG_INFO("touch_handle_orientation trigged with id: %d\n", id);
}

const struct wl_touch_listener touch_listener = {
  touch_handle_down,
  touch_handle_up,
  touch_handle_motion,
  touch_handle_frame,
  touch_handle_cancel,
  touch_handle_shape,
  touch_handle_orientation
};

static void pointer_handle_enter(void* data, struct wl_pointer* wlPointer, uint32_t serial, struct wl_surface* wlSurface, wl_fixed_t sx, wl_fixed_t sy)
{
  LOG_INFO("pointer_handle_enter trigged\n");
}

static void pointer_handle_leave(void* data, struct wl_pointer* wlPointer, uint32_t serial, struct wl_surface* wlSurface)
{
  LOG_INFO("pointer_handle_Leave trigged\n");
}

static void pointer_handle_motion(void* data, struct wl_pointer* wlPointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
  LOG_INFO("pointer_handle_motion trigged\n");
}

static void pointer_handle_button(void* data, struct wl_pointer* wlPointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
  LOG_INFO("pointer_handle_button trigged\n");
}

static void pointer_handle_axis(void* data, struct wl_pointer* wlPointer, uint32_t time, uint32_t axis, wl_fixed_t value)
{
  LOG_INFO("pointer_handle_axis trigged\n");
}

static void pointer_handle_frame(void* data, struct wl_pointer* wlPointer)
{
  LOG_INFO("pointer_handle_frame trigged\n");
}

static void pointer_handle_axis_source(void* data, struct wl_pointer* wlPointer, uint32_t axisSource)
{
  LOG_INFO("pointer_handle_axis_source trigged\n");
}

static void pointer_handle_axis_stop(void* data, struct wl_pointer* wlPointer, uint32_t time, uint32_t axis)
{
  LOG_INFO("pointer_handle_axis_stop trigged\n");
}

static void pointer_handle_axis_discrete(void* data, struct wl_pointer* wlPointer, uint32_t axis, int32_t discrete)
{
  LOG_INFO("pointer_handle_axis_discrete trigged\n");
}

const struct wl_pointer_listener pointer_listener = {
  pointer_handle_enter,
  pointer_handle_leave,
  pointer_handle_motion,
  pointer_handle_button,
  pointer_handle_axis,
  pointer_handle_frame,
  pointer_handle_axis_source,
  pointer_handle_axis_stop,
  pointer_handle_axis_discrete
};

static void keyboard_handle_keymap(void* data, struct wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size)
{
  LOG_INFO("keyboard_handle_keymap trigged\n");
}

static void keyboard_handle_enter(void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys)
{
  LOG_INFO("keyboard_handle_enter trigged\n");
}

static void keyboard_handle_leave(void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface)
{
  LOG_INFO("keyboard_handle_leave trigged\n");
}

static void keyboard_handle_key(void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state_w)
{
  LOG_INFO("keyboard_handle_key trigged\n");
}

static void keyboard_handle_modifiers(void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
  LOG_INFO("keyboard_handle_modifiers trigged\n");
}

static void keyboard_handle_repeat_info(void* data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay)
{
  LOG_INFO("keyboard_handle_repeat_info trigged\n");
}

const struct wl_keyboard_listener keyboard_listener = {
  keyboard_handle_keymap,
  keyboard_handle_enter,
  keyboard_handle_leave,
  keyboard_handle_key,
  keyboard_handle_modifiers,
  keyboard_handle_repeat_info
};

static void output_listener_geometry(void *data, struct wl_output *output, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform)
{
  LOG_INFO("output_listener_geometry trigged\n");
}

static void output_listener_mode(void *data, struct wl_output *output, uint32_t flags, int32_t width, int32_t height, int32_t refresh)
{
  LOG_INFO("output_listener_mode trigged\n");
}

static void output_listener_done(void *data, struct wl_output *output)
{
  LOG_INFO("output_listener_done trigged\n");
}

static void output_listener_scale(void *data, struct wl_output *output, int32_t factor)
{
  LOG_INFO("output_listener_scale trigged\n");
}

static void output_listener_name(void *data, struct wl_output *wl_output, const char *name)
{
  LOG_INFO("output_listener_name trigged: %s\n", name);
}

static void output_listener_description(void *data, struct wl_output *wl_output, const char *description)
{
  LOG_INFO("output_listener_description trigged: %s\n", description);
}

const struct wl_output_listener output_listener = {
  output_listener_geometry,
  output_listener_mode,
  output_listener_done,
  output_listener_scale,
  output_listener_name,
  output_listener_description
};