/*
 * task5.cpp
 */

#include <cmath>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

const float l_max = 150.0f;
const float square_size = 20.0f;
const int time_step = 25;
const float dt = 0.025f;

const std::string file_name = "data.txt";

float current_time = 0.0f;

float x0 = -l_max;
float length = 250.0f;
float v1 = 100.0f;
float v2 = 20.0f;

float a = 0.0f;
float current_v = v1;
float current_x = 0.0f;

bool reached_v2 = false;

void updatePosition(int value)
{
    current_time += dt;

    current_v += a * dt;
    float old_x = x0;
    x0 += current_v * dt;
    current_x += (x0 - old_x);

    std::ofstream file(file_name, std::ios::app);
    if (file.is_open())
    {
        file << current_time << " "
             << (x0 + l_max) << " "
             << current_v << "\n";
    }
    file.close();

    if (current_v <= v2 && !reached_v2)
    {
        std::cout << "Real braking time from v1 to v2: " << current_time << std::endl;
        reached_v2 = true;
    }

    if (current_v <= 0.0f)
    {
        std::cout << "Real braking time from v1 to full stop: " << current_time << std::endl;
        exit(0);
    }

    glutPostRedisplay();
    glutTimerFunc(time_step, updatePosition, 0);
}

void handleResize(int width, int height)
{
    if (width == 0) width = 1;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float ratio = static_cast<float>(height) / static_cast<float>(width);
    gluOrtho2D(-l_max, l_max, -l_max * ratio, l_max * ratio);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawSquare()
{
    glBegin(GL_QUADS);
        glVertex2f(-(square_size/2.0f), -(square_size/2.0f));
        glVertex2f( (square_size/2.0f), -(square_size/2.0f));
        glVertex2f( (square_size/2.0f),  (square_size/2.0f));
        glVertex2f(-(square_size/2.0f),  (square_size/2.0f));
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
        glTranslatef(x0, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawSquare();
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    a = (v2*v2 - v1*v1) / (2.0f * length);
    float t = (v2 - v1) / a;
    float t_stop = - v1 / a;
    float x_stop = - (v1*v1) / (2.0f * a);

    std::cout << "Calculated deceleration a: " << a << std::endl;
    std::cout << "Calculated braking time from v1 to v2: " << t << std::endl;
    std::cout << "Calculated braking time from v1 to full stop: " << t_stop << std::endl;
    std::cout << "Calculated distance from v1 to full stop: " << x_stop << std::endl;

    std::ofstream file(file_name, std::ios::trunc);
    file.close();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutCreateWindow("Braking Simulation");

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePosition, 0);

    glutMainLoop();
    return 0;
}
