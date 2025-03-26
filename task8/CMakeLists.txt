# task8/cmake
add_executable(task8 main.cpp)

target_link_libraries(task8 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

