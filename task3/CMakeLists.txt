# task3/cmake
add_executable(task3 main.cpp)

target_link_libraries(task3 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

