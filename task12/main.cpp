/*
 * task12.cpp
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

const float g = 9.8f;
const float m = 80.0f;
const float C = 0.3f;       // Коэффициент лобового сопротивления
const float S = 1.0f;
const float rho = 1.225f;   // Плотность воздуха
const float z0 = 2500.0f;

const float dt = 0.05f;
const int   time_step = 25;
const float l_view = 3500.0f;
const float face_size = 50.0f;

static float current_time = 0.0f;

const float v00 = std::sqrt((2.0f * m * g) / (C * S * rho));

float z(float t)
{
    // z(t)
    float val = z0 - ( (v00 * v00) / g ) * std::log( std::cosh( (g * t) / v00 ) );
    return (val > 0.0f) ? val : 0.0f;
}

void updatePositions(int value)
{
    current_time += dt;

    float current_z = z(current_time);

    if (current_z <= face_size / 2.0f)
    {
        std::cout << "Real landing time "<< current_time << std::endl;
        exit(0);
    }

    glutPostRedisplay();
    glutTimerFunc(time_step, updatePositions, 0);
}


void handleResize(int w, int h)
{
    if (w == 0) w = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float ratio = static_cast<float>(h) / static_cast<float>(w);
    gluOrtho2D(-l_view, l_view, -l_view * ratio, l_view * ratio);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawSquare()
{
    glBegin(GL_QUADS);
        glVertex2f(-face_size / 2.f, -face_size / 2.f);
        glVertex2f( face_size / 2.f, -face_size / 2.f);
        glVertex2f( face_size / 2.f,  face_size / 2.f);
        glVertex2f(-face_size / 2.f,  face_size / 2.f);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Рисуем линию
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2f(-l_view, 0.0f);
        glVertex2f( l_view, 0.0f);
    glEnd();

    // Рисуем парашютиста
    glPushMatrix();
    {
        glTranslatef(0.0f, z(current_time), 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawSquare();
    }
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    double exponent = std::exp( (z0 * g) / (v00 * v00) );
    double t0 = (v00 / g) * std::acosh(exponent);

    std::cout << "Calculated landing time " << t0 << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Parachutist Simulation");
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePositions, 0);

    glutMainLoop();
    return 0;
}
