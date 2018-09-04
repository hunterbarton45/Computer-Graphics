/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */

#include "viewer.h"
#include "ctxgraf_pub.h"

#define M_PI       3.14159265358979323846

/**
 * @file triangle_test.cpp
 * This is the main program for an executable that tests IDrawingContext::triangle()
 * and related IDrawingContext methods.
 */

static const unsigned IMAGE_WIDTH = 768;
static const unsigned IMAGE_HEIGHT = 768;

using namespace ctxgraf;

static IDrawingContext* s_context = nullptr;
static IZBuffer* s_zBuffer = nullptr;
static unsigned s_testNumber = 0;
static bool s_exceptionSeen = false;

enum TEST_ID
{
    TID_ONE_TRIANGLE_CONSTANT_COLOR,
    TID_ONE_TRIANGLE_SHADED,
    TID_SIMPLE_Z,
    TID_HORIZONTAL_EDGES,
    TID_TRIANGLE_CIRCLE,
    TID_TRIANGLE_CIRCLE_ROTATING,
    TID_MODERATE_Z,
    TID_CLIPPING,
    TID_TRIANGLE_CIRCLE_ROTATING_WITH_Z,
    TID_DEGENERATES,

    TID_TEST_COUNT
};

static const char* s_testStrings[TID_TEST_COUNT] =
{
    "Draw a single triangle with all vertices one color",
    "Draw a single triangle with Gouraud shading",
    "Test simple Z-buffering",
    "Draw triangles with horizontal edges",
    "Draw a many-sided polygon",
    "Draw a many-sided polygon that rotates",
    "Test slightly more complex Z-buffering",
    "Test x/y clipping",
    "Draw a many-sided polygon that rotates, with Z-buffering",
    "Draw some degenerate triangles (and one plain one)",
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
        float red =  (float)(((cos(angle - startAngle) * radius) + radius) / (2.0 * radius));
        float blue = (float)(((sin(angle - startAngle) * radius) + radius) / (2.0 * radius));
        vertices[i].color.red   = red;
        vertices[i].color.blue  = blue;
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
        surface->clear(Color(15, 15, 63));

        switch (s_testNumber)
        {
        case TID_ONE_TRIANGLE_CONSTANT_COLOR:
        {
            Vertex v1(-0.5f, -0.5f, 0.0f, VertexColor(1.0f, 0.5f, 0.8f));
            Vertex v2(0.4f, -0.3f, 0.0f, VertexColor(1.0f, 0.5f, 0.8f));
            Vertex v3(-0.1f, 0.4f, 0.0f, VertexColor(1.0f, 0.5f, 0.8f));

            s_context->triangle(surface, nullptr, &v1, &v2, &v3);
            break;
        }

        case TID_ONE_TRIANGLE_SHADED:
        {
            Vertex v1(-0.5f, -0.5f, 0.0f, VertexColor(1.0f, 0.0f, 0.0f));
            Vertex v2(0.4f, -0.3f, 0.0f, VertexColor(0.0f, 0.0f, 1.0f));
            Vertex v3(-0.1f, 0.4f, 0.0f, VertexColor(0.0f, 1.0f, 0.0f));

            s_context->triangle(surface, nullptr, &v1, &v2, &v3);
            break;
        }

        case TID_HORIZONTAL_EDGES:
        {
            const VertexColor theColor(0.1f, 0.9f, 1.0f);

            Vertex v1(-0.2f, -0.2f, 0.0f, theColor);
            Vertex v2(0.1f, -0.2f, 0.0f, theColor);
            Vertex v3(-0.2f, 0.1f, 0.0f, theColor);
            s_context->triangle(surface, nullptr, &v1, &v2, &v3);

            Vertex v4(0.2f, 0.2f, 0.0f, theColor);
            Vertex v5(-0.1f, 0.2f, 0.0f, theColor);
            Vertex v6(0.2f, -0.1f, 0.0f, theColor);
            s_context->triangle(surface, nullptr, &v4, &v5, &v6);

            break;
        }

        case TID_SIMPLE_Z:
        {
            s_zBuffer->clear(0xFFFF);

            Vertex v1(-0.5f, -0.5f, 0.0f, VertexColor(0.0f, 1.0f, 0.0f));
            Vertex v2(0.4f, -0.3f, 0.0f, VertexColor(0.0f, 0.0f, 1.0f));
            Vertex v3(-0.1f, 0.4f, 0.0f, VertexColor(0.0f, 1.0f, 1.0f));
            s_context->triangle(surface, s_zBuffer, &v1, &v2, &v3);

            Vertex v4(-0.6f, -0.6f, 0.1f, VertexColor(1.0f, 0.0f, 0.0f));
            Vertex v5(0.5f, -0.3f, 0.1f, VertexColor(1.0f, 0.0f, 0.0f));
            Vertex v6(-0.2f, 0.2f, 0.1f, VertexColor(1.0f, 0.0f, 0.0f));
            s_context->triangle(surface, s_zBuffer, &v4, &v5, &v6);

            break;
        }

        case TID_TRIANGLE_CIRCLE:
        case TID_TRIANGLE_CIRCLE_ROTATING:
        {
            static const double RADIUS = 0.6;
            static const unsigned PERIMETER_COUNT = 24;
            static const float FRAME_ANGLE_DELTA = 0.1f;

            Vertex perimeter[PERIMETER_COUNT];
            Vertex center(0.0f, 0.0f, 0.0f, VertexColor(1.0f, 1.0f, 1.0f));
            double startAngle = (s_testNumber == TID_TRIANGLE_CIRCLE_ROTATING ? frame * FRAME_ANGLE_DELTA : 0.1);

            setCircularVertexPattern(perimeter, PERIMETER_COUNT, RADIUS, 2.0f * M_PI / PERIMETER_COUNT, startAngle);

            for (unsigned i = 0; i < PERIMETER_COUNT - 1; i++)
                s_context->triangle(surface, nullptr, perimeter + i, perimeter + i + 1, &center);
            s_context->triangle(surface, nullptr, perimeter + PERIMETER_COUNT - 1, perimeter, &center);

            break;
        }

        case TID_MODERATE_Z:
        {
            // Test slightly more complex Z-buffering

            s_zBuffer->clear(0xFFFF);

            {
                // Draw near square
                Vertex v1(-0.25f, -0.25f, -0.1f, VertexColor(0.0f, 1.0f, 0.0f));
                Vertex v2(0.25f, -0.25f, -0.1f, VertexColor(1.0f, 1.0f, 0.0f));
                Vertex v3(0.25f, 0.25f, -0.1f, VertexColor(1.0f, 1.0f, 0.0f));
                Vertex v4(-0.25f, 0.25f, -0.1f, VertexColor(0.0f, 1.0f, 0.0f));
                s_context->triangle(surface, s_zBuffer, &v1, &v2, &v3);
                s_context->triangle(surface, s_zBuffer, &v3, &v4, &v1);
            }

            {
                // Draw far square
                Vertex v1(-0.35f, -0.35f, 0.1f, VertexColor(1.0f, 0.0f, 0.0f));
                Vertex v2(0.35f, -0.35f, 0.1f, VertexColor(1.0f, 0.0f, 0.0f));
                Vertex v3(0.35f, 0.35f, 0.1f, VertexColor(0.5f, 0.0f, 1.0f));
                Vertex v4(-0.35f, 0.35f, 0.1f, VertexColor(0.5f, 0.0f, 1.0f));
                s_context->triangle(surface, s_zBuffer, &v1, &v2, &v3);
                s_context->triangle(surface, s_zBuffer, &v3, &v4, &v1);
            }

            {
                // Draw penetrating rectangle
                Vertex v1(-0.15f, -0.45f, 0.3f, VertexColor(0.5f, 0.5f, 0.5f));
                Vertex v2(0.15f, -0.45f, -0.2f, VertexColor(0.7f, 0.7f, 0.7f));
                Vertex v3(0.15f, 0.45f, -0.2f, VertexColor(0.7f, 0.7f, 0.7f));
                Vertex v4(-0.15f, 0.45f, 0.3f, VertexColor(0.5f, 0.5f, 0.5f));
                s_context->triangle(surface, s_zBuffer, &v1, &v2, &v3);
                s_context->triangle(surface, s_zBuffer, &v3, &v4, &v1);
            }

            break;
        }

        case TID_CLIPPING:
        {
            // Test x/y clipping

            Vertex v1(0.5f, -1.2f, 0.0f, VertexColor(1.0f, 0.0f, 0.0f));
            Vertex v2(-1.4f, -0.3f, 0.0f, VertexColor(0.0f, 0.0f, 1.0f));
            Vertex v3(0.1f, 1.4f, 0.0f, VertexColor(0.0f, 1.0f, 0.0f));

            s_context->triangle(surface, nullptr, &v1, &v2, &v3);
            break;
        }

        case TID_TRIANGLE_CIRCLE_ROTATING_WITH_Z:
        {
            // Draw a many-sided polygon that rotates, with Z-buffering

            static const double RADIUS = 0.45;
            static const unsigned PERIMETER_COUNT = 36;
            static const float FRAME_ANGLE_DELTA = 0.1f;

            Vertex perimeter[PERIMETER_COUNT];
            Vertex center(0.0f, 0.0f, 0.0f, VertexColor(1.0f, 1.0f, 1.0f));
            double startAngle = frame * FRAME_ANGLE_DELTA;

            setCircularVertexPattern(perimeter, PERIMETER_COUNT, RADIUS, 2.0f * M_PI / PERIMETER_COUNT, startAngle);

            // Change the Z value of all perimeter vertices to -0.2, so that the perimeter is closer than the center
            for (unsigned i = 0; i < PERIMETER_COUNT; i++)
                perimeter[i].z = -0.2f;

            s_zBuffer->clear(0xFFFF);

            // Draw the circle
            for (unsigned i = 0; i < PERIMETER_COUNT - 1; i++)
                s_context->triangle(surface, s_zBuffer, perimeter + i, perimeter + i + 1, &center);
            s_context->triangle(surface, s_zBuffer, perimeter + PERIMETER_COUNT - 1, perimeter, &center);

            {
                // Draw penetrating rectangle
                Vertex v1(-0.15f, -0.55f, 0.2f, VertexColor(0.5f, 0.5f, 0.5f));
                Vertex v2(0.15f, -0.55f, -0.3f, VertexColor(0.7f, 0.7f, 0.7f));
                Vertex v3(0.15f, 0.55f, -0.3f, VertexColor(0.7f, 0.7f, 0.7f));
                Vertex v4(-0.15f, 0.55f, 0.2f, VertexColor(0.5f, 0.5f, 0.5f));
                s_context->triangle(surface, s_zBuffer, &v1, &v2, &v3);
                s_context->triangle(surface, s_zBuffer, &v3, &v4, &v1);
            }

            break;
        }

        case TID_DEGENERATES:
        {
            // Draw some degenerate triangles (and one plain one)

            {
                // Draw a plain little (rotating) triangle in the center
                Vertex vtx[3];
                setCircularVertexPattern(vtx, 3, 0.1, 2.0f * M_PI / 3, 0.1 * frame);
                s_context->triangle(surface, nullptr, vtx + 0, vtx + 1, vtx + 2);
            }

            {
                // Draw a triangle with coincident vertices
                Vertex v1(-0.5f, -0.6f, 0.0f, VertexColor(1.0f, 0.0f, 0.0f));
                Vertex v2(-0.5f, -0.6f, 0.0f, VertexColor(1.0f, 1.0f, 0.0f));
                Vertex v3(-0.5f, -0.6f, 0.0f, VertexColor(1.0f, 0.0f, 1.0f));
                s_context->triangle(surface, nullptr, &v1, &v2, &v3);
            }

            {
                // Draw a triangle with vertices in a horizontal line
                Vertex v1(-0.5f, -0.4f, 0.0f, VertexColor(1.0f, 0.0f, 0.0f));
                Vertex v2(0.0f, -0.4f, 0.0f, VertexColor(1.0f, 1.0f, 0.0f));
                Vertex v3(0.2f, -0.4f, 0.0f, VertexColor(1.0f, 0.0f, 1.0f));
                s_context->triangle(surface, nullptr, &v1, &v2, &v3);
            }

            {
                // Draw a triangle with vertices in a vertical line
                Vertex v1(0.5f, 0.6f, 0.0f, VertexColor(1.0f, 0.0f, 0.0f));
                Vertex v2(0.5f, -0.6f, 0.0f, VertexColor(1.0f, 1.0f, 0.0f));
                Vertex v3(0.5f, 0.1f, 0.0f, VertexColor(1.0f, 0.0f, 1.0f));
                s_context->triangle(surface, nullptr, &v1, &v2, &v3);
            }

            {
                // Draw a triangle with vertices in a diagonal line
                Vertex v1(-0.5f, -0.6f, 0.0f, VertexColor(1.0f, 0.0f, 0.0f));
                Vertex v2(-0.1f, -0.2f, 0.0f, VertexColor(1.0f, 1.0f, 0.0f));
                Vertex v3(-0.3f, -0.4f, 0.0f, VertexColor(1.0f, 0.0f, 1.0f));
                s_context->triangle(surface, nullptr, &v1, &v2, &v3);
            }

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
        fprintf(stderr, "usage: triangle_test <test number>\n");
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
        if (!top)
            throw Exception("NULL ITop");
        ISurface* surface = top->createSurface(PF_RGB_888, IMAGE_WIDTH, IMAGE_HEIGHT);
        if (!surface)
            throw Exception("NULL ISurface");
        s_zBuffer = top->createZBuffer(IMAGE_WIDTH, IMAGE_HEIGHT);
        if (!s_zBuffer)
            throw Exception("NULL IZBuffer");
        s_context = top->createDrawingContext();
        if (!s_context)
            throw Exception("NULL IDrawingContext");

        Viewer* viewer = Viewer::createViewer(surface);
        if (!viewer)
            throw Exception("NULL Viewer");

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
