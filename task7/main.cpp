/*
 * task7.cpp
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

const float dt = 0.025f;
const int   time_step = 25;
const float L_view = 200.0f;
const float face_size = 5.0f;
const float g = 9.8f;

const std::string file_name = "data.txt";

static float current_time = 0.0f;

const float d = 150.0f;
const float V = 40.0f;
const float alpha_deg = 45.0f;
const float alpha = alpha_deg * M_PI / 180.0f;

static float h = 0.0f;
static float t_collision = 0.0f;

float xPackage(float t)
{
    return d;
}

float yPackage(float t)
{
    float y = h - 0.5f * g * t * t;
    return (y < 0.0f ? 0.0f : y);
}

float xAmmo(float t)
{
    return V * std::cos(alpha) * t;
}

float yAmmo(float t)
{
    float y = V * std::sin(alpha) * t - 0.5f * g * t * t;
    return (y < 0.0f ? 0.0f : y);
}


void updatePositions(int value)
{
    current_time += dt;

    float xp = xPackage(current_time);
    float yp = yPackage(current_time);
    float xa = xAmmo(current_time);
    float ya = yAmmo(current_time);

    std::ofstream file(file_name, std::ios::app);
    if(file.is_open())
    {
        file << current_time << " "
                << xp << " " << yp << " "
                << xa << " " << ya << "\n";
    }
    file.close();

    float dx = xp - xa;
    float dy = yp - ya;
    float dist = dx * dx + dy * dy;

    if (dist < 1.0f)
    {
        std::cout << "Hit! Real collision time: " << current_time << std::endl;
        exit(0);
    }

    if(yp <= 0.0f && ya <= 0.0f)
    {
        std::cout << "Ground! Real time: " << current_time << std::endl;
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
    gluOrtho2D(-L_view, L_view, -L_view*ratio, L_view*ratio);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawSquare()
{
    glBegin(GL_QUADS);
        glVertex2f(-face_size/2.0f, -face_size/2.0f);
        glVertex2f( face_size/2.0f, -face_size/2.0f);
        glVertex2f( face_size/2.0f,  face_size/2.0f);
        glVertex2f(-face_size/2.0f,  face_size/2.0f);
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

    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex2f(-L_view, 0.0f);
        glVertex2f( L_view, 0.0f);
        glEnd();
    }

    {
        glPushMatrix();
        glTranslatef(xPackage(current_time), yPackage(current_time), 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawSquare();
        glPopMatrix();
    }

    {
        glPushMatrix();
        glTranslatef(xAmmo(current_time), yAmmo(current_time), 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawCircle();
        glPopMatrix();
    }

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    h = d * std::tan(alpha);
    t_collision = d / (V * std::cos(alpha));

    std::cout << "Calculated drone height " << h << std::endl;
    std::cout << "Calculated collision time " << t_collision << std::endl;

    std::ofstream file(file_name, std::ios::trunc);
    file.close();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Shooting Simulation");
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePositions, 0);

    glutMainLoop();

    return 0;
}
