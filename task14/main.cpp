/*
 * task14.cpp
 */

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <GL/gl.h>
#include <GL/glut.h>

const float dt = 0.015f;
const int time_step  = 15;
const float l_view = 150.0f;

const float cannon_w = 10.0f;
const float cannon_h = 6.0f;
float cannon_x = 0.0f;

const float proj_mass[3] = { 1.0f, 2.0f, 3.0f };
const float box_mass [3] = { 5.0f, 10.0f, 15.0f };

const float proj_r0 = 3.0f;                 // radius for 1
const float box_s0 = 12.0f;                 // size for 5
const float target_y = l_view - 5.0f;
const float cannon_y = -l_view + 10.0f;
const float vy0 = 90.0f;                    // projectile v0
const float box_v0 = -25.0f;                // box v0

struct Projectile {
    float x, y;
    float vy;
    float r;
    float mass;
    bool  active = true;
};
struct Box {
    float x, y;
    float vy;
    float size;
    float mass;
    bool  reached_target = false;
};

static std::vector<Projectile> projectiles;
static std::vector<Box> boxes;

static float game_time = 0.0f;
static bool game_over = false;
static bool game_win  = false;

static float world_left = -l_view;
static float world_top = l_view;

void drawText(float x, float y, const std::string& s)
{
    glRasterPos2f(x, y);
    for (char c : s) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
}

bool intersects(const Projectile& p, const Box& b)
{
    float dx = std::fabs(p.x - b.x);
    float dy = std::fabs(p.y - b.y);
    float half = b.size * 0.5f;

    if (dx > half + p.r || dy > half + p.r)
        return false;

    if (dx <= half || dy <= half)
        return true;

    float corner = (dx - half)*(dx - half) + (dy - half)*(dy - half);
    return corner <= p.r * p.r;
}


void fireProjectile(int idx)
{
    for (auto& p : projectiles)
        if (p.active) return;

    Projectile p;

    {
        p.mass = proj_mass[idx];
        p.r = proj_r0 * (p.mass / proj_mass[0]); // mass -> size
        p.x = cannon_x;
        p.y = cannon_y + cannon_h * 0.5f + p.r;
        p.vy = vy0;
    }

    projectiles.push_back(p);
}

void resetSimulation()
{
    projectiles.clear();
    boxes.clear();
    game_over = game_win = false;
    game_time = 0.0f;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const float spawn_top = target_y - 20.0f;   // spawn point

    for (int i = 0; i < 3; ++i)
    {
        Box b;

        {
            b.mass = box_mass[i];
            b.size = box_s0 * (b.mass / box_mass[0]);       // mass -> sizw
            b.x = -l_view + b.size * 0.5f + std::rand() % static_cast<int>(2 * l_view - b.size);
            b.y = spawn_top - i * 30.0f;
            b.vy = box_v0;
        }

        boxes.push_back(b);
    }
}


void handleKeyboard(unsigned char key, int, int)
{
    switch (key)
    {
        case 'a':
            cannon_x -= 2.0f;
            break;
        case 'd':
            cannon_x += 2.0f;
            break;
        case '1':
            fireProjectile(0);
            break;
        case '2':
            fireProjectile(1);
            break;
        case '3':
            fireProjectile(2);
            break;
        case 27:
            std::exit(0);
    }

    cannon_x = std::max(-l_view + cannon_w*0.5f, std::min(l_view - cannon_w*0.5f, cannon_x));
}

void physicsStep()
{
    if (game_over || game_win) return;

    game_time += dt;

    // projectiles
    for (auto& p : projectiles)
    {
        if (!p.active) continue;
        p.y += p.vy * dt;
        if (p.y - p.r > l_view) p.active = false;
    }

    // boxes
    for (auto& b : boxes)
    {
        if (b.reached_target) continue;

        b.y += b.vy * dt;

        // top
        if (b.y + b.size*0.5f >= target_y)
        {
            b.y = target_y - b.size*0.5f;
            b.vy = 0.0f;
            b.reached_target = true;
        }
        // bottom
        if (b.y - b.size*0.5f <= -l_view)
            game_over = true;
    }

    // collisions
    for (auto& p : projectiles)
    {
        if (!p.active) continue;
        for (auto& b : boxes)
        {
            if (b.reached_target) continue;
            if (intersects(p, b))
            {
                float total = p.mass + b.mass;
                b.vy = (p.mass * p.vy + b.mass * b.vy) / total;
                p.active = false;
                break;
            }
        }
    }

    // win
    bool all = true;
    for (auto& b : boxes)
        if (!b.reached_target) { all = false; break; }

    if (all)
        game_win = true;
}

void drawRect(float cx, float cy, float w, float h)
{
    glBegin(GL_QUADS);
        glVertex2f(cx - w*0.5f, cy - h*0.5f);
        glVertex2f(cx + w*0.5f, cy - h*0.5f);
        glVertex2f(cx + w*0.5f, cy + h*0.5f);
        glVertex2f(cx - w*0.5f, cy + h*0.5f);
    glEnd();
}
void drawCircle(float cx, float cy, float r, int seg = 30)
{
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= seg; ++i)
        {
            float a = 2.f * M_PI * i / seg;
            glVertex2f(cx + r * std::cos(a), cy + r * std::sin(a));
        }
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // target
    glColor3f(1,0,0);
    drawRect(0, target_y, 2*l_view, 6);

    // cannon
    glColor3f(0,0.7f,1);
    drawRect(cannon_x, cannon_y, cannon_w, cannon_h);

    // boxes
    glColor3f(0.9f,0.9f,0);
    for (auto& b : boxes)
        drawRect(b.x, b.y, b.size, b.size);

    // projectiles
    glColor3f(0,1,0);
    for (auto& p : projectiles)
        if (p.active) drawCircle(p.x, p.y, p.r);

    // timer
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.1f", game_time);
    glColor3f(1,1,1);
    drawText(world_left + 5, world_top - 12, buf);

    // messages
    if (game_over)
    {
        glColor3f(1,0,0);
        drawText(-30, 0, "GAME  OVER");
    }
    if (game_win)
    {
        glColor3f(0,1,0);
        drawText(-25, 0, "WIN");
    }

    glutSwapBuffers();
}

void timer(int)
{
    physicsStep();
    glutPostRedisplay();
    glutTimerFunc(time_step, timer, 0);
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float aspect = float(w) / float(h);
    gluOrtho2D(-l_view * aspect, l_view * aspect, -l_view, l_view);

    world_left = -l_view * aspect;
    world_top  =  l_view;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    resetSimulation();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("Momentum Game");

    glClearColor(0,0,0,1);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(handleKeyboard);
    glutTimerFunc(time_step, timer, 0);

    glutMainLoop();
    return 0;
}
