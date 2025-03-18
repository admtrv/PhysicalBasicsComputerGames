/*
 * task6.cpp
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

const float dt = 0.025f;
const int   time_step = 25;
const float L_view = 150.0f;
const float face_size = 10.0f;

const float g = 9.8f;
const float h = 80.0f;

static float current_time = 0.0f;

const float v1_up = 20.0f;
static float t1 = 0.0f;

const float v2_down = 0.0f;
static float t2  = 0.0f;

float y1(float t)
{
    float y = h + v1_up * t - 0.5f * g * t * t;
    return (y > 0.0f) ? y : 0.0f;
}

float y2(float t)
{
    if (t < t2)
    {
        return h;
    }
    else
    {
        float delta = t - t2;
        float y = h - v2_down * delta - 0.5f * g * delta * delta;
        return (y > 0.0f) ? y : 0.0f;
    }
}

void updatePositions(int value)
{
    current_time += dt;

    float y_1 = y1(current_time);
    float y_2 = y2(current_time);

    if (y_1 <= face_size/2.0f)
    {
        std::cout << "Real landing time of first object: " << current_time << std::endl;
    }

    if (y_2 <= face_size/2.0f)
    {
        std::cout << "Real landing time of second object: " << current_time << std::endl;
    }

    if (y_1 <= face_size/2.0f && y_2 <= face_size/2.0f)
    {
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
    gluOrtho2D(-L_view, L_view, -L_view * ratio, L_view * ratio);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawSquare()
{
    glBegin(GL_QUADS);
        glVertex2f(-face_size/2.f, -face_size/2.f);
        glVertex2f( face_size/2.f, -face_size/2.f);
        glVertex2f( face_size/2.f,  face_size/2.f);
        glVertex2f(-face_size/2.f,  face_size/2.f);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(-L_view, 0.0f);
    glVertex2f(L_view, 0.0f);
    glEnd();

    glPushMatrix();
    {
        glTranslatef(-face_size, y1(current_time), 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawSquare();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(face_size, y2(current_time), 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawSquare();
    }
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    t1 = ( v1_up + std::sqrt(v1_up * v1_up + 2.0f * g * (h - face_size/2.0f) ) )/ g;
    std::cout << "Time for the first object to land: " << t1 << std::endl;

    float delta = ( -v2_down + std::sqrt(v2_down * v2_down + 2.0f * g * (h - face_size/2.0f) ) ) / g;
    t2 = t1 - delta;
    std::cout << "Time to throw second object: " << t2 << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Free Fall Simulation");
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePositions, 0);

    glutMainLoop();
    return 0;
}
