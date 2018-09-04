/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */


/**
 * @file host.cpp
 * The implementation of the Host class. 
 */

#include "host.h"
#include <exception>
#include <string>

#include "glew.h"
#include "wglew.h"
#include "freeglut.h"


namespace ctxogl
{

/** The current (singleton) Host object, or NULL if there isn't one. */
static Host* s_currentHost = nullptr;

/**
 * The display callback for GLUT.
 */
static void display()
{
    if (!s_currentHost)
        throw std::exception("No current host in display()");

    s_currentHost->makeDrawingCallback();
 
    glFlush();
    glutSwapBuffers();
}

/**
 * The keyboard callback for GLUT.
 */
static void keyboard(unsigned char key, int x, int y)
{
    if (!s_currentHost)
        throw std::exception("No current host in keyboard()");

    int modifiers = glutGetModifiers();
    bool shiftPressed = (modifiers & GLUT_ACTIVE_SHIFT) ? true : false;
    bool ctrlPressed = (modifiers & GLUT_ACTIVE_CTRL) ? true : false;

    s_currentHost->makeKeyboardCallback(key, x, y, shiftPressed, ctrlPressed);
}

/**
 * The timer callback for GLUT.
 * It resets the timer and marks the window as needing to be redisplayed.
 */
static void timer(int value)
{
    static const unsigned TIMER_INTERVAL_IN_MSEC = 33;  // for approximately 30 fps

    glutTimerFunc(TIMER_INTERVAL_IN_MSEC, timer, 0);
    glutPostRedisplay();
}


Host::Host(unsigned width, unsigned height, const char* windowName)
    : m_width(width)
    , m_height(height)
    , m_frameCounter(0)
    , m_drawFunction(nullptr)
    , m_keyFunction(nullptr)
{
    if (s_currentHost)
        throw std::exception("Host object already exists");

    int argc = 1;
    char* argv[1] = { (char*)"ignore" };
    glutInit(&argc, argv);
    glutInitWindowSize(m_width, m_height);
    glutCreateWindow(windowName);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        char msg[1024];
        snprintf(msg, sizeof(msg), "GLEW init failed: %s", glewGetErrorString(err));
        throw std::exception(msg);
    }

    s_currentHost = this;
}

Host::~Host()
{
    if (s_currentHost == this)
        s_currentHost = nullptr;
}

void Host::start(DrawFunction* drawFunction, KeyFunction* keyFunction, MouseFunction* mouseFunction)
{
    if (m_drawFunction)
        throw std::exception("start() has already been called");

    m_frameCounter = 0;
    m_drawFunction = drawFunction;
    m_keyFunction = keyFunction;
    m_mouseFunction = mouseFunction;

    if (m_keyFunction)
        glutKeyboardFunc(keyboard);
    if (m_mouseFunction)
        glutMotionFunc(m_mouseFunction);

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
}

void Host::stop()
{
    m_drawFunction = nullptr;
}

void Host::makeDrawingCallback()
{
    if (!m_drawFunction)
        return;

    const float aspectRatio = (float)m_width / (float)m_height;
    m_drawFunction(m_frameCounter, aspectRatio);

    m_frameCounter += 1;
}

void Host::makeKeyboardCallback(unsigned char key, int x, int y, bool shift, bool ctrl)
{
    if (m_keyFunction)
        m_keyFunction(key, x, y, shift, ctrl);
}


} // namespace ctxogl


