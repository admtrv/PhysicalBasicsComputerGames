/*
 * task2.cpp
 */

#include <fstream>
#include <iosfwd>
#include <GL/gl.h>
#include <GL/glut.h>

const float l_max = 150.0f;         // maximum coordinate range for projection
const float face_size = 20.0f;    // side length of square representing car
const int time_step = 25;           // time step for glut timer
const float dt = 0.025f;            // time increment for calculations

const std::string file_name = "data.txt";

float current_time = 0.0f;

// first car
float x1 = -l_max;
float v1 = 15.0f;

// second car
float x2 = -l_max + 50;
float v2 = 10.0f;

// third car
float x3 = l_max;
float v3 = -20.0f;

// update position of cars
void updatePosition(int value)
{
    current_time += dt;

    x1 += v1 * dt;
    x2 += v2 * dt;
    x3 += v3 * dt;

    std::ofstream file(file_name, std::ios::app);

    if (file.is_open()) {
        file << current_time << " "
            << x1 + l_max << " " << v1 << " "
            << x2 + l_max << " " << v2 << " "
            << x3 + l_max << " " << v3 << "\n";
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

    // draw first car
    glPushMatrix();
        glTranslatef(x1, face_size, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f); // red
        drawSquare();
    glPopMatrix();

    // draw second car
    glPushMatrix();
        glTranslatef(x2, 0.0f, 0.0f);
        glColor3f(0.0f, 1.0f, 0.0f); // green
        drawSquare();
    glPopMatrix();

    // draw third car
    glPushMatrix();
    glTranslatef(x3, -face_size, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f); // blue
    drawSquare();
    glPopMatrix();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    std::ofstream file(file_name, std::ios::trunc);
    file.close();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutCreateWindow("Graph's Data Collecting");

    // set callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updatePosition, 0);

    // main loop
    glutMainLoop();
    return 0;
}
