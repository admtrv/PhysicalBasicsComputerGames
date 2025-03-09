cmake_minimum_required(VERSION 3.30)
project(GamePhysics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_subdirectory(task1)
add_subdirectory(task2)
add_subdirectory(task3)
add_subdirectory(task4)
