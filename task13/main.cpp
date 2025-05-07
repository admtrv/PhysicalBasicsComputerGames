/*
 * task13.cpp
 */

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

enum class Integrator {
    Euler,
    Midpoint
};

constexpr float g = 9.8f;
constexpr float m = 80.0f;
constexpr float C_d = 0.3f;         // коэффициент лобового сопротивления
constexpr float S = 1.0f;
constexpr float rho0 = 1.225f;      // плотность воздуха на уровне моря
constexpr float z0 = 2500.0f;

constexpr float M_air = 0.0289644f;     // кг/моль – молярная масса воздуха
constexpr float R_g = 8.314f;           // Дж/(моль·К) – универсальная газовая
constexpr float T0 = 288.0f;            // К – считаем температуру постоянной

constexpr float dt = 0.05f;
constexpr int time_step = 25;
constexpr float l_view = 3500.0f;
constexpr float face_size = 50.0f;

static float current_time = 0.0f;
static float current_z = z0;
static float current_v = 0.0f;

constexpr Integrator METHOD = Integrator::Midpoint;

inline float airDensity(float z)
{
    // ρ(z) = rho0 * exp( -M * g * x / (R * T))
    return rho0 * std::exp(-(M_air * g * z) / (R_g * T0));
}

inline float accel(float z, float v)
{
    // a = − g − (sign(v) * 1/2 C * rho0 * S * V^2) / m
    float drag = 0.5f * C_d * airDensity(z) * S * v * std::fabs(v);
    return -g - drag / m;
}

void stepEuler()
{
    float a = accel(current_z, current_v);

    current_v += a * dt;
    current_z += current_v * dt;
}

void stepMidpoint()
{
    float a1 = accel(current_z, current_v);
    float v_mid = current_v + 0.5f * a1 * dt;
    float z_mid = current_z + 0.5f * current_v * dt;

    float a2 = accel(z_mid, v_mid);

    current_v += a2 * dt;
    current_z += v_mid * dt;
}


void updatePositions(int value)
{
    current_time += dt;

    if (METHOD == Integrator::Euler)
        stepEuler();
    else
        stepMidpoint();

    if (current_z <= face_size * 0.5f) {
        std::cout << "Landing time "<< current_time << std::endl;
        std::exit(EXIT_SUCCESS);
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

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2f(-l_view, 0.0f);
        glVertex2f( l_view, 0.0f);
    glEnd();

    glPushMatrix();
    {
        glTranslatef(0.0f, current_z, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawSquare();
    }
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    std::cout << (METHOD == Integrator::Euler ? "Euler method" : "Midpoint method") << std::endl;

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
