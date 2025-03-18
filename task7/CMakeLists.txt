# task7/cmake
add_executable(task7 main.cpp)

target_link_libraries(task7 PRIVATE OpenGL::GL OpenGL::GLU GLUT::GLUT)

