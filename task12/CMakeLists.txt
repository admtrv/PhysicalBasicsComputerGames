# task12/cmake
add_executable(task12 main.cpp)

target_link_libraries(task12 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

