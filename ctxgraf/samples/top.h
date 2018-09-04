/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */

#ifndef TOP_H_INCLUDED
#define TOP_H_INCLUDED

/**
 * @file top.h
 *
 * This file contains the definition of the Top class, which implements
 * the ctxgraf::ITop interface.
 */

#include "ctxgraf_pub.h"


namespace ctxgraf {

class Top: public ITop
{
public:

    Top() {}
    virtual ~Top() {}

    // ITop methods
    virtual ISurface* createSurface(PixelFormat format, uint32_t width, uint32_t height);
    virtual IDrawingContext* createDrawingContext();
    virtual IZBuffer* createZBuffer(uint32_t width, uint32_t height);
};

} // namespace ctxgraf

#endif // TOP_H_INCLUDED

