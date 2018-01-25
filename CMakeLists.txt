cmake_minimum_required(VERSION 3.5.1)
project(v4l2)

include_directories(/usr/local/include)
#include_directories(/usr/include)

link_directories(/usr/local/lib)
link_directories(libs)
link_directories(/usr/lib/x86_64-linux-gnu)

set(SRC_FILES
  main.cpp V4L2Camera.cpp SimpleViewer.cpp
)
add_executable(v4l2 ${SRC_FILES})
target_link_libraries(v4l2 shift2depth pthread opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs GL GLU glut usb-1.0)