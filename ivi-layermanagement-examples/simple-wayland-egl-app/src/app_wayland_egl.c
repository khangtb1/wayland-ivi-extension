#include "app_wayland_egl.h"

struct AppWaylandEgl gAppWaylandEgl;

static void registry_handle_remove(void *p_data, struct wl_registry *p_registry, uint32_t id)
{
  LOG_WARNING("registry_handle_remove trigger with id: %d\n", id);
}

static void registry_handle_global(void *p_data, struct wl_registry *p_registry,
  uint32_t id, const char *p_interface, uint32_t version)
{
  LOG_INFO("registry_handle_global trigger: name: %s, version: %d, id: %d\n", p_interface, version, id);

  if (0 == strcmp(p_interface, "wl_compositor")) {
    gAppWaylandEgl.p_compositor = wl_registry_bind(p_registry, id, &wl_compositor_interface, 1);
  }
  else if (0 == strcmp(p_interface, "wl_shell")) {
    gAppWaylandEgl.p_shell = wl_registry_bind(p_registry, id, &wl_shell_interface, 1);
  }
}

static const struct wl_registry_listener registry_listener = {
  registry_handle_global,
  registry_handle_remove
};

void DisconnectFromCompositor()
{
  LOG_INFO("DisconnectFromCompositor() trigged \n");

  //@Todo with window_list

  if (NULL != gAppWaylandEgl.p_shell)
    wl_shell_destroy(gAppWaylandEgl.p_shell);

  if (NULL != gAppWaylandEgl.p_compositor)
    wl_compositor_destroy(gAppWaylandEgl.p_compositor);

  if (NULL != gAppWaylandEgl.p_registry)
    wl_registry_destroy(gAppWaylandEgl.p_registry);

  if (NULL != gAppWaylandEgl.p_display)
    wl_display_disconnect(gAppWaylandEgl.p_display);
}

int ConnectToCompositor()
{
  LOG_INFO("ConnectToCompositor() trigged \n");

  wl_list_init(&gAppWaylandEgl.window_list);

  gAppWaylandEgl.p_display = wl_display_connect(NULL);
  if (!gAppWaylandEgl.p_display) {
    LOG_ERROR("wl_display_connect() failed\n");
    return -1;
  }

  gAppWaylandEgl.p_registry = wl_display_get_registry(gAppWaylandEgl.p_display);
  if (!gAppWaylandEgl.p_registry) {
    LOG_ERROR("wl_display_get_registry() failed\n");
    return -1;
  }

  if (wl_registry_add_listener(gAppWaylandEgl.p_registry, &registry_listener, NULL) != 0) {
    LOG_ERROR("wl_registry_add_listener() failed\n");
    return -1;
  }

  if (wl_display_dispatch(gAppWaylandEgl.p_display) < 0) {
    LOG_ERROR("wl_display_dispatch() failed\n");
    return -1;
  }

  if (!gAppWaylandEgl.p_compositor || !gAppWaylandEgl.p_shell) {
    LOG_ERROR("Can't bind wlglobals\n");
    return -1;
  }

  return 0;
}

int InitializeEgl()
{
  LOG_INFO("InitializeEgl() trigged \n");

  EGLint iMajor, iMinor, n;

  const EGLint argb_config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE,   1,
    EGL_GREEN_SIZE, 1,
    EGL_BLUE_SIZE,  1,
    EGL_ALPHA_SIZE, 1,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    EGL_NONE
  };

  const EGLint context_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };

  gAppWaylandEgl.egldisplay = eglGetDisplay(gAppWaylandEgl.p_display);
  if (!gAppWaylandEgl.egldisplay) {
    LOG_ERROR("eglGetDisplay() failed\n");
    return -1;
  }

  if (!eglInitialize(gAppWaylandEgl.egldisplay, &iMajor, &iMinor)) {
    LOG_ERROR("eglInitialize() failed\n");
    return -1;
  }

  if (!eglBindAPI(EGL_OPENGL_ES_API)) {
    LOG_ERROR("eglBindAPI() failed\n");
    return -1;
  }

  if (!eglChooseConfig(gAppWaylandEgl.egldisplay, argb_config_attribs, &gAppWaylandEgl.eglconfig, 1, &n)) {
    LOG_ERROR("eglChooseConfig() failed\n");
    return -1;
  }

  gAppWaylandEgl.eglcontext = eglCreateContext(gAppWaylandEgl.egldisplay, gAppWaylandEgl.eglconfig, EGL_NO_CONTEXT, context_attribs);
  if (NULL == gAppWaylandEgl.eglcontext) {
    LOG_ERROR("eglCreateContext() failed\n");
    return -1;
  }

  return 0;
}

void DeInitializeEgl()
{
  LOG_INFO("DeInitializeEgl() trigged \n");

  if(gAppWaylandEgl.egldisplay) {

    if (gAppWaylandEgl.eglcontext)
      eglDestroyContext(gAppWaylandEgl.egldisplay, gAppWaylandEgl.eglcontext);

    eglMakeCurrent(gAppWaylandEgl.egldisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(gAppWaylandEgl.egldisplay);
    eglReleaseThread();
  }
}
