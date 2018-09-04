#include <stdio.h>

#include "viewer.h"
#include "ctxgraf_pub.h"

/**
 * @file main.cpp
 * This is the main program for an executable that tests the Viewer class,
 * and (trivially) the ctxgraf implementation.
 */

static const unsigned IMAGE_WIDTH = 640;
static const unsigned IMAGE_HEIGHT = 480;


/**
 * The function that is called to draw each frame.
 * This particular function draws a diagonal line whose (constant) color changes
 * over time, and a square with changing colors.
 *
 * @param[in] surface The surface to draw onto.
 * @param[in] frame The frame number, which begins at zero and increases by one
 * for each successive frame.
 */
static void drawTexture(ctxgraf::ISurface* surface, unsigned frame)
{
    surface->clear(ctxgraf::Color(0, 0, 0));

    ctxgraf::Color lineColor(255, frame % 255, (frame/2) % 255);
    for (int i = 0; i < IMAGE_HEIGHT; i++)
        surface->drawPixel(i, i, lineColor);

    for (unsigned y = 100; y < 200; y++)
    {
        for (unsigned x = 100; x < 200; x++)
        {
            ctxgraf::Color color((x+y+frame)%255, (y<=150)?255:63, ((x+frame)%300<=150)?255:127);
            surface->drawPixel(x, y, color);
        }
    }
}

int main(int argc, char** argv)
{
    printf("Hello, world\n");

	try
	{
		ctxgraf::ITop* top = ctxgraf::getTop();
		ctxgraf::ISurface* surface = top->createSurface(ctxgraf::PF_RGB_888, IMAGE_WIDTH, IMAGE_HEIGHT);
		ctxgraf::Viewer* viewer = ctxgraf::Viewer::createViewer(surface);

		viewer->start(drawTexture);
		return 0;
	}
	catch (ctxgraf::Exception& ex)
	{
		fprintf(stderr, "ctxgraf error: %s\n", ex.what());
		getchar();
	}
	catch (std::exception& ex)
	{
		fprintf(stderr, "unknown error: %s\n", ex.what());
		getchar();
	}

	return 1;
}
