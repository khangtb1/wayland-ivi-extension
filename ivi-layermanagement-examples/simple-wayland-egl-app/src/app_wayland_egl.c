#include "app_wayland_egl.h"
#include "app_events.h"
#include <sys/signalfd.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>

struct AppWaylandEgl gAppWaylandEgl;

static int SetupSignalFd()
{
  LOG_INFO("SetupSignalFd trigged\n");

  sigset_t mask;
  if (sigemptyset(&mask) != 0) {
    LOG_ERROR("sigemptyset() failed\n");
    return -1;
  }

  if (sigaddset(&mask, SIGINT) != 0) {
    LOG_ERROR("sigaddset(SIGINT) failed\n");
    return -1;
  }

  if (sigaddset(&mask, SIGTERM) != 0) {
    LOG_ERROR("sigaddset(SIGTERM) failed\n");
    return -1;
  }

  if (sigprocmask(SIG_BLOCK, &mask, NULL) != 0) {
    LOG_ERROR("sigprocmask() failed\n");
    return -1;
  }

  gAppWaylandEgl.signal_fd = signalfd(-1, &mask, SFD_NONBLOCK | SFD_CLOEXEC);
  if (gAppWaylandEgl.signal_fd < 0) {
    LOG_ERROR("signalfd() failed\n");
    return -1;
  }

  return 0;
}

static void seat_capabilities(void *p_data, struct wl_seat *p_seat, uint32_t caps)
{
  LOG_INFO("seat_capabilities trigged with caps: %d\n", caps);

  struct AppSeat *lpSeat = (struct AppSeat*)p_data;

  if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !lpSeat->wltouch) {
    LOG_INFO("seat_capabilities do wl_seat_get_touch\n");
    lpSeat->wltouch = wl_seat_get_touch(p_seat);
    wl_touch_add_listener(lpSeat->wltouch, &touch_listener, lpSeat);
  }
  else if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && lpSeat->wltouch)
  {
    LOG_INFO("seat_capabilities do wl_touch_destroy\n");
    wl_touch_destroy(lpSeat->wltouch);
    lpSeat->wltouch = NULL;
  }

  if ((caps & WL_SEAT_CAPABILITY_POINTER) && !lpSeat->wlpointer) {
    LOG_INFO("seat_capabilities do wl_seat_get_pointer\n");
    lpSeat->wlpointer = wl_seat_get_pointer(p_seat);
    wl_pointer_add_listener(lpSeat->wlpointer, &pointer_listener, lpSeat);
  }
  else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && lpSeat->wlpointer)
  {
    LOG_INFO("seat_capabilities do wl_pointer_destroy\n");
    wl_pointer_destroy(lpSeat->wlpointer);
    lpSeat->wlpointer = NULL;
  }

  if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !lpSeat->wlkeyboard) {
    LOG_INFO("seat_capabilities do wl_seat_get_keyboard\n");
    lpSeat->wlkeyboard = wl_seat_get_keyboard(p_seat);
    wl_keyboard_add_listener(lpSeat->wlkeyboard, &keyboard_listener, lpSeat);
  }
  else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && lpSeat->wlkeyboard)
  {
    LOG_INFO("seat_capabilities do wl_keyboard_destroy\n");
    wl_keyboard_destroy(lpSeat->wlkeyboard);
    lpSeat->wlkeyboard = NULL;
  }
}

static const struct wl_seat_listener seat_listener = {
  seat_capabilities,
  NULL /* name: since version 2 */
};

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
  else if(0 == strcmp(p_interface, "wl_seat")) {
    struct AppSeat *lpSeat = calloc(1, sizeof(struct AppSeat));
    if (!lpSeat) {
      LOG_ERROR("Can't allocate for AppSeat id: %d\n", id);
      return;
    }
    else {
      LOG_INFO("wl_seat is created id: %d\n", id);
      lpSeat->wlseat = wl_registry_bind(p_registry, id, &wl_seat_interface, 1);
      lpSeat->id = id;
      wl_seat_add_listener(lpSeat->wlseat, &seat_listener, lpSeat);
      wl_list_insert(&gAppWaylandEgl.seat_list, &lpSeat->link);
    }
  }
  else if(0 == strcmp(p_interface, "wl_output")) {
    struct AppOutput *lpOutput = calloc(1, sizeof(struct AppOutput));
    if (!lpOutput) {
      LOG_ERROR("Can't allocate for AppOutput id: %d\n", id);
    }
    else {
      LOG_INFO("wl_output is created id: %d\n", id);
      lpOutput->wloutput = wl_registry_bind(p_registry, id, &wl_output_interface, 1);
      lpOutput->id = id;
      wl_output_add_listener(lpOutput->wloutput, &output_listener, lpOutput);
      wl_list_insert(&gAppWaylandEgl.output_list, &lpOutput->link);
    }
  }
}

static void registry_handle_remove(void *p_data, struct wl_registry *p_registry, uint32_t id)
{
  LOG_WARNING("registry_handle_remove trigger with id: %d\n", id);
  struct AppSeat *lpSeat = NULL;
  struct AppOutput *lpOutput = NULL;

  wl_list_for_each(lpSeat, &gAppWaylandEgl.seat_list, link) {
    if (lpSeat->id == id) {
      LOG_INFO("wl_seat is removed id: %d\n", id);
      wl_list_remove(&lpSeat->link);

      if (lpSeat->wltouch) {
        LOG_INFO("wl_touch_destroy\n");
        wl_touch_destroy(lpSeat->wltouch);
      }

      if (lpSeat->wlpointer) {
        LOG_INFO("wl_pointer_destroy\n");
        wl_pointer_destroy(lpSeat->wlpointer);
      }

      if (lpSeat->wlkeyboard) {
         LOG_INFO("wl_keyboard_destroy\n");
         wl_keyboard_destroy(lpSeat->wlkeyboard);
      }

      if (lpSeat->wlseat)
        wl_seat_destroy(lpSeat->wlseat);

      free(lpSeat);
      return;
    }
  }

  wl_list_for_each(lpOutput, &gAppWaylandEgl.output_list, link) {
    if (lpOutput->id == id) {
      LOG_INFO("wl_output is removed id: %d\n", id);
      wl_list_remove(&lpOutput->link);

      if (lpOutput->wloutput)
        wl_output_destroy(lpOutput->wloutput);

      free(lpOutput);
      return;
    }
  }
}

static const struct wl_registry_listener registry_listener = {
  registry_handle_global,
  registry_handle_remove
};

void DisconnectFromCompositor()
{
  LOG_INFO("DisconnectFromCompositor() trigged \n");

  struct AppSeat *lpSeat, *lpSeatNext;
  struct AppOutput *lpOutput, *lpOutputNext;

  /* destroy the wlseat list */
  wl_list_for_each_safe(lpSeat, lpSeatNext, &gAppWaylandEgl.seat_list, link) {
    LOG_INFO("wl_seat is removed id: %d\n", lpSeat->id);
    wl_list_remove(&lpSeat->link);

      if (lpSeat->wltouch) {
        LOG_INFO("DisconnectFromCompositor wl_touch_destroy\n");
        wl_touch_destroy(lpSeat->wltouch);
      }

      if (lpSeat->wlpointer) {
        LOG_INFO("DisconnectFromCompositor wl_pointer_destroy\n");
        wl_pointer_destroy(lpSeat->wlpointer);
      }

      if (lpSeat->wlkeyboard) {
         LOG_INFO("DisconnectFromCompositor wl_keyboard_destroy\n");
         wl_keyboard_destroy(lpSeat->wlkeyboard);
      }

    if (lpSeat->wlseat)
      wl_seat_destroy(lpSeat->wlseat);

    free(lpSeat);
  }

  /* destroy the wloutput list */
  wl_list_for_each_safe(lpOutput, lpOutputNext, &gAppWaylandEgl.output_list, link) {
    LOG_INFO("wl_output is removed id: %d\n", lpOutput->id);
    wl_list_remove(&lpOutput->link);

    if (lpOutput->wloutput)
      wl_output_destroy(lpOutput->wloutput);

    free(lpOutput);
  }

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
  wl_list_init(&gAppWaylandEgl.seat_list);
  wl_list_init(&gAppWaylandEgl.output_list);

  if (SetupSignalFd() != 0) {
    return -1;
  }

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

static void AppPoll(short int event)
{
  LOG_INFO("AppPoll() trigged \n");
  int lret;
  struct pollfd pfd[2];
  pfd[0].fd = wl_display_get_fd(gAppWaylandEgl.p_display);
  pfd[0].events = event;
  pfd[1].fd = gAppWaylandEgl.signal_fd;
  pfd[1].events = POLLIN;

  do {
    lret = poll(pfd, 2, -1);
    if (lret > 0 && pfd[1].revents) {
      LOG_INFO("Terminate signal trigged\n");
      gAppWaylandEgl.is_running = false;
      break;
    }
  } while (lret == -1 && errno == EINTR);
}

int AppDispatcher()
{
  LOG_INFO("AppDispatcher() trigged \n");
  gAppWaylandEgl.is_running = true;

  while(gAppWaylandEgl.is_running) {
    int ret;

    if (wl_display_prepare_read(gAppWaylandEgl.p_display) == -1) {
      if (wl_display_dispatch_pending(gAppWaylandEgl.p_display) != 0)
      {
        LOG_ERROR("wl_display_dispatch_pending() failed\n");
        continue;
      }
    }

    while (gAppWaylandEgl.is_running) {
      ret = wl_display_flush(gAppWaylandEgl.p_display);
      if (ret != -1 || errno != EAGAIN)
        break;

      AppPoll(POLLOUT);
    }

    /* Don't stop if flushing hits an EPIPE; continue so we can read any
     * protocol error that may have triggered it. */
    if (ret < 0 && errno != EPIPE) {
      wl_display_cancel_read(gAppWaylandEgl.p_display);
      LOG_ERROR("wl_display_flush() failed with %s\n", strerror(errno));
      return -1;
    }

    AppPoll(POLLIN);

    if (wl_display_read_events(gAppWaylandEgl.p_display) == -1) {
      LOG_ERROR("wl_display_read_events() failed\n");
      return -1;
    }

    if (wl_display_dispatch_pending(gAppWaylandEgl.p_display) == -1) {
      LOG_ERROR("wl_display_dispatch_pending() failed\n");
      continue;
    }
  }
  return 0;
}
