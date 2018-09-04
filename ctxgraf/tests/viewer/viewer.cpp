#include "viewer.h"

#include "glew.h"
#include "wglew.h"
#include "freeglut.h"


namespace ctxgraf
{

static const unsigned MIN_WIDTH = 16;
static const unsigned MIN_HEIGHT = 16;

static Viewer* s_currentViewer = NULL;
static GLuint s_textureName = 0;


static void initTexture()
{
    // Create a new texture object and bind that to the context
    glCreateTextures(GL_TEXTURE_2D, 1, &s_textureName);
    glBindTexture(GL_TEXTURE_2D, s_textureName);

    // Set source texture data to be tightly-packed
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Set texture filtering (both minification and magnification) to be bilinear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Let's use texture unit 0
    glActiveTexture(GL_TEXTURE0);

    // Set texture drawing mode to DECAL
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Finally, enable 2D texturing!
    glEnable(GL_TEXTURE_2D);
}

static void loadTexture()
{
    ISurface* image = s_currentViewer->getSurface();

    // Reload the current texture with the new surface data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->getWidth(), image->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image->getStart());
}

static void display()
{
    if (!s_currentViewer)
        throw InternalException("No current viewer in display()");

    static unsigned frame = 0;

    if (frame == 0)
        initTexture();

    s_currentViewer->getCallback()(s_currentViewer->getSurface(), frame);
    loadTexture();

    glClearColor(0.2f, 0.2f, 0.2f, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    {
        glColor4ub(255, 0, 255, 255);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0, -1.0, 0.0);
        glTexCoord2f(1, 1);
        glVertex3f(1.0, -1.0, 0.0);
        glTexCoord2f(1, 0);
        glVertex3f(1.0, 1.0, 0.0);

        glColor4ub(0, 255, 255, 255);

        glTexCoord2f(0, 1);
        glVertex3f(-1.0, -1.0, 0.0);
        glTexCoord2f(1, 0);
        glVertex3f(1.0, 1.0, 0.0);
        glTexCoord2f(0, 0);
        glVertex3f(-1.0, 1.0, 0.0);
    }
    glEnd();
    glFlush();
    glutSwapBuffers();

    frame++;
}

static void timer(int value)
{
    glutTimerFunc(16, timer, 0);
    glutPostRedisplay();
}

Viewer* Viewer::createViewer(ISurface* surface)
{
    if (!surface)
        throw ParameterException("NULL surface");
    if (surface->getWidth() < MIN_WIDTH || surface->getHeight() < MIN_HEIGHT)
        throw ParameterException("Surface is too small");
    if (surface->getFormat() != PF_RGB_888)
        throw ParameterException("Unsupported pixel format");

    Viewer* viewer = new Viewer(surface);

    return viewer;
}

Viewer::~Viewer()
{
}

Viewer::Viewer(ISurface* surface)
    : m_surface(surface)
    , m_running(false)
    , m_renderCallback(NULL)
{
}

void Viewer::start(RenderCallback callback)
{
    if (m_running)
        return;
    if (s_currentViewer)
        throw NotImplementedException("Cannot have multiple viewers");

    s_currentViewer = this;
    m_renderCallback = callback;
    launchGlut();
    m_running = true;
}

void Viewer::stop()
{
    if (!m_running)
        return;

    stopGlut();
    m_running = false;
    m_renderCallback = NULL;
    s_currentViewer = NULL;
}

void Viewer::launchGlut()
{
    int argc = 1;
    char* argv[1] = {(char*)"ignore"};
    glutInit(&argc, argv);
    glutInitWindowSize(m_surface->getWidth(), m_surface->getHeight());
    glutCreateWindow("CTX Test");

    GLenum err=glewInit();
    if (GLEW_OK != err)
    {
        char msg[1024];
        snprintf(msg, sizeof(msg), "GLEW init failed: %s", glewGetErrorString(err));
        throw InternalException(msg);
    }

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
}

void Viewer::stopGlut()
{
    glutLeaveMainLoop();
}

} // namespace ctxgraf
