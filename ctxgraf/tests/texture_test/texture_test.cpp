/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */

#include "viewer.h"
#include "ctxgraf_pub.h"

#define M_PI       3.14159265358979323846

/**
 * @file texture_test.cpp
 * This is the main program for an executable that tests IDrawingContext
 * texture mapping functionality.
 */

static const unsigned IMAGE_WIDTH = 864;
static const unsigned IMAGE_HEIGHT = 864;

using namespace ctxgraf;

static ITop* s_top = nullptr;
static IDrawingContext* s_context = nullptr;
static unsigned s_testNumber = 0;
static bool s_exceptionSeen = false;

enum TEST_ID
{
    TID_SIMPLE,
    TID_SIMPLE_MODULATE,
    TID_WRAPPING_MODES,
    TID_FILTER_MODES,
    TID_FILTER_MODES_WITH_REPEAT,
    TID_WEIRD_SIZES,
    TID_BIG_CIRCLE,
    TID_BIG_CIRCLE_ROTATING,
    TID_BAD_PARAMETERS,

    TID_TEST_COUNT
};

static const char* s_testStrings[TID_TEST_COUNT] =
{
    "Draw a few objects with decal blending, nearest filtering, and no texture wrapping",
    "Draw a few objects with modulate blending",
    "Draw objects with all three texture wrapping modes",
    "Draw objects with both filtering modes",
    "Draw objects with both filtering modes AND wrapping mode = REPEAT",
    "Draw objects with interesting texture sizes",
    "Draw a big textured circle",
    "Draw a big textured circle that rotates",
    "Ensure that state-setting calls with bad parameters throw exceptions",
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
        vertices[i].s = (float)((cos(angle - startAngle) + 1) / 2.0);
        vertices[i].t = (float)((sin(angle - startAngle) + 1) / 2.0);
        angle += angleDelta;
    }
}

/**
 * Create and return a texture map with a standard test pattern.
 */
static ISurface* makeTestTexture(unsigned width, unsigned height)
{
    ISurface* texture = s_top->createSurface(PF_RGB_888, width, height);

    const Color
        ulColor(255, 0, 0),         // red
        urColor(0, 255, 0),         // green
        llColor(0, 0, 255),         // blue
        lrColor(255, 255, 0),       // yellow
        middleColor(127, 127, 127); // grey

    for (unsigned y = 0; y < height; y++)
    {
        for (unsigned x = 0; x < width; x++)
        {
            if (width >= 4 && height >= 4)
            {
                if (x >= width / 4 && x < width * 3 / 4 && y >= height / 4 && y < height * 3 / 4)
                {
                    texture->drawPixel(x, y, middleColor);
                    continue;
                }
            }

            Color texelColor;
            if (y >= height / 2)
                texelColor = (x >= width / 2 ? lrColor : llColor);
            else
                texelColor = (x >= width / 2 ? urColor : ulColor);
            texture->drawPixel(x, y, texelColor);
        }
    }

    return texture;
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
        surface->clear(Color(15, 47, 15));

        switch (s_testNumber)
        {
        case TID_SIMPLE:
        case TID_SIMPLE_MODULATE:
        case TID_WEIRD_SIZES:
        {
            ISurface* texture = makeTestTexture(16, 16);
            s_context->setTextureMap(texture);
            const bool weirdSizes = (s_testNumber == TID_WEIRD_SIZES);

            if (s_testNumber == TID_SIMPLE_MODULATE)
                s_context->setTextureBlendingMode(TEXTURE_BLENDING_MODE_MODULATE);

            {
                // Draw the axis-oriented square
                Vertex v1(-0.5f, -0.5f, 0.0f, VertexColor(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
                Vertex v2(-0.3f, -0.5f, 0.0f, VertexColor(1.0f, 0.0f, 0.0f), 1.0f, 0.0f);
                Vertex v3(-0.3f, -0.3f, 0.0f, VertexColor(0.0f, 1.0f, 0.0f), 1.0f, 1.0f);
                Vertex v4(-0.5f, -0.3f, 0.0f, VertexColor(0.0f, 0.0f, 1.0f), 0.0f, 1.0f);

                if (weirdSizes)
                    s_context->setTextureMap(makeTestTexture(3, 5));
                s_context->triangle(surface, nullptr, &v1, &v2, &v3);
                s_context->triangle(surface, nullptr, &v1, &v3, &v4);
            }

            {
                // Draw the 45-degree rotated square
                Vertex v1(0.5f, -0.5f, 0.0f, VertexColor(0.8f, 0.8f, 0.8f), 0.0f, 0.0f);
                Vertex v2(0.6f, -0.4f, 0.0f, VertexColor(1.0f, 0.5f, 0.5f), 1.0f, 0.0f);
                Vertex v3(0.5f, -0.3f, 0.0f, VertexColor(0.5f, 0.5f, 0.5f), 1.0f, 1.0f);
                Vertex v4(0.4f, -0.4f, 0.0f, VertexColor(0.1f, 0.1f, 0.1f), 0.0f, 1.0f);

                if (weirdSizes)
                    s_context->setTextureMap(makeTestTexture(7, 12));
                s_context->triangle(surface, nullptr, &v1, &v2, &v3);
                s_context->triangle(surface, nullptr, &v1, &v3, &v4);
            }

            {
                // Draw the long triangle
                Vertex v1(-0.5f, 0.5f, 0.0f, VertexColor(0.6f, 0.6f, 0.6f), 0.0f, 0.0f);
                Vertex v2(0.4f, 0.4f, 0.0f, VertexColor(1.0f, 0.0f, 0.0f), 1.0f, 0.5f);
                Vertex v3(-0.4f, 0.7f, 0.0f, VertexColor(0.0f, 1.0f, 0.0f), 0.0f, 1.0f);

                if (weirdSizes)
                    s_context->setTextureMap(makeTestTexture(111, 42));
                s_context->triangle(surface, nullptr, &v1, &v2, &v3);
            }

            break;
        }

        case TID_WRAPPING_MODES:
        {
            ISurface* texture = makeTestTexture(32, 32);
            s_context->setTextureMap(texture);

            for (unsigned i = 0; i < TEXTURE_WRAPPING_MODE_COUNT; i++)
            {
                s_context->setTextureWrappingMode((TextureWrappingMode)i);

                // Draw a 45-degree rotated square
                Vertex v1(-0.5f + i*0.5f, -0.2f, 0.0f, VertexColor(1.0f, 0.5f, 0.8f), -1.0f, -1.0f);
                Vertex v2(-0.3f + i*0.5f, 0.0f, 0.0f, VertexColor(1.0f, 0.5f, 0.8f), 2.0f, -1.0f);
                Vertex v3(-0.5f + i*0.5f, 0.2f, 0.0f, VertexColor(1.0f, 0.5f, 0.8f), 2.0f, 2.0f);
                Vertex v4(-0.7f + i*0.5f, 0.0f, 0.0f, VertexColor(1.0f, 0.5f, 0.8f), -1.0f, 2.0f);

                s_context->triangle(surface, nullptr, &v1, &v2, &v3);
                s_context->triangle(surface, nullptr, &v1, &v3, &v4);
            }

            break;
        }

        case TID_FILTER_MODES:
        case TID_FILTER_MODES_WITH_REPEAT:
        {
            ISurface* littleTexture = makeTestTexture(2, 2);
            ISurface* biggerTexture = makeTestTexture(8, 8);

            if (s_testNumber == TID_FILTER_MODES_WITH_REPEAT)
                s_context->setTextureWrappingMode(TEXTURE_WRAPPING_MODE_REPEAT);

            for (unsigned i = 0; i < TEXTURE_FILTERING_MODE_COUNT; i++)
            {
                s_context->setTextureFilteringMode((TextureFilteringMode)i);

                {
                    // Draw the axis-oriented square with the 2x2 texture
                    s_context->setTextureMap(littleTexture);

                    Vertex v1(-0.5f, -0.5f + i, 0.0f, VertexColor(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
                    Vertex v2(-0.3f, -0.5f + i, 0.0f, VertexColor(1.0f, 0.0f, 0.0f), 1.0f, 0.0f);
                    Vertex v3(-0.3f, -0.3f + i, 0.0f, VertexColor(0.0f, 1.0f, 0.0f), 1.0f, 1.0f);
                    Vertex v4(-0.5f, -0.3f + i, 0.0f, VertexColor(0.0f, 0.0f, 1.0f), 0.0f, 1.0f);

                    s_context->triangle(surface, nullptr, &v1, &v2, &v3);
                    s_context->triangle(surface, nullptr, &v1, &v3, &v4);
                }

                {
                    // Draw the 45-degree rotated square with the 8x8 texture
                    s_context->setTextureMap(biggerTexture);

                    Vertex v1(0.5f, -0.5f + i, 0.0f, VertexColor(1.0f, 0.5f, 0.8f), 0.0f, 0.0f);
                    Vertex v2(0.6f, -0.4f + i, 0.0f, VertexColor(1.0f, 0.5f, 0.8f), 1.0f, 0.0f);
                    Vertex v3(0.5f, -0.3f + i, 0.0f, VertexColor(1.0f, 0.5f, 0.8f), 1.0f, 1.0f);
                    Vertex v4(0.4f, -0.4f + i, 0.0f, VertexColor(1.0f, 0.5f, 0.8f), 0.0f, 1.0f);

                    s_context->triangle(surface, nullptr, &v1, &v2, &v3);
                    s_context->triangle(surface, nullptr, &v1, &v3, &v4);
                }
            }
            break;
        }

        case TID_BIG_CIRCLE:
        case TID_BIG_CIRCLE_ROTATING:
        {
            static const double RADIUS = 0.75;
            static const unsigned PERIMETER_COUNT = 36;
            static const float FRAME_ANGLE_DELTA = 0.1f;

            Vertex perimeter[PERIMETER_COUNT];
            Vertex center(0.0f, 0.0f, 0.0f, VertexColor(1.0f, 1.0f, 1.0f), 0.5f, 0.5f);
            double startAngle = (s_testNumber == TID_BIG_CIRCLE_ROTATING ? frame * FRAME_ANGLE_DELTA : 0.1);

            setCircularVertexPattern(perimeter, PERIMETER_COUNT, RADIUS, 2.0f * M_PI / PERIMETER_COUNT, startAngle);

            s_context->setTextureMap(makeTestTexture(16, 16));
            for (unsigned i = 0; i < PERIMETER_COUNT - 1; i++)
                s_context->triangle(surface, nullptr, perimeter + i, perimeter + i + 1, &center);
            s_context->triangle(surface, nullptr, perimeter + PERIMETER_COUNT - 1, perimeter, &center);

            break;
        }

        case TID_BAD_PARAMETERS:
        {
            bool missedException = false, wrongException = false;
            s_context->setTextureMap(makeTestTexture(16, 16));

            try
            {
                s_context->setTextureBlendingMode(TEXTURE_BLENDING_MODE_COUNT);
                fprintf(stderr, "setTextureBlendingMode(COUNT) didn't throw an exception\n");
                missedException = true;
            }
            catch (ParameterException& e)
            {
                printf("good exception: %s\n", e.what());
            }
            catch (...)
            {
                fprintf(stderr, "setTextureBlendingMode(COUNT) threw the wrong type of exception\n");
                wrongException = true;
            }

            try
            {
                s_context->setTextureFilteringMode(TEXTURE_FILTERING_MODE_COUNT);
                fprintf(stderr, "setTextureFilteringMode(COUNT) didn't throw an exception\n");
                missedException = true;
            }
            catch (ParameterException& e)
            {
                printf("good exception: %s\n", e.what());
            }
            catch (...)
            {
                fprintf(stderr, "setTextureFilteringMode(COUNT) threw the wrong type of exception\n");
                wrongException = true;
            }

            try
            {
                s_context->setTextureWrappingMode(TEXTURE_WRAPPING_MODE_COUNT);
                fprintf(stderr, "setTextureWrappingMode(COUNT) didn't throw an exception\n");
                missedException = true;
            }
            catch (ParameterException& e)
            {
                printf("good exception: %s\n", e.what());
            }
            catch (...)
            {
                fprintf(stderr, "setTextureWrappingMode(COUNT) threw the wrong type of exception\n");
                wrongException = true;
            }

            s_context->setTextureMap(nullptr);
            VertexColor vcolor = missedException ? VertexColor(1.0f, 0.0f, 0.0f) : wrongException ? VertexColor(1.0f, 1.0f, 0.0f) : VertexColor(0.0f, 1.0f, 0.0f);
            const Vertex v1(0.0f, -0.3f, 0.0f, vcolor);
            const Vertex v2(0.2f, 0.3f, 0.0f, vcolor);
            const Vertex v3(-0.2f, 0.3f, 0.0f, vcolor);
            s_context->triangle(surface, nullptr, &v1, &v2, &v3);

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
        fprintf(stderr, "usage: texture_test <test number>\n");
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
        s_top = getTop();
        if (!s_top)
            throw Exception("NULL ITop");
        ISurface* surface = s_top->createSurface(PF_RGB_888, IMAGE_WIDTH, IMAGE_HEIGHT);
        if (!surface)
            throw Exception("NULL ISurface");
        s_context = s_top->createDrawingContext();
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
