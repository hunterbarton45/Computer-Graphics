#ifndef SURFACE_H_INCLUDED
#define SURFACE_H_INCLUDED

#include "ctxgraf_pub.h"


namespace ctxgraf
{

class Surface: public ISurface, public IZBuffer
{
public:

    Surface(PixelFormat format, uint32_t width, uint32_t height);
    virtual ~Surface();

    // ISurface methods
    virtual void clear(Color clearColor);
    virtual void drawPixel(uint32_t x, uint32_t y, Color pixelColor);
    virtual Color getPixel(uint32_t x, uint32_t y) const;
    virtual void bitBlt(uint32_t width, uint32_t height,
                        uint32_t dstX, uint32_t dstY,
                        const ISurface* src, uint32_t srcX, uint32_t srcY,
                        uint8_t rop);
    virtual void* getStart();
    virtual const void* getStart() const;
    virtual uint32_t getWidth() const;
    virtual uint32_t getHeight() const;
    virtual uint32_t getPitch() const;
    virtual PixelFormat getFormat() const;

    // Extra IZBuffer methods
    virtual void clear(uint32_t clearValue);
    virtual void setZ(uint32_t x, uint32_t y, uint32_t zValue);
    virtual uint32_t getZ(uint32_t x, uint32_t y) const;

protected:

    uint8_t* m_surface;
    PixelFormat m_format;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_pitch;

    /** Return the bytes per pixel required by this object's pixel format (m_format) */
    unsigned bytesPerPixel() const;

    /** Return true iff the two given rectangles overlap. */
    static bool rectanglesOverlap(uint32_t width, uint32_t height,
                                  uint32_t dstX, uint32_t dstY,
                                  uint32_t srcX, uint32_t srcY);
};

} // namespace ctxgraf

#endif // SURFACE_H_INCLUDED
