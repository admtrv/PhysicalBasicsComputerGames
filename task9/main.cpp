/*
 * task9.cpp
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

const float dt = 0.025f;
const int time_step = 25;

const float r = 50.0f;
const float e1 = 2.0f;
const float e2 = 1.5f;
const int n = 2;

static float t1=0.0f, w1=0.0f, theta1=0.0f;
static float t2=0.0f, theta2=0.0f;
static float t3=0.0f, theta3=0.0f;
static float T1=0.0f, T2=0.0f, T3=0.0f;

static float current_time = 0.0f;

const std::string f = "data.txt";

static float camera_angle = 0.5f;
static float camera_speed = 0.0f;
static float cam_distance = 200.0f;

float getAngle(float t)
{
    float alpha1 = e1 / r;
    float alpha2 = e2 / r;

    // phase 1
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
        return (theta1 + theta2)
               + w1 * dt_in_phase3
               - 0.5f * alpha2 * dt_in_phase3 * dt_in_phase3;
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

float zPos(float t)
{
    float angle = getAngle(t);
    return r * std::sin(angle);
}

void updatePositions(int value)
{
    current_time += dt;

    float x = xPos(current_time);
    float z = zPos(current_time);
    float speed = getSpeed(current_time);

    static std::ofstream ofs(f, std::ios::app);
    if (ofs.is_open())
    {
        ofs << current_time << " " << x << " " << z << " " << speed << "\n";
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

    camera_angle += camera_speed * dt;

    glutPostRedisplay();
    glutTimerFunc(time_step, updatePositions, 0);
}

void handleResize(int w, int h)
{
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 1.0, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawAxes(float length = 100.f)
{
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);

      // x (red)
      glColor3f(1,0,0);
      glVertex3f(-length, 0, 0);
      glVertex3f( length, 0, 0);

      // y (green)
      glColor3f(0,1,0);
      glVertex3f(0, -length, 0);
      glVertex3f(0,  length, 0);

      // z (blue)
      glColor3f(0,0,1);
      glVertex3f(0, 0, -length);
      glVertex3f(0, 0,  length);
    glEnd();

    glColor3f(1,1,1);

    glEnable(GL_LIGHTING);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera
    float camX = cam_distance * std::cos(camera_angle);
    float camZ = cam_distance * std::sin(camera_angle);
    float camY = 80.0f;

    gluLookAt(camX, camY, camZ,
              0.0,  0.0,  0.0,
              0.0,  1.0,  0.0);

    // axes
    drawAxes(120.0f);

    // light
    static GLfloat lightPos[4]  = { 50,100,50, 1 };
    static GLfloat whiteCol[4]  = { 1,1,1, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  whiteCol);
    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteCol);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // static sphere
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidSphere(5.0, 20, 20);
    glPopMatrix();

    // moving sphere
    float x = xPos(current_time);
    float z = zPos(current_time);

    glPushMatrix();
      glTranslatef(x, 0.0f, z);
      glColor3f(0.0f, 1.0f, 0.0f);
      glutSolidSphere(5.0, 20, 20);
    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);

    glutSwapBuffers();
}

int main(int argc, char** argv)
{

    float alpha1 = e1 / r;
    float alpha2 = e2 / r;

    // phase 1
    theta1 = n * 2.0f * M_PI;
    t1 = std::sqrt(2.0f * theta1 / alpha1);
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

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Circular Motion Simulation");

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePositions, 0);

    glutMainLoop();

    return 0;
}
