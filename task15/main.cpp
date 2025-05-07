/*
 * task15.cpp
 */

#include <cmath>
#include <cstdio>
#include <GL/gl.h>
#include <GL/glut.h>

const float dt = 0.01f;
const int time_step = 10;
const float l_view = 120.0f;

// red
const float r1 = 10.0f;
const float m1 = 1.0f;

// blue
const float r2 = 10.0f;
const float m2 = 1.5f;

struct Vec2 {
    float x, y;

    Vec2 operator+(const Vec2& o) const {
        return {x + o.x, y + o.y};
    }

    Vec2 operator-(const Vec2& o) const {
        return {x - o.x, y - o.y};
    }

    Vec2 operator*(float s) const {
        return {x * s, y * s};
    }

    Vec2& operator+=(const Vec2& o) {
        x += o.x;
        y += o.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& o) {
        x -= o.x;
        y -= o.y;
        return *this;
    }

};

inline float dot(const Vec2& a, const Vec2& b) {
    return a.x*b.x + a.y*b.y;
}

// red
static Vec2 p1{  45.0f, -50.0f};    // start position
static Vec2 v1{-15.0f,  18.0f};     // start speed

// blue
static Vec2 p2{ -45.0f,  55.0f};
static Vec2 v2{ 17.0f, -16.0f};

// walls
static float x_min, x_max, y_min, y_max;

void collisionBall()
{
    Vec2 r12 = p1 - p2;
    float dist = dot(r12, r12);
    float min_d = r1 + r2;

    if (dist > min_d * min_d)
        return;                 // far away

    // p12 = m1 * v1 − m2 * v2
    Vec2 p12{ m1 * v1.x - m2 * v2.x,m1 * v1.y - m2 * v2.y };

    // scalar = r12x^2 + r12y^2
    float S = r12.x * p12.x + r12.y * p12.y;

    // impulses
    float Ix = -r12.x / dist * S;
    float Iy = -r12.y / dist * S;

    // speeds
    v1.x += Ix / m1;
    v1.y += Iy / m1;

    v2.x -= Ix / m2;
    v2.y -= Iy / m2;
}

void collisionWall(const float r, Vec2& p, Vec2& v)
{
    if (p.x - r < x_min)
        { p.x = x_min + r; v.x = -v.x; }
    else if (p.x + r > x_max)
        { p.x = x_max - r; v.x = -v.x; }

    if (p.y - r < y_min)
        { p.y = y_min + r; v.y = -v.y; }
    else if (p.y + r > y_max)
        { p.y = y_max - r; v.y = -v.y; }
}

inline float timeToWall(const Vec2& p, const Vec2& v, float r, float minB, float maxB)
{
    const float INF = std::numeric_limits<float>::infinity();
    float tx = INF;
    float ty = INF;

    if (v.x > 0.0f)
        tx = (maxB - r - p.x) / v.x;
    else if (v.x < 0.0f)
        tx = (minB + r - p.x) / v.x;

    if (v.y > 0.0f)
        ty = (maxB - r - p.y) / v.y;
    else if (v.y < 0.0f)
        ty = (minB + r - p.y) / v.y;

    return std::fmax(0.0f, std::fmin(tx, ty));
}

inline float timeToBall(const Vec2& pA, const Vec2& vA, float rA, const Vec2& pB, const Vec2& vB, float rB)
{
    Vec2 dp = pA - pB;
    Vec2 dv = vA - vB;

    float a = dot(dv, dv);

    float b = 2.0f * dot(dp, dv);

    float c = dot(dp, dp) - (rA + rB)*(rA + rB);

    float disc = b*b - 4.0f*a*c;
    if (disc < 0.0f)
        return std::numeric_limits<float>::infinity();

    float t = (-b - std::sqrt(disc)) / (2.0f*a);
    return (t > 0.0f) ? t : std::numeric_limits<float>::infinity();
}

inline void renderBitmapString(float x, float y, const char* text)
{
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
}

void update(int)
{
    p1 += v1 * dt;
    p2 += v2 * dt;

    collisionBall();
    collisionWall(r1, p1, v1);
    collisionWall(r2, p2, v2);

    glutPostRedisplay();
    glutTimerFunc(time_step, update, 0);
}

void handleResize(int w, int h)
{
    if (!w) w = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float ratio = static_cast<float>(h) / w;
    gluOrtho2D(-l_view, l_view, -l_view * ratio, l_view * ratio);

    x_min = -l_view;
    x_max =  l_view;
    y_min = -l_view * ratio;
    y_max =  l_view * ratio;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawCircle(float r,int seg = 40)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);

    for (int i = 0; i <= seg; ++i) {
        float a = 2.0f * M_PI * i / seg;
        glVertex2f(r * std::cos(a), r * std::sin(a));
    }

    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // red
    glPushMatrix();
        glTranslatef(p1.x, p1.y, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawCircle(r1);
    glPopMatrix();

    // blue
    glPushMatrix();
        glTranslatef(p2.x, p2.y, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        drawCircle(r2);
    glPopMatrix();

    // timers
    float t_wall1 = timeToWall(p1, v1, r1, x_min, x_max);
    float t_wall2 = timeToWall(p2, v2, r2, x_min, x_max);
    float t_bb = timeToBall(p1, v1, r1, p2, v2, r2);

    float t1 = std::fmin(t_wall1, t_bb);
    float t2 = std::fmin(t_wall2, t_bb);

    char buf[64];
    glColor3f(1.0f, 1.0f, 1.0f);

    std::snprintf(buf, sizeof(buf), "1: %.2f s", t1);
    renderBitmapString(x_min + 5.0f, y_max - 5.0f, buf);

    std::snprintf(buf, sizeof(buf), "2: %.2f s", t2);
    renderBitmapString(x_min + 5.0f, y_max - 10.0f, buf);

    glutSwapBuffers();
}

int main(int argc,char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Two-Ball Collision");
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, update, 0);

    glutMainLoop();
    return 0;
}
