# task5/cmake
add_executable(task5 main.cpp)

target_link_libraries(task5 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

