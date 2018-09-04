/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */


/**
 * @file ctxoglsample.cpp
 * A sample program demonstrating the use of the ctxogl classes.
 */

#include <exception>
#include <stdio.h>
#include "glew.h"
#include "freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "host.h"

using namespace ctxogl;


/**
 * This is the drawing callback function.  It will be called repeatedly to render
 * the desired image.  The frameCounter parameter will begin at zero (for the first call)
 * and be incremented by one for every call thereafter.
 */
static void draw(unsigned frameCounter, float aspectRatio)
{
    // Set up view and projection matrices
    glm::mat4 projMatrix = glm::perspective(
        glm::radians(20.0f),    // Field of view (in radians)
        aspectRatio,            // Aspect ratio of drawing surface
        0.1f,                   // Z near clip plane
        10.0f);                 // Z far clip plane
    glm::mat4 viewMatrix = glm::lookAt(
        glm::vec3(4, 0, 3), // Camera position, in world coordinates
        glm::vec3(0, 0, 0), // Camera target, in world coordinates
        glm::vec3(0, 1, 0)  // Up vector, in world coordinates
    );
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(&projMatrix[0][0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(&modelViewMatrix[0][0]);

    // Enable standard alpha transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Clear the drawing surface
    glClearColor(0.5f, 0.5f, 0.2f, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw a big triangle
    glBegin(GL_TRIANGLES);
    {
        glColor4ub(255, 0, 0, 255);
        glVertex3f(0.1f, -0.0f, 0.0f);
        glColor4ub(0, 255, 0, 255);
        glVertex3f(0.5f, 0.8f, 0.0f);
        glColor4ub(0, 0, (frameCounter*2) % 256, 255);  // animating the blue channel on the third vertex
        glVertex3f(-0.7f, 0.3f, 0.0f);
    }
    glEnd();

    // Draw the famous teapot!
    GLubyte teapotIntensity = (frameCounter * 3) % 256;
    glColor4ub(teapotIntensity, teapotIntensity, teapotIntensity, 63);  // notice the alpha value: translucent!
    glutSolidTeapot(0.25);
}

/**
 * This is the keyboard callback function.  It is called when the user presses a key while the
 * drawing window has focus.
 */
static void keyboard(unsigned char key, int x, int y, bool shift, bool ctrl)
{
    printf("Key '%c' pressed at %d,%d (shift=%d, ctrl=%d)\n", key, x, y, shift, ctrl);
}

/**
 * This is the mouse movemnt callback function.  It is called when the user holds down the left button
 * and moves the mouse.
 */
static void mouse(int x, int y)
{
    printf("Mouse moved to %d, %d\n", x, y);
}

int main(int argc, char** argv)
{
    static const unsigned WINDOW_WIDTH = 512;
    static const unsigned WINDOW_HEIGHT = 512;

    try
    {
        Host host(WINDOW_WIDTH, WINDOW_HEIGHT, "CTX OGL Sample");

        host.start(draw, keyboard, mouse);
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "Exception caught: %s\n", e.what());
        return 1;
    }
    catch (...)
    {
        fprintf(stderr, "Unknown exception caught\n");
        return 1;
    }

    return 0;
}
