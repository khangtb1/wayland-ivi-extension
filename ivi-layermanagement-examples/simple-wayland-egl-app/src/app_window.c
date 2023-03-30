#include "app_window.h"
#include "app_wayland_egl.h"

static void surface_enter(void *p_data, struct wl_surface *p_surface, struct wl_output *p_output)
{
  LOG_INFO("surface_enter trigged %lX\n", (uintptr_t)p_data);
}

static void surface_leave(void *p_data, struct wl_surface *p_surface, struct wl_output *p_output)
{
  LOG_INFO("surface_leave trigged %lX\n", (uintptr_t)p_data);
}

static const struct wl_surface_listener surface_listener = {
  surface_enter,
  surface_leave
};

static void
shell_surface_handle_ping(void *p_data, struct wl_shell_surface *p_shell_surface, uint32_t serial)
{
  LOG_INFO("shell_surface_handle_ping trigged %lX\n", (uintptr_t)p_data);
}

static void shell_surface_handle_configure(void *p_data, struct wl_shell_surface *p_shell_surface, uint32_t edges, int32_t width, int32_t height)
{
  LOG_INFO("shell_surface_handle_configure trigged %lX\n", (uintptr_t)p_data);
}

static void shell_surface_handle_popup_done(void *p_data, struct wl_shell_surface *p_shell_surface)
{
  LOG_INFO("shell_surface_handle_popup_done trigged %lX\n", (uintptr_t)p_data);
}

static const struct wl_shell_surface_listener shell_surface_listener = {
  shell_surface_handle_ping,
  shell_surface_handle_configure,
  shell_surface_handle_popup_done
};

struct AppWindow *createWindow(uint32_t width, uint32_t height, const char *window_title)
{
  LOG_INFO("createWindow() trigged %d %d %s\n", width, height, window_title);

  struct AppWindow *lpWindow = calloc(1, sizeof(struct AppWindow));
  if (!lpWindow) {
    LOG_ERROR("Can't allocate new surface\n");
    goto error;
  }

  lpWindow->size.width = width;
  lpWindow->size.height = height;

  lpWindow->p_surface = wl_compositor_create_surface(gAppWaylandEgl.p_compositor);
  if (!lpWindow->p_surface) {
    LOG_ERROR("wl_compositor_create_surface() failed\n");
    goto error;
  }

  if (wl_surface_add_listener(lpWindow->p_surface, &surface_listener, lpWindow) !=0) {
    LOG_ERROR("wl_surface_add_listener() failed\n");
    goto error;
  }

  lpWindow->p_shell_surface = wl_shell_get_shell_surface(gAppWaylandEgl.p_shell, lpWindow->p_surface);
  if (!lpWindow->p_shell_surface) {
    LOG_ERROR("wl_shell_get_shell_surface() failed\n");
    goto error;
  }

  if (wl_shell_surface_add_listener(lpWindow->p_shell_surface , &shell_surface_listener, lpWindow) != 0) {
    LOG_ERROR("wl_shell_surface_add_listener() failed\n");
    goto error;
  }
  wl_shell_surface_set_title(lpWindow->p_shell_surface, window_title);

  lpWindow->p_egl_window = wl_egl_window_create(lpWindow->p_surface, lpWindow->size.width, lpWindow->size.height);
  if (!lpWindow->p_egl_window) {
    LOG_ERROR("wl_egl_window_create() failed\n");
    goto error;
  }

  lpWindow->eglsurface = eglCreateWindowSurface(gAppWaylandEgl.egldisplay, gAppWaylandEgl.eglconfig, lpWindow->p_egl_window, NULL);
  if (!lpWindow->eglsurface){
    LOG_ERROR("wl_egl_window_create() failed\n");
    goto error;
  }

  if (!eglMakeCurrent(gAppWaylandEgl.egldisplay, lpWindow->eglsurface, lpWindow->eglsurface, gAppWaylandEgl.eglcontext)) {
    LOG_ERROR("wl_egl_window_create() failed\n");
    goto error;
  }

  wl_list_insert(&gAppWaylandEgl.window_list, &lpWindow->link);

  return lpWindow;

error:
  removeWindow(lpWindow);
  return NULL;
}

void removeWindow(struct AppWindow *window)
{
  LOG_INFO("removeWindow() trigged %lX\n", (uintptr_t)window);

  if (window) {
    wl_list_remove(&window->link);
    free(window);
  }
}