# task13/cmake
add_executable(task13 main.cpp)

target_link_libraries(task13 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

