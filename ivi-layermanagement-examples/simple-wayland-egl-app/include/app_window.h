#ifndef APP_WINDOW_H
#define APP_WINDOW_H
#include <wayland-egl.h>
#include <EGL/egl.h>
#include "utils.h"

struct AppWindow
{
  struct wl_surface       *p_surface;
  struct wl_shell_surface *p_shell_surface;
  struct wl_callback      *p_frame_cb;
  struct wl_egl_window    *p_egl_window;
  EGLSurface               eglsurface;
  struct wl_list           link;

  struct {
    uint32_t width;
    uint32_t height;
  } size;

};

struct AppWindow *createWindow(uint32_t width, uint32_t height, const char *window_title);
void removeWindow(struct AppWindow *surface);
void removeAllWindows();

#endif