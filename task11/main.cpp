/*
 * task11.cpp
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

const float dt = 0.025f;
const int   time_step = 25;
const float l_view = 150.0f;
const float face_size = 5.0f;
static float current_time = 0.0f;

const float R = 50.0f; // mountain radius
const float g = 9.81f;
const float v0 = 5.0f;

// for the circular motion
// position = ( R * cos(phi), R * sin(phi) )
// starts at pi/2 and decreases toward 0
static float phi = M_PI / 2.0f;
static float v_tang = v0;
static bool  onCircle = true;

static float t_detach  = 0.0f;
static float x_detach  = 0.0f;
static float y_detach  = 0.0f;
static float vx_detach = 0.0f;
static float vy_detach = 0.0f;

bool checkDetach(float phi)
{
    float detach_threshold = ((v0 * v0) / (g * R) + 2.0f) / 3.0f;
    return (std::sin(phi) <= detach_threshold);
}

float xPos(float t)
{
    if (onCircle)
        return R * std::cos(phi);

    float d = t - t_detach;
    return x_detach + vx_detach * d;
}

float yPos(float t)
{
    if (onCircle)
        return R * std::sin(phi);

    float d = t - t_detach;
    return y_detach + vy_detach * d - 0.5f * g * d * d;
}

float speed(float t)
{
    if (onCircle)
        return v_tang;

    float d = t - t_detach;
    float vx = vx_detach;
    float vy = vy_detach - g * d;
    return std::sqrt(vx * vx + vy * vy);
}

void updatePositions(int)
{
    current_time += dt;

    if (onCircle)
    {
        float a_tang = g * std::cos(phi);
        v_tang += a_tang * dt;
        float new_phi = phi - (v_tang / R) * dt;

        if (checkDetach(phi))
        {
            std::cout << "Detachment condition met at t = " << current_time << ", phi = " << phi << std::endl;
            onCircle = false;
            t_detach = current_time;
            x_detach = R * std::cos(phi);
            y_detach = R * std::sin(phi);
            vx_detach = v_tang * std::sin(phi);
            vy_detach = -v_tang * std::cos(phi);
        }
        else
        {
            phi = new_phi;
            if (phi <= 0.0f)
            {
                exit(0);
            }
        }
    }
    else
    {
        if (yPos(current_time) <= 0.0f)
        {
            exit(0);
        }
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
        glVertex2f(-face_size/2.f, -face_size/2.f);
        glVertex2f( face_size/2.f, -face_size/2.f);
        glVertex2f( face_size/2.f,  face_size/2.f);
        glVertex2f(-face_size/2.f,  face_size/2.f);
    glEnd();
}

void drawHalfCircle(int segments = 30)
{
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; i++)
    {
        float ang = (M_PI * float(i)) / float(segments);
        float x = R * std::cos(ang);
        float y = R * std::sin(ang);
        glVertex2f(x, y);
    }
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // coordinate axes
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2f(-l_view, 0.0f);
        glVertex2f( l_view, 0.0f);
        glVertex2f(0.0f, -l_view);
        glVertex2f(0.0f,  l_view);
    glEnd();

    // mountain
    glColor3f(0.7f, 0.7f, 0.7f);
    drawHalfCircle();

    glPushMatrix();
        float x = xPos(current_time);
        float y = yPos(current_time);
        glTranslatef(x, y, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawSquare();
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Mountain Fall Simulation");
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePositions, 0);

    glutMainLoop();

    return 0;
}
