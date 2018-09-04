/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */

#include "surface.h"
#include <stdio.h>


namespace ctxgraf {


Surface::Surface(PixelFormat format, uint32_t width, uint32_t height)
    : m_surface(NULL)
    , m_format(format)
    , m_width(width)
    , m_height(height)
    , m_pitch(0)
{
    if (width == 0 || height == 0)
        throw ParameterException("invalid width or height");
    if (format >= PF_COUNT)
        throw NotImplementedException("unsupported pixel format");

    m_pitch = width * bytesPerPixel();
    uint32_t size = m_pitch * height;

    m_surface = new uint8_t[size];
}

Surface::~Surface()
{
    delete[] m_surface;
}

void Surface::clear(Color clearColor)
{
    for (uint32_t y = 0; y < m_height; y++)
    {
        for (uint32_t x = 0; x < m_width; x++)
        {
            drawPixel(x, y, clearColor);
        }
    }
}

void Surface::drawPixel(uint32_t x, uint32_t y, Color pixelColor)
{
    if (x < m_width && y < m_height)
    {
        uint8_t* loc = m_surface + (y * m_pitch) + (x * bytesPerPixel());
        *loc++ = pixelColor.red;
        *loc++ = pixelColor.green;
        *loc++ = pixelColor.blue;
    }
}

Color Surface::getPixel(uint32_t x, uint32_t y) const
{
    if (x >= m_width || y >= m_height)
    {
        char msg[256];
        snprintf(msg, sizeof(msg), "illegal coordinates to getPixel: (%u,%u)\n", x, y);
        throw ParameterException(msg);
    }

    Color result;
    uint8_t* loc = m_surface + (y * m_pitch) + (x * bytesPerPixel());
    result.red   = *loc++;
    result.green = *loc++;
    result.blue  = *loc++;

    return result;
}


void Surface::bitBlt(uint32_t width, uint32_t height,
    uint32_t dstX, uint32_t dstY,
    const ISurface* src, uint32_t srcX, uint32_t srcY,
    uint8_t rop)
{
    if (rop > 15)
        throw ParameterException("Bad bitBlt rop parameter");
    if (rop != BITBLT_ROP_SRCCOPY && rop != BITBLT_ROP_BLACKNESS && rop != BITBLT_ROP_DSTINVERT && rop != BITBLT_ROP_WHITENESS)
        throw NotImplementedException("Unsupported rop");

    const bool srcRequired = (rop == BITBLT_ROP_SRCCOPY);
    if (srcRequired && !src)
        throw ParameterException("BitBlt source required, but source parameter is NULL");

    enum CopyDirection
    {
        CD_TOP_TO_BOTTOM,
        CD_BOTTOM_TO_TOP,
        CD_RIGHT_TO_LEFT,
    } copyDirection = CD_TOP_TO_BOTTOM;

    if (srcRequired && src == this && rectanglesOverlap(width, height, dstX, dstY, srcX, srcY))
    {
        if (dstY > srcY)
            copyDirection = CD_BOTTOM_TO_TOP;
        else if (dstY == srcY && dstX > srcX)
            copyDirection = CD_RIGHT_TO_LEFT;
    }

    // Clipping
    if (dstX > m_width)
        width = 0;
    else if (width > m_width - dstX)
        width = m_width - dstX;

    if (dstY > m_height)
        height = 0;
    else if (height > m_height - dstY)
        height = m_height - dstY;

    if (srcRequired)
    {
        if (srcX > m_width)
            width = 0;
        else if (width > m_width - srcX)
            width = m_width - srcX;

        if (srcY > m_height)
            height = 0;
        else if (height > m_height - srcY)
            height = m_height - srcY;
    }

    switch (rop)
    {
    case BITBLT_ROP_SRCCOPY:
        switch (copyDirection)
        {
        case CD_TOP_TO_BOTTOM:
            for (uint32_t y = 0; y < height; y++)
            {
                for (uint32_t x = 0; x < width; x++)
                {
                    drawPixel(dstX + x, dstY + y, src->getPixel(srcX + x, srcY + y));
                }
            }
            break;

        case CD_BOTTOM_TO_TOP:
            for (int32_t y = height - 1; y >= 0; y--)
            {
                for (uint32_t x = 0; x < width; x++)
                {
                    drawPixel(dstX + x, dstY + y, src->getPixel(srcX + x, srcY + y));
                }
            }
            break;

        case CD_RIGHT_TO_LEFT:
            for (uint32_t y = 0; y < height; y++)
            {
                for (int32_t x = width - 1; x >= 0; x--)
                {
                    drawPixel(dstX + x, dstY + y, src->getPixel(srcX + x, srcY + y));
                }
            }
            break;

        default:
            throw BadStateException("Bad CopyDirection");
        }
        break;

    case BITBLT_ROP_BLACKNESS:
    case BITBLT_ROP_WHITENESS:
    {
        const Color color = (rop == BITBLT_ROP_BLACKNESS ? Color(0, 0, 0) : Color(255, 255, 255));

        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                drawPixel(dstX + x, dstY + y, color);
            }
        }
        break;
    }

    case BITBLT_ROP_DSTINVERT:
    {
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                Color dstColor = getPixel(dstX + x, dstY + y);
                dstColor.red   = ~dstColor.red;
                dstColor.green = ~dstColor.green;
                dstColor.blue  = ~dstColor.blue;
                drawPixel(dstX + x, dstY + y, dstColor);
            }
        }
        break;
    }

    default:
        throw BadStateException("Illegal rop in switch");
    }
}

void* Surface::getStart()
{
    return static_cast<void*>(m_surface);
}

const void* Surface::getStart() const
{
    return static_cast<const void*>(m_surface);
}

uint32_t Surface::getWidth() const
{
    return m_width;
}

uint32_t Surface::getHeight() const
{
    return m_height;
}

uint32_t Surface::getPitch() const
{
    return m_pitch;
}

PixelFormat Surface::getFormat() const
{
    return m_format;
}

void Surface::clear(uint32_t clearValue)
{
    for (uint32_t y = 0; y < m_height; y++)
    {
        for (uint32_t x = 0; x < m_width; x++)
        {
            setZ(x, y, clearValue);
        }
    }
}

void Surface::setZ(uint32_t x, uint32_t y, uint32_t zValue)
{
    if (x >= m_width || y >= m_height)
        return;
    if (zValue > 0xFFFF)
        throw ParameterException("Z value is too big in setZ");

    uint16_t* loc = (uint16_t*)(m_surface + (y * m_pitch) + (x * bytesPerPixel()));
    *loc = zValue;
}

uint32_t Surface::getZ(uint32_t x, uint32_t y) const
{
    if (x >= m_width || y >= m_height)
    {
        char msg[256];
        snprintf(msg, sizeof(msg), "illegal coordinates to getZ: (%u,%u)\n", x, y);
        throw ParameterException(msg);
    }

    uint16_t* loc = (uint16_t*)(m_surface + (y * m_pitch) + (x * bytesPerPixel()));
    return *loc;
}

unsigned Surface::bytesPerPixel() const
{
    switch (m_format)
    {
        case PF_RGB_888:    return 3;
        case PF_RGBA_8888:  return 4;
        case PF_Z16:        return 2;
        default:
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Unknown pixel format: %d\n", m_format);
            throw ParameterException(msg);
        }
    }
}

/*static*/
bool Surface::rectanglesOverlap(uint32_t width, uint32_t height,
                                uint32_t dstX, uint32_t dstY,
                                uint32_t srcX, uint32_t srcY)
{
    bool xOverlap = (dstX >= srcX && dstX < srcX + width)  ||
                    (dstX + width - 1 >= srcX && dstX + width - 1 < srcX + width);
    bool yOverlap = (dstY >= srcY && dstY < srcY + height) ||
                    (dstY + height - 1 >= srcY && dstY + height - 1 < srcY + height);

    return xOverlap && yOverlap;
}

} // namespace ctxgraf
