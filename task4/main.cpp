/*
 * task4.cpp
 */

#include <cmath>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

const float l_max = 150.0f;         // maximum coordinate range for projection
const float face_size = 20.0f;    // side length of square representing car
const int time_step = 25;           // time step for glut timer
const float dt = 0.025f;            // time increment for calculations

const std::string file_name = "data.txt";

float current_time = 0.0f;

// bus
float x1 = -l_max;
float v1 = 15.0f;

// car
float x2 = -l_max;
float v2 = 0.0f;
float a2 = 1.5f;

// update position of cars
void updatePosition(int value)
{
    current_time += dt;

    x1 += v1 * dt;

    v2 = v2 + a2 * dt;
    x2 += v2 * dt + 0.5f * a2 * dt * dt;

    std::ofstream file(file_name, std::ios::app);

    if (file.is_open()) {
        file << current_time << " "
            << x1 + l_max << " " << v1 << " "
            << x2 + l_max << " " << v2 << "\n";
    }

    file.close();

    if (x2 >= x1)
    {
        std::cout << "Real catch time: " << current_time << std::endl;
        exit(0);
    }

    glutPostRedisplay();
    glutTimerFunc(time_step, updatePosition, 0);
}

// handle window resizing
void handleResize(int width, int height)
{
    if (width == 0) width = 1;  // prevent division by zero

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float ratio = static_cast<float>(height) / static_cast<float>(width);
    gluOrtho2D(-l_max, l_max, -l_max * ratio, l_max * ratio);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// draw a square
void drawSquare()
{
    glBegin(GL_QUADS);
        glVertex2f(-(face_size/2.0f), -(face_size/2.0f));
        glVertex2f( (face_size/2.0f), -(face_size/2.0f));
        glVertex2f( (face_size/2.0f),  (face_size/2.0f));
        glVertex2f(-(face_size/2.0f),  (face_size/2.0f));
    glEnd();
}

// draws the scene
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // draw bus
    glPushMatrix();
        glTranslatef(x1, face_size, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f); // red
        drawSquare();
    glPopMatrix();

    // draw car
    glPushMatrix();
        glTranslatef(x2, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f); // green
        drawSquare();
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    float сatch_calc = (2.0f * v1) / a2;
    std::cout << "Calculated catch time: " << сatch_calc << std::endl;

    std::ofstream file(file_name, std::ios::trunc);
    file.close();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutCreateWindow("Catch Time Simulation");

    // set callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePosition, 0);

    // main loop
    glutMainLoop();
    return 0;
}
