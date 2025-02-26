# task2/cmake
add_executable(task2 main.cpp)

target_link_libraries(task2 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

