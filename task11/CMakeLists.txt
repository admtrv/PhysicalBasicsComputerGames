# task11/cmake
add_executable(task11 main.cpp)

target_link_libraries(task11 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

