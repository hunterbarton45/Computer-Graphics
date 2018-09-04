/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */

#include "viewer.h"
#include "ctxgraf_pub.h"

#define M_PI       3.14159265358979323846

/**
 * @file line_test.cpp
 * This is the main program for an executable that tests IDrawingContext::polyline()
 * and related IDrawingContext methods.
 */

static const unsigned IMAGE_WIDTH = 800;
static const unsigned IMAGE_HEIGHT = 800;

using namespace ctxgraf;

static IDrawingContext* s_context = nullptr;
static unsigned s_testNumber = 0;
static bool s_exceptionSeen = false;

enum TEST_ID
{
    TID_RECTANGLE,
    TID_X_MAJOR_INCREASING,
    TID_X_MAJOR_DECREASING,
    TID_Y_MAJOR_INCREASING,
    TID_Y_MAJOR_DECREASING,
    TID_45_DEGREES,
    TID_CIRCLE,
    TID_STARBURST,
    TID_CIRCLE_BIDIRECTIONAL,
    TID_RECTANGLE_SMOOTH,
    TID_CIRCLE_SMOOTH,
    TID_CLIP,

    TID_TEST_COUNT
};

static const char* s_testStrings[TID_TEST_COUNT] =
{
    "Draw a centered rectangle",
    "Draw X-major lines with X increasing",
    "Draw X-major lines with X decreasing",
    "Draw Y-major lines with Y increasing",
    "Draw Y-major lines with Y decreasing",
    "Draw lines at 45 degrees",
    "Draw a counter-clockwise circle",
    "Draw a starburst",
    "Draw a circle both ways",
    "Draw a centered rectangle with smooth shading",
    "Draw a counter-clockwise circle with smooth shading",
    "Draw a polyline that needs clipping",
};


/**
 * Place vertices in a regular pattern on the circumference of a circle with the given radius.
 * Each vertex is angleDelta radians counter-clockwise from the previous vertex.
 * The first vertex is at (radius, 0) if startAngle is zero.
 */
static void setCircularVertexPattern(Vertex* vertices, unsigned count, double radius, double angleDelta, double startAngle = 0.0)
{
    double angle = startAngle;
    for (unsigned i = 0; i < count; i++)
    {
        vertices[i].x = (float)(cos(angle) * radius);
        vertices[i].y = (float)(sin(angle) * radius);
        vertices[i].color.red   = (float)((vertices[i].x + radius) / (2.0 * radius));
        vertices[i].color.blue  = (float)((vertices[i].y + radius) / (2.0 * radius));
        vertices[i].color.green = 0.5f;
        angle += angleDelta;
    }
}

/**
 * The function that is called to draw each frame.
 * This particular function draws different images based on s_testNumber.
 *
 * @param[in] surface The surface to draw onto.
 * @param[in] frame The frame number, which begins at zero and increases by one
 * for each successive frame.
 */
static void drawSurface(ISurface* surface, unsigned frame)
{
    try
    {
        surface->clear(Color(63, 63, 63));

        switch (s_testNumber)
        {
        case TID_RECTANGLE:
        case TID_RECTANGLE_SMOOTH:
        {
            Vertex vtx[5];
            for (unsigned i = 0; i < 5; i++)
            {
                vtx[i].x = (i == 1 || i == 2) ? 0.5f : -0.5f;
                vtx[i].y = (i == 2 || i == 3) ? 0.3f : -0.3f;
                vtx[i].color.red = 0.25f * i;
                vtx[i].color.green = 0.25f * (4 - i);
                vtx[i].color.blue = 0.0f;
            }
            s_context->setLineColor(VertexColor(1, 0, 0));
            if (s_testNumber == TID_RECTANGLE_SMOOTH)
                s_context->setLineShadingMode(LINE_SHADING_MODE_SMOOTH);
            s_context->polyline(surface, vtx, 5);
            break;
        }

        case TID_X_MAJOR_INCREASING:
        {
            Vertex vtx[2];
            s_context->setLineColor(VertexColor(1, 0, 1));
            for (unsigned i = 0; i < 6; i++)
            {
                vtx[0].x = -0.25f;
                vtx[0].y = -0.25f + i * 0.1f;
                vtx[1].x = vtx[0].x + 0.51f;
                vtx[1].y = -0.75f + i * 0.3f;

                s_context->polyline(surface, vtx, 2);
            }
            break;
        }

        case TID_X_MAJOR_DECREASING:
        {
            Vertex vtx[2];
            s_context->setLineColor(VertexColor(0, 1, 1));
            for (unsigned i = 0; i < 6; i++)
            {
                vtx[1].x = -0.25f;
                vtx[1].y = -0.25f + i * 0.1f;
                vtx[0].x = vtx[1].x + 0.51f;
                vtx[0].y = -0.75f + i * 0.3f;

                s_context->polyline(surface, vtx, 2);
            }
            break;
        }

        case TID_Y_MAJOR_INCREASING:
        {
            Vertex vtx[2];
            s_context->setLineColor(VertexColor(1, 0, 1));
            for (unsigned i = 0; i < 6; i++)
            {
                vtx[0].y = -0.25f;
                vtx[0].x = -0.25f + i * 0.1f;
                vtx[1].y = vtx[0].y + 0.51f;
                vtx[1].x = -0.75f + i * 0.3f;

                s_context->polyline(surface, vtx, 2);
            }
            break;
        }

        case TID_Y_MAJOR_DECREASING:
        {
            Vertex vtx[2];
            s_context->setLineColor(VertexColor(0, 1, 1));
            for (unsigned i = 0; i < 6; i++)
            {
                vtx[1].y = -0.25f;
                vtx[1].x = -0.25f + i * 0.1f;
                vtx[0].y = vtx[1].y + 0.51f;
                vtx[0].x = -0.75f + i * 0.3f;

                s_context->polyline(surface, vtx, 2);
            }
            break;
        }

        case TID_45_DEGREES:
        {
            Vertex vtx[7];
            vtx[0].x = 0.9f;
            vtx[0].y = 0.9f;
            vtx[1].x = -0.9f;
            vtx[1].y = -0.9f;
            vtx[2].x = 0.9f;
            vtx[2].y = -0.9f;
            vtx[3].x = -0.9f;
            vtx[3].y = 0.9f;
            vtx[4].x = -0.9f;
            vtx[4].y = 0.0f;
            vtx[5].x = 0.0f;
            vtx[5].y = 0.9f;
            vtx[6].x = 0.9f;
            vtx[6].y = 0.0f;

            s_context->setLineColor(VertexColor(0.5f, 1.0f, 0.5f));
            s_context->polyline(surface, vtx, 7);
            break;
        }

        case TID_CIRCLE:
        case TID_STARBURST:
        case TID_CIRCLE_SMOOTH:
        {
            static const float RADIUS = 0.7f;
            static const unsigned COUNT = 36;
            Vertex vtx[COUNT + 1];

            const double angleDelta = 2.0 * M_PI / COUNT * (s_testNumber == TID_STARBURST ? 13 : 1);
            setCircularVertexPattern(vtx, COUNT + 1, RADIUS, angleDelta);

            s_context->setLineColor(VertexColor(1, 1, 1));
            if (s_testNumber == TID_CIRCLE_SMOOTH)
                s_context->setLineShadingMode(LINE_SHADING_MODE_SMOOTH);
            s_context->polyline(surface, vtx, COUNT + 1);
            break;
        }

        case TID_CIRCLE_BIDIRECTIONAL:
        {
            static const float RADIUS = 0.7f;
            static const unsigned COUNT = 36;
            Vertex vtx[COUNT + 1];

            const double angleDelta = 2.0 * M_PI / COUNT;

            // Draw circle counter-clockwise in red
            setCircularVertexPattern(vtx, COUNT + 1, RADIUS, angleDelta);
            s_context->setLineColor(VertexColor(1, 0, 0));
            s_context->polyline(surface, vtx, COUNT + 1);

            // Draw same circle clockwise in green (except for final segment)
            setCircularVertexPattern(vtx, COUNT + 1, RADIUS, -angleDelta);
            s_context->setLineColor(VertexColor(0, 1, 0));
            s_context->polyline(surface, vtx, COUNT);

            break;
        }

        case TID_CLIP:
        {
            static const float RADIUS = 1.3f;
            static const unsigned COUNT = 36;
            Vertex vtx[COUNT + 1];

            const double angleDelta = 2.0 * M_PI / COUNT * 13;
            setCircularVertexPattern(vtx, COUNT + 1, RADIUS, angleDelta);

            s_context->setLineColor(VertexColor(1, 1, 1));
            s_context->setLineShadingMode(LINE_SHADING_MODE_SMOOTH);
            s_context->polyline(surface, vtx, COUNT + 1);
            break;
        }

        default:
            fprintf(stderr, "Unknown test id: %d\n", s_testNumber);
            getchar();
            exit(1);
        }
    }
    catch (Exception& ex)
    {
        fprintf(stderr, "ctxgraf error: %s\n", ex.what());
        s_exceptionSeen = true;
        getchar();
    }
    catch (std::exception& ex)
    {
        fprintf(stderr, "unknown error: %s\n", ex.what());
        s_exceptionSeen = true;
        getchar();
    }
}

int main(int argc, char** argv)
{
    if (argc != 2 || sscanf(argv[1], " %u", &s_testNumber) != 1)
    {
        fprintf(stderr, "usage: line_test <test number>\n");
        getchar();
        return 255;
    }
    if (s_testNumber >= TID_TEST_COUNT)
    {
        fprintf(stderr, "test number (%d) is out of range\n", s_testNumber);
        getchar();
        return 255;
    }


    printf("Running test %d (%s)\n", s_testNumber, s_testStrings[s_testNumber]);

    try
    {
        ITop* top = getTop();
        ISurface* surface = top->createSurface(PF_RGB_888, IMAGE_WIDTH, IMAGE_HEIGHT);
        s_context = top->createDrawingContext();
        Viewer* viewer = Viewer::createViewer(surface);

        viewer->start(drawSurface);
        return 0;
    }
    catch (Exception& ex)
    {
        fprintf(stderr, "ctxgraf error: %s\n", ex.what());
        s_exceptionSeen = true;
        getchar();
    }
    catch (std::exception& ex)
    {
        fprintf(stderr, "unknown error: %s\n", ex.what());
        s_exceptionSeen = true;
        getchar();
    }

    return 1;
}
