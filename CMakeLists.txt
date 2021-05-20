# REQUIRED: ADD to the System PATH: <projectPath>\OpenCV_lib\x64\mingw\bin
#           <projectPath> is the path to the base project folder

cmake_minimum_required(VERSION 3.17)
project(TP2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}\\OpenCV_lib")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(OpenCV REQUIRED)
include_directories("${OpenCV_INCLUDE_DIRS}")

add_executable(TP2 main.cpp vc.h vc.c)

set(OpenCV_LIBS opencv_core opencv_highgui opencv_videoio)
target_link_libraries(TP2 ${OpenCV_LIBS})
