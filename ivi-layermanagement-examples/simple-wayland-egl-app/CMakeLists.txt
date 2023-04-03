############################################################################
#
# \copyright (c) 2012, 2013 Advanced Driver Information Technology.
# This code is developed by Advanced Driver Information Technology.
# Copyright of Advanced Driver Information Technology, Bosch, and DENSO.
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
############################################################################
cmake_minimum_required (VERSION 2.6)
################################################################

project (simple-wayland-egl-app)

find_package(PkgConfig)
pkg_check_modules(GLESv2 glesv2 REQUIRED)
pkg_check_modules(EGL egl REQUIRED)
pkg_check_modules(WAYLAND_EGL wayland-egl REQUIRED)
pkg_check_modules(WAYLAND_CLIENT wayland-client REQUIRED)

include_directories(
    include
    ${GLESv2_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${WAYLAND_CLIENT_INCLUDE_DIRS}
)

link_directories(
    ${GLESv2_LIBRARY_DIRS}
    ${EGL_LIBRARY_DIRS}
    ${WAYLAND_CLIENT_LIBRARY_DIRS}
)

SET(LIBS
    ${GLESv2_LIBRARIES}
    ${WAYLAND_CLIENT_LIBRARIES}
    ${WAYLAND_EGL_LIBRARIES}
    ${EGL_LIBRARIES}
)
add_executable(${PROJECT_NAME}
    src/main.c
    src/app_wayland_egl.c
    src/app_window.c
    src/app_events.c
)

SET(GCC_SANITIZER_COMPILE_FLAGS "-fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector-all")
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_SANITIZER_COMPILE_FLAGS}" )
SET( CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -static-libasan -static-libubsan" )

add_dependencies(${PROJECT_NAME} ${LIBS})
add_definitions(${EGL_CFLAGS})
target_link_libraries(${PROJECT_NAME} ${LIBS})
install (TARGETS ${PROJECT_NAME} DESTINATION bin)
