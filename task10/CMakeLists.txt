# task10/cmake
add_executable(task10 main.cpp)

target_link_libraries(task10 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

