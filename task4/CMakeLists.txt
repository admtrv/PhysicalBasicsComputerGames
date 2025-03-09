# task4/cmake
add_executable(task4 main.cpp)

target_link_libraries(task4 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

