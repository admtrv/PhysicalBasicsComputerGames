# task6/cmake
add_executable(task6 main.cpp)

target_link_libraries(task6 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

