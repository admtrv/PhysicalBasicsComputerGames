/*
 * task8.cpp
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

const float dt = 0.025f;
const int   time_step = 25;
const float L_view = 150.0f;
const float face_size = 5.0f;
static float current_time = 0.0f;

const float r = 50.0f;
const float e1 = 2.0f;
const float e2 = 1.5f;
const int   n = 2;

const std::string f = "data.txt";

static float t1 = 0.0f;
static float w1 = 0.0f;
static float theta1 = 0.0f;

static float t2 = 0.0f;
static float theta2 = 0.0f;

static float t3 = 0.0f;
static float theta3 = 0.0f;

static float T1 = 0.0f;
static float T2 = 0.0f;
static float T3 = 0.0f;

float getAngle(float t)
{
    // phase 1
    float alpha1 = e1 / r;
    float alpha2 = e2 / r;

    if (t <= T1)
    {
        return 0.5f * alpha1 * t * t;
    }
    // phase 2
    else if (t <= T2)
    {
        float dt_in_phase2 = t - T1;
        return theta1 + w1 * dt_in_phase2;
    }
    // phase 3
    else if (t <= T3)
    {
        float dt_in_phase3 = t - T2;
        return (theta1 + theta2) + w1 * dt_in_phase3 - 0.5f * alpha2 * dt_in_phase3 * dt_in_phase3;
    }

    return theta1 + theta2 + theta3;
}

float getSpeed(float t)
{
    float alpha1 = e1 / r;
    float alpha2 = e2 / r;

    if (t <= T1)
    {
        return alpha1 * t;
    }
    else if (t <= T2)
    {
        return w1;
    }
    else if (t <= T3)
    {
        float dt_in_phase3 = t - T2;
        float speed = w1 - alpha2 * dt_in_phase3;
        return (speed > 0.0f ? speed : 0.0f);
    }
    return 0.0f;
}

float xPos(float t)
{
    float angle = getAngle(t);
    return r * std::cos(angle);
}

float yPos(float t)
{
    float angle = getAngle(t);
    return r * std::sin(angle);
}

void updatePositions(int value)
{
    current_time += dt;

    float x = xPos(current_time);
    float y = yPos(current_time);
    float speed = getSpeed(current_time);

    static std::ofstream ofs(f, std::ios::app);
    if (ofs.is_open())
    {
        ofs << current_time << " " << x << " " << y << " " << speed << "\n";
    }

    if (current_time >= T3)
    {
        float final_speed = getSpeed(current_time);
        if (final_speed <= 0.0001f)
        {
            std::cout << "Current time " << current_time << std::endl;
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

void drawCircle(int segments = 30)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * M_PI * float(i) / float(segments);
        float x = face_size/2.0f * std::cos(angle);
        float y = face_size/2.0f * std::sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2f(-L_view, 0.0f);
        glVertex2f( L_view, 0.0f);
        glVertex2f(0.0f, -L_view);
        glVertex2f(0.0f,  L_view);
    glEnd();

    {
        glPushMatrix();
        glTranslatef( xPos(current_time), yPos(current_time), 0.0f );
        glColor3f(0.0f, 1.0f, 0.0f);
        drawCircle();
        glPopMatrix();
    }

    glutSwapBuffers();
}

int main(int argc, char** argv)
{

    float alpha1 = e1 / r;
    float alpha2 = e2 / r;

    // phase 1
    theta1 = n * 2.0f * M_PI;
    t1 = std::sqrt( 2.0f * theta1 / alpha1 );
    w1 = alpha1 * t1;

    // phase 2
    theta2 = 3.0f * 2.0f * M_PI;
    t2 = theta2 / w1;

    // phase 3
    t3 = w1 / alpha2;
    theta3 = w1 * t3 - 0.5f * alpha2 * t3 * t3;

    T1 = t1;
    T2 = t1 + t2;
    T3 = t1 + t2 + t3;

    std::cout << "Calculated total time " << T3 << std::endl;

    std::ofstream ofs(f, std::ios::trunc);
    ofs.close();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Circular Motion Simulation");
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePositions, 0);

    glutMainLoop();

    return 0;
}
