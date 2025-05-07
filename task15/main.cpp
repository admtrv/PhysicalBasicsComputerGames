/*
 * task15.cpp
 */

#include <cmath>
#include <cstdio>
#include <limits>
#include <GL/gl.h>
#include <GL/glut.h>

#define INF std::numeric_limits<float>::infinity();

const float dt = 0.01f;
const int time_step = 10;
const float l_view = 120.0f;

// hud - timers
const int HUD_MARGIN = 20;
const int HUD_LINE = 15;
int width = 800;
int height = 600;

// walls
static float x_min, x_max, y_min, y_max;

void drawCircle(float r, int seg = 40)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);

    for (int i = 0; i <= seg; ++i)
    {
        float a = 2.0f * M_PI * i / seg;
        glVertex2f(r * std::cos(a), r * std::sin(a));
    }
    glEnd();
}

void beginOverlay()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

void endOverlay()
{
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawString(float x, float y, const char* text)
{
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c)
    {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
    }
}

struct Vec2 {
    float x, y;

    Vec2 operator+(const Vec2& o) const
    {
        return {x + o.x, y + o.y};
    }

    Vec2 operator-(const Vec2& o) const
    {
        return {x - o.x, y - o.y};
    }

    Vec2 operator*(float s) const
    {
        return {x * s, y * s};
    }

    Vec2& operator+=(const Vec2& o)
    {
        x += o.x;
        y += o.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& o)
    {
        x -= o.x;
        y -= o.y;
        return *this;
    }

    static float dot(const Vec2& a, const Vec2& b)
    {
        return a.x*b.x + a.y*b.y;
    }
};

class Ball {
public:
    float r;            // radius
    float m;            // mass
    Vec2  p;            // position
    Vec2  v;            // velocity
    float cr, cg, cb;   // color

    Ball(float radius, float mass, const Vec2& pos, const Vec2& vel,
         float red, float green, float blue)
        : r(radius), m(mass), p(pos), v(vel), cr(red), cg(green), cb(blue) {}

    void update(float dt)
    {
        p += v * dt;
    }

    void collisionWall()
    {
        if (p.x - r < x_min)
        {
            p.x = x_min + r;
            v.x = -v.x;
        }
        else if (p.x + r > x_max)
        {
            p.x = x_max - r;
            v.x = -v.x;
        }

        if (p.y - r < y_min)
        {
            p.y = y_min + r;
            v.y = -v.y;
        }
        else if (p.y + r > y_max)
        {
            p.y = y_max - r;
            v.y = -v.y;
        }
    }

    float timeToWall() const
    {
        float tx = INF;
        float ty = INF;

        if (v.x > 0.0f)
            tx = (x_max - r - p.x) / v.x;
        else if (v.x < 0.0f)
            tx = (x_min + r - p.x) / v.x;

        if (v.y > 0.0f)
            ty = (y_max - r - p.y) / v.y;
        else if (v.y < 0.0f)
            ty = (y_min + r - p.y) / v.y;

        return std::fmax(0.0f, std::fmin(tx, ty));
    }

    void draw() const
    {
        glPushMatrix();
            glTranslatef(p.x, p.y, 0.0f);
            glColor3f(cr, cg, cb);
            drawCircle(r);
            glColor3f(1.0f, 1.0f, 1.0f);
        glPopMatrix();
    }

    static float timeToBall(const Ball& A, const Ball& B)
    {
        Vec2 dp = A.p - B.p;
        Vec2 dv = A.v - B.v;

        float a = Vec2::dot(dv, dv);
        float b = 2.0f * Vec2::dot(dp, dv);
        float c = Vec2::dot(dp, dp) - (A.r + B.r) * (A.r + B.r);

        float disc = b*b - 4.0f*a*c;
        if (disc < 0.0f)
            return std::numeric_limits<float>::infinity();

        float t = (-b - std::sqrt(disc)) / (2.0f*a);
        return (t > 0.0f) ? t : std::numeric_limits<float>::infinity();
    }

    static void collision(Ball& A, Ball& B)
    {
        Vec2 r12 = A.p - B.p;
        float dist = Vec2::dot(r12, r12);
        float min_d2 = (A.r + B.r) * (A.r + B.r);

        if (dist > min_d2)  // no collision
            return;

        Vec2 p12{ A.m * A.v.x - B.m * B.v.x, A.m * A.v.y - B.m * B.v.y };
        float S = r12.x * p12.x + r12.y * p12.y;
        float Ix = -r12.x / dist * S;
        float Iy = -r12.y / dist * S;

        A.v.x += Ix / A.m;
        A.v.y += Iy / A.m;
        B.v.x -= Ix / B.m;
        B.v.y -= Iy / B.m;
    }
};

struct Timer {
    const Ball* self;
    const Ball* other;
    const char* label;
    int order;

    float value() const
    {
        float t_wall = self->timeToWall();
        float t_bb = Ball::timeToBall(*self, *other);
        return std::fmin(t_wall, t_bb);
    }

    void draw() const
    {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%s: %.2f s", label, value());

        float x = HUD_MARGIN;
        float y = height - HUD_MARGIN - order * HUD_LINE;

        glRasterPos2f(x, y);

        for (const char* c = buf; *c; ++c)
        {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
        }
    }
};

// balls
static Ball ball1{ 10.0, 1.0, { 45.0f, -50.0f}, {-15.0f,  18.0f}, 1.0f, 0.0f, 0.0f };
static Ball ball2{ 10.0, 1.0, {-45.0f,  55.0f}, { 17.0f, -16.0f}, 0.0f, 0.0f, 1.0f };

// timers
Timer timer1{ &ball1, &ball2, "Red", 0 };
Timer timer2{ &ball2, &ball1, "Blue", 1 };

void update(int)
{
    // update coordinates
    ball1.update(dt);
    ball2.update(dt);

    // check for collision
    Ball::collision(ball1, ball2);
    ball1.collisionWall();
    ball2.collisionWall();

    glutPostRedisplay();
    glutTimerFunc(time_step, update, 0);
}

void handleResize(int w, int h)
{
    if (w == 0) w = 1;
    if (h == 0) h = 1;

    width = w;
    height = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = static_cast<float>(w) / h;

    if (aspect >= 1.0f) {
        gluOrtho2D(-l_view * aspect, l_view * aspect, -l_view, l_view);
        x_min = -l_view * aspect;
        x_max =  l_view * aspect;
        y_min = -l_view;
        y_max =  l_view;
    } else {
        gluOrtho2D(-l_view, l_view, -l_view / aspect, l_view / aspect);
        x_min = -l_view;
        x_max =  l_view;
        y_min = -l_view / aspect;
        y_max =  l_view / aspect;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // balls
    ball1.draw();
    ball2.draw();

    // timers
    beginOverlay();
        glColor3f(1,1,1);
        timer1.draw();
        timer2.draw();
    endOverlay();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Two-Ball Collision");
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glutDisplayFunc(display);
    glutReshapeFunc(handleResize);
    glutTimerFunc(time_step, update, 0);

    glutMainLoop();
    return 0;
}
