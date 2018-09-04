/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */

#include "viewer.h"
#include "ctxgraf_pub.h"

/**
 * @file bitblt_test.cpp
 * This is the main program for an executable that tests ISurface::bitBlt(),
 * ISurface::drawPixel(), and ISurface::clear()
 */

static const unsigned IMAGE_WIDTH = 640;
static const unsigned IMAGE_HEIGHT = 480;

using namespace ctxgraf;


/** Separate source surface for tests that need it */
static ISurface* s_srcSurface = nullptr;
static unsigned s_testNumber = 0;
static bool s_exceptionSeen = false;

enum TEST_ID
{
    TID_CLEAR,
    TID_DRAW_PIXEL,
    TID_SAME_SURFACE_NO_OVERLAP,
    TID_DIFF_SURFACE,
    TID_OVERLAP_DST_BELOW_SRC,
    TID_OVERLAP_SRC_BELOW_DST,
    TID_OVERLAP_DST_RIGHT_OF_SRC,
    TID_OVERLAP_DST_LEFT_OF_SRC,
    TID_CLIP,
    TID_ZERO_SIZES,
    TID_ALL_ROPS,

    TID_TEST_COUNT
};

static const char* s_testStrings[TID_TEST_COUNT] =
{
    "Clear",
    "DrawPixel",
    "BitBlt: same surface, no overlap",
    "BitBlt: different surfaces",
    "BitBlt: overlap, destination below source",
    "BitBlt: overlap, source below destination",
    "BitBlt: overlap, destination to right of source",
    "BitBlt: overlap, destination to left of source",
    "BitBlt: clip",
    "BitBlt: zero sizes",
    "BitBlt: all rops",
};

/**
 * A utility function to create a bitblt test rectangle.
 */
static void drawTestRectangle(ISurface* surface, uint32_t left, uint32_t top, uint32_t width, uint32_t height)
{
    if (left >= surface->getWidth() || top >= surface->getHeight())
    {
        fprintf(stderr, "Warning: test rectangle out of bounds (%d, %d)\n", left, top);
        return;
    }
    if (width == 0 || height == 0)
    {
        fprintf(stderr, "Warning: test rectangle has zero area (%d x %d)\n", width, height);
        return;
    }

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            uint8_t red = 0, green = 0, blue = 0;

            if (x < 2 || y < 2)
            {
                red = 255;
            }
            else if (x >= width - 2 || y >= height - 2)
            {
                green = 255;
            }
            else if (x < width / 2)
            {
                if (y < height / 2)
                {
                    // White
                    red = 255;
                    green = 255;
                    blue = 255;
                }
                else
                {
                    // Magenta
                    red = 255;
                    blue = 255;
                }
            }
            else
            {
                if (y < height / 2)
                {
                    // Cyan
                    green = 255;
                    blue = 255;
                }
                else
                {
                    // 50% grey
                    red = 127;
                    green = 127;
                    blue = 127;
                }
            }

            surface->drawPixel(x + left, y + top, Color(red, green, blue));
        }
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
        surface->clear(Color(0, 0, 0));

        switch (s_testNumber)
        {
        case TID_CLEAR:
            surface->clear(Color(255, 0, 0));
            break;

        case TID_DRAW_PIXEL:
            for (unsigned y = 0; y < IMAGE_HEIGHT + 2; y += 4)
            {
                for (unsigned x = 0; x < IMAGE_WIDTH + 2; x += 4)
                {
                    uint8_t red = static_cast<uint8_t>(255.0 * x / (IMAGE_WIDTH - 2));
                    uint8_t green = static_cast<uint8_t>(255.0 * y / (IMAGE_HEIGHT - 2));
                    uint8_t blue = static_cast<uint8_t>(255.0 * (IMAGE_WIDTH - x) / IMAGE_WIDTH);
                    Color pixelColor(red, green, blue);

                    surface->drawPixel(x, y, pixelColor);
                    surface->drawPixel(x+1, y, pixelColor);
                    surface->drawPixel(x, y+1, pixelColor);
                    surface->drawPixel(x+1, y+1, pixelColor);
                }
            }
            break;

        case TID_SAME_SURFACE_NO_OVERLAP:
            drawTestRectangle(surface, 32, 32, 200, 150);
            surface->bitBlt(200, 150, 300, 100, surface, 32, 32, BITBLT_ROP_SRCCOPY);
            break;

        case TID_DIFF_SURFACE:
        {
            static const uint32_t WIDTH = 200, HEIGHT = 150;
            drawTestRectangle(s_srcSurface, 10, 20, WIDTH, HEIGHT);
            surface->bitBlt(WIDTH, HEIGHT, 32, 32, s_srcSurface, 10, 20, BITBLT_ROP_SRCCOPY);
            break;
        }

        case TID_OVERLAP_DST_BELOW_SRC:
        {
            static const uint32_t WIDTH = 192, HEIGHT = 192;
            drawTestRectangle(surface, 200, 100, WIDTH, HEIGHT);
            surface->bitBlt(WIDTH, HEIGHT, 220, 110, surface, 200, 100, BITBLT_ROP_SRCCOPY);
            break;
        }

        case TID_OVERLAP_SRC_BELOW_DST:
        {
            static const uint32_t WIDTH = 192, HEIGHT = 192;
            drawTestRectangle(surface, 200, 100, WIDTH, HEIGHT);
            surface->bitBlt(WIDTH, HEIGHT, 180, 90, surface, 200, 100, BITBLT_ROP_SRCCOPY);
            break;
        }

        case TID_OVERLAP_DST_RIGHT_OF_SRC:
        {
            static const uint32_t WIDTH = 192, HEIGHT = 192;
            drawTestRectangle(surface, 200, 100, WIDTH, HEIGHT);
            surface->bitBlt(WIDTH, HEIGHT, 232, 100, surface, 200, 100, BITBLT_ROP_SRCCOPY);
            break;
        }

        case TID_OVERLAP_DST_LEFT_OF_SRC:
        {
            static const uint32_t WIDTH = 192, HEIGHT = 192;
            drawTestRectangle(surface, 200, 100, WIDTH, HEIGHT);
            surface->bitBlt(WIDTH, HEIGHT, 168, 100, surface, 200, 100, BITBLT_ROP_SRCCOPY);
            break;
        }

        case TID_CLIP:
        {
            // To attempt to detect memory corruption, allocate another chunk of memory and look for overwrites
            static const unsigned GUARDBAND_SIZE = 10000;
            uint8_t* guardband = new uint8_t[GUARDBAND_SIZE];
            memset(guardband, 0, GUARDBAND_SIZE);

            static const uint32_t WIDTH = 300, HEIGHT = 300;
            drawTestRectangle(surface, 0, 0, WIDTH, HEIGHT);
            surface->bitBlt(WIDTH, HEIGHT, IMAGE_WIDTH-WIDTH+100, IMAGE_HEIGHT-HEIGHT+100, surface, 0, 0, BITBLT_ROP_SRCCOPY);

            // Now see if the guardband has been corrupted
            for (unsigned i = 0; i < GUARDBAND_SIZE; i++)
            {
                if (guardband[i] != 0)
                {
                    fprintf(stderr, "Guardband error at byte offset %d (value is %d)\n", i, guardband[i]);
                    throw BadStateException("Guardband error in clip test");
                }
            }

            delete[] guardband;
            break;
        }

        case TID_ZERO_SIZES:
        {
            static const uint32_t WIDTH = 192, HEIGHT = 192;
            drawTestRectangle(surface, 200, 100, WIDTH, HEIGHT);
            surface->bitBlt(WIDTH, 0, 500, 110, surface, 200, 100, BITBLT_ROP_SRCCOPY);
            surface->bitBlt(0, HEIGHT, 400, 90, surface, 200, 100, BITBLT_ROP_SRCCOPY);
            surface->bitBlt(0, 0, 201, 101, surface, 200, 100, BITBLT_ROP_SRCCOPY);
            break;
        }

        case TID_ALL_ROPS:
        {
            static const uint32_t WIDTH = 100, HEIGHT = 100;
            surface->clear(Color(127, 127, 127));
            drawTestRectangle(surface, 100, 100, WIDTH, HEIGHT);
            drawTestRectangle(surface, 300, 100, WIDTH, HEIGHT);
            drawTestRectangle(surface, 100, 300, WIDTH, HEIGHT);
            surface->bitBlt(WIDTH, HEIGHT, 300, 300, surface, 100, 100, BITBLT_ROP_SRCCOPY);
            surface->bitBlt(WIDTH, HEIGHT, 100, 100, nullptr, 0, 0, BITBLT_ROP_BLACKNESS);
            surface->bitBlt(WIDTH, HEIGHT, 300, 100, nullptr, 0, 0, BITBLT_ROP_WHITENESS);
            surface->bitBlt(WIDTH, HEIGHT, 100, 300, nullptr, 0, 0, BITBLT_ROP_DSTINVERT);
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
        fprintf(stderr, "usage: bitblt_test <test number>\n");
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
        s_srcSurface = top->createSurface(PF_RGB_888, IMAGE_WIDTH, IMAGE_HEIGHT);
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
