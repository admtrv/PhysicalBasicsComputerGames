# task15/cmake
add_executable(task15 main.cpp)

target_link_libraries(task15 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

