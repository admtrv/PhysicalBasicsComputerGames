# task14/cmake
add_executable(task14 main.cpp)

target_link_libraries(task14 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

