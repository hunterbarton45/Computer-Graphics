/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */

 /**
  * @file top.cpp
  *

  * This file contains the implementation of the Top class and the getTop() function,
  * which returns the (single) ITop instance for applications to use.
  */

#include "top.h"
#include "surface.h"
#include "drawingContext.h"


namespace ctxgraf {


ISurface* Top::createSurface(PixelFormat format, uint32_t width, uint32_t height)
{
    return new Surface(format, width, height);
}

IDrawingContext* Top::createDrawingContext()
{
    return new DrawingContext();
}

IZBuffer* Top::createZBuffer(uint32_t width, uint32_t height)
{
    return new Surface(PF_Z16, width, height);
}

static ITop* s_top = NULL;

ITop* getTop()
{
    if (!s_top)
        s_top = new Top();

    return s_top;
}

} // namespace ctxgraf

