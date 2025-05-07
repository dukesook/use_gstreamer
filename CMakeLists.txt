cmake_minimum_required(VERSION 3.10)
project(gstreamer_hello)

set(CMAKE_CXX_STANDARD 17)

# Find GStreamer package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)

include_directories(${GST_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS})
add_definitions(${GST_CFLAGS_OTHER})

add_executable(hello main.cpp)
target_link_libraries(hello ${GST_LIBRARIES})
