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
};

extern struct AppWaylandEgl gAppWaylandEgl;

int ConnectToCompositor();
void DisconnectFromCompositor();

int InitializeEgl();
void DeInitializeEgl();

#endif
