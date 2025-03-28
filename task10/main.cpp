/*
 * task10.cpp
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

// face structure
struct Face
{
    int count;
    float nx, ny, nz;
    int idx[4];
};

// camera parameters
float camera_angle = 0.0f;
float camera_speed = 0.3f;
float cam_distance = 150.0f;

// time parameters
const float dt = 0.025f;
const int time_step = 25;

// file parameters
const std::string f = "data.txt";

// entry parameters
const float alpha_deg = 15.0f;
const float alpha = alpha_deg * M_PI / 180.0f;

const float m = 2.0f;
const float F_pull = 10.0f;
const float g = 9.8f;
const float mu = 0.2f;

// calculated parameters
float F_g = 0.0f;
float N = 0.0f;
float F_fr = 0.0f;
float F_net = 0.0f;
float friction_direction = 0.0f;

float F = 0.0f;
float a = 0.0f;
float v = 0.0f;
float t = 0.0f;

// wedge parameters
const float wedge_x = 100.0f;
const float wedge_y = wedge_x * std::tan(alpha);
const float wedge_z  = 40.0f;
const float wedge_length = wedge_x / cos(alpha);

// object parameters
const float cube_x = 20.0f;
const float cube_y = 10.0f;
const float cube_z = 10.0f;

float x = wedge_x / 2.0f;
float y = wedge_y / 2.0f;
float z = wedge_z / 2.0f;

float x_local = wedge_length / 2.0f;
float y_local = cube_y / 2.0f;
float z_local = 0.0f;

const float precision = 1.0f;

void updateMotion()
{
    t += dt;
    v += a * dt;

    // local coordinates
    x_local += v * dt;

    if (x_local >= wedge_length - (cube_x / 2.0f) - precision)
    {
        exit(0);
    }

    if (x_local <= (cube_x / 2.0f) - precision)
    {
        exit(0);
    }

    // global coordinates
    x = x_local * cos(alpha);
    y = x_local * sin(alpha);
}

void updateScene(int value)
{
    camera_angle += camera_speed * dt;

    updateMotion();

    static std::ofstream ofs(f, std::ios::app);
    if (ofs.is_open())
    {
        ofs << t << " " << x_local << " " << v << " " << x << " " << y << "\n";
    }

    glutPostRedisplay();
    glutTimerFunc(time_step, updateScene, 0);
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

void drawGlobalAxes(float length = 100.0f)
{
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);
        // x
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-length, 0.0f, 0.0f);
        glVertex3f( length, 0.0f, 0.0f);

        // y
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -length, 0.0f);
        glVertex3f(0.0f,  length, 0.0f);

        // z
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -length);
        glVertex3f(0.0f, 0.0f,  length);
    glEnd();

    glColor3f(1,1,1);
    glEnable(GL_LIGHTING);
}

void drawLocalAxes(float length = 50.0f)
{
    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);
        // x'
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(length, 0.0f, 0.0f);

        // y'
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, length, 0.0f);

        // z'
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, length);
    glEnd();

    glColor3f(1,1,1);
    glEnable(GL_LIGHTING);
}

void drawFloorGrid(float size = 70.0f, float step = 10.0f)
{
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.0f);

    glBegin(GL_LINES);
    for (float i = -size; i <= size; i += step)
    {
        glVertex3f(-size, 0.0f, i);
        glVertex3f( size, 0.0f, i);

        glVertex3f(i, 0.0f, -size);
        glVertex3f(i, 0.0f,  size);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

void drawPlane(float size_x, float size_y, float size_z)
{
    float v[6][3] =
    {
        {0, 0, 0},
        {size_x, 0, 0},
        {size_x, size_y, 0},
        {0, 0, size_z},
        {size_x, 0, size_z},
        {size_x, size_y, size_z}
    };

    Face faces[] =
    {
        {3, 0.0f, 0.0f, -1.0f, {0, 1, 2, -1}},
        {3, 0.0f, 0.0f, 1.0f, {3, 4, 5, -1}},
        {4, 0.0f, -1.0f, 0.0f, {0, 1, 4, 3}},
        {4, 1.0f, 0.0f, 0.0f, {1, 2, 5, 4}},
        {4, 0.0f, 1.0f, 0.0f, {0, 2, 5, 3}}
    };

    glColor3f(0.7f, 0.7f, 0.7f);

    for (auto & face : faces)
    {
        if (face.count == 3)
            glBegin(GL_TRIANGLES);
        else
            glBegin(GL_QUADS);

        glNormal3f(face.nx, face.ny, face.nz);

        for (int i = 0; i < face.count; i++)
        {
            int idx = face.idx[i];
            glVertex3f(v[idx][0], v[idx][1], v[idx][2]);
        }

        glEnd();
    }
}

void drawVector(float x, float y, float z, float dx, float dy, float dz)
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(x, y, z);
    glVertex3f(x + dx, y + dy, z + dz);
    glEnd();
}

void drawParallelepiped(float angle, float x, float y, float z, float size_x, float size_y, float size_z)
{
    glTranslatef(x , y , z);
    glTranslatef(0.0f, size_y * 0.5f, 0.0f); // to correctly stand on surface
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glColor3f(0.3f, 0.6f, 0.9f);

    glPushMatrix();
        glScalef(size_x, size_y, size_z);
        glutSolidCube(1.0);
    glPopMatrix();

    float scale = 3.0f;

    // F pull
    drawVector(0.0f, 0.0f, 0.0f, F_pull * scale, 0.0f, 0.0f);

    // F friction
    drawVector(friction_direction * size_x / 2.0f, -size_y / 2.0f + precision, 0.0f, friction_direction * F_fr * scale, 0.0f, 0.0f);

    // N
    drawVector(0.0f, 0.0f, 0.0f, 0.0f, N * scale, 0.0f);
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

    // drawGlobalAxes(1000.0f);

    // light
    static GLfloat light_pos[4]  = { 50, 100, 50, 1};
    static GLfloat white_col[4]  = { 1, 1, 1, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  white_col);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_col);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // floor grid
    glPushMatrix();
        drawFloorGrid();
    glPopMatrix();

    // plane axes
    glPushMatrix();
        glTranslatef(-wedge_x * 0.5f, 0.0f, -wedge_z * 0.5f);
        glTranslatef(wedge_x / 2.0f, wedge_y / 2.0f, wedge_z / 2.0f);
        glRotatef(alpha_deg, 0.0f, 0.0f, 1.0f);
        drawLocalAxes(80.0f);
    glPopMatrix();

    // plane
    glPushMatrix();
        glTranslatef(-wedge_x * 0.5f, 0.0f, -wedge_z * 0.5f);
        drawPlane(wedge_x, wedge_y, wedge_z);
    glPopMatrix();

    // parallelepiped
    glPushMatrix();
        glTranslatef(-wedge_x * 0.5f, 0.0f, -wedge_z * 0.5f);
        drawParallelepiped(alpha_deg, x, y, z, cube_x, cube_y, cube_z);
    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    F_g = m * g * sin(alpha);
    N = m * g * cos(alpha);
    F_fr = mu * N;

    F_net = F_pull - F_g;

    if (std::abs(F_net) < F_fr)
    {
        F = 0.0f;
        a = 0.0f;
    }
    else
    {
        friction_direction = (F_net > 0) ? -1.0f : 1.0f;
        F = F_net + friction_direction * F_fr;
        a = F / m;
    }

    std::cout << "Calculated resulting force F = " << F << " N" << std::endl;
    std::cout << "Calculated acceleration a = " << a << " m/s^2" << std::endl;

    std::ofstream ofs(f, std::ios::trunc);
    ofs.close();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Parallelepiped Simulation");

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, updateScene, 0);

    glutMainLoop();

    return 0;
}
