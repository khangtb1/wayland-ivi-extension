#include "app_wayland_egl.h"
#include "app_window.h"

static int RunningApp()
{
  if (!createWindow(1920, 1080, "app_wayland_egl")) {
    return -1;
  }

  struct AppWindow *appWindowElem, *appWindowElemNext;
  wl_list_for_each_safe(appWindowElem, appWindowElemNext, &gAppWaylandEgl.window_list, link) {
    removeWindow(appWindowElem);
  }

  return 0;
}

int main(int argc, char **argv)
{
  int ret = 0;

  if (ConnectToCompositor() != 0){
    DisconnectFromCompositor();
    return -1;
  }

  if (InitializeEgl() != 0) {
    DeInitializeEgl();
    return -1;
  }

  ret = RunningApp();

  DeInitializeEgl();
  DisconnectFromCompositor();

  return ret;
}