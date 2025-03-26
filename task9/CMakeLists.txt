# task9/cmake
add_executable(task9 main.cpp)

target_link_libraries(task9 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

