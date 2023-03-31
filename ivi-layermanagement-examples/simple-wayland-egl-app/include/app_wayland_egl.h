#ifndef APP_WAYLAND_EGL_H
#define APP_WAYLAND_EGL_H
#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include "utils.h"

struct AppWaylandEgl
{
  struct wl_display    *p_display;
  struct wl_registry   *p_registry;
  struct wl_compositor *p_compositor;
  struct wl_shell      *p_shell;

  EGLDisplay            egldisplay;
  EGLConfig             eglconfig;
  EGLContext            eglcontext;

  struct wl_list        window_list;
  struct wl_list        seat_list;
  struct wl_list        output_list;

  bool                  is_running;
  int                   signal_fd;
};

struct AppOutput
{
  struct wl_output *wloutput;
  uint32_t id;
  struct wl_list link;
};

struct AppSeat
{
  struct wl_seat *wlseat;
  struct wl_touch *wltouch;
  uint32_t id;
  struct wl_list link;
};

extern struct AppWaylandEgl gAppWaylandEgl;

int ConnectToCompositor();
void DisconnectFromCompositor();

int InitializeEgl();
void DeInitializeEgl();

int AppDispatcher();

#endif
