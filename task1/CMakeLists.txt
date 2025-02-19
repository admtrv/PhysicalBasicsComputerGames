# task1/cmake
add_executable(task1 main.cpp)

target_link_libraries(task1 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

