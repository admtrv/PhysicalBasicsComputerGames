/*
 * task3.cpp
 */

#include <fstream>
#include <iosfwd>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

const float l_max = 150.0f;         // maximum coordinate range for projection
const float square_size = 20.0f;    // side length of square representing car
const int time_step = 25;           // time step for glut timer
const float dt = 0.025f;            // time increment for calculations

const std::string file_name = "data.txt";

const float scale_factor = 13.0f;

// bus
float current_t = 0.0f;
float current_v = 0.0f;
float current_x = -l_max;

float s1 = 10.0f;
float v1 = 70.0f;

float t_stop = 60.0f / 3600.0f;

float s2 = 12.0f;
float v2 = 60.0f;

const float t1 = s1 / v1;           // first stage
const float t2 = t1 + t_stop;       // second stage
const float t3 = t2 + (s2 / v2);    // third stage

// update position of cars
void updatePosition(int value)
{
    current_t += dt;

    if (current_t < t1 * scale_factor)
    {
        current_v = v1;
    }
    else if (current_t >= t1 * scale_factor && current_t < t2 * scale_factor)
    {
        current_v = 0.0f;
    }
    else if (current_t >= t2 * scale_factor && current_t < t3 * scale_factor)
    {
        current_v = v2;
    }
    else
    {
        float avg_speed = ((current_x + l_max) / (current_t));
        std::cout << "Actual average speed: " << avg_speed << std::endl;
        std::exit(0);
    }

    current_x += current_v * dt;

    std::ofstream file(file_name, std::ios::app);

    if (file.is_open()) {
        file << current_t / scale_factor << " "
            << (current_x + l_max) / scale_factor << " "
            << current_v << "\n";
    }

    file.close();

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
        glVertex2f(-(square_size/2.0f), -(square_size/2.0f));
        glVertex2f( (square_size/2.0f), -(square_size/2.0f));
        glVertex2f( (square_size/2.0f),  (square_size/2.0f));
        glVertex2f(-(square_size/2.0f),  (square_size/2.0f));
    glEnd();
}

// draws the scene
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // draw bus
    glPushMatrix();
        glTranslatef(current_x, 0.0f, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f); // red
        drawSquare();
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    float calculated_avg_speed = (s1 + s2) / t3;
    std::cout << "Calculated average speed: " << calculated_avg_speed << std::endl;

    std::ofstream file(file_name, std::ios::trunc);
    file.close();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutCreateWindow("Average Speed Simulation");

    // set callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePosition, 0);

    // main loop
    glutMainLoop();
    return 0;
}
