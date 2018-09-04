/**
 * Concordia University CSC3308
 *
 * Author: Brad Cain
 */

#ifndef API_INCLUDED
#define API_INCLUDED

#include <stdint.h>
#include <string>


/**
 * @file ctxgraf_pub.h
 *
 * This file contains definitions for all public interfaces in the ctxgraf
 * library.
 */

namespace ctxgraf
{

/**
 * An enumeration for pixel formats recognized by ctxgraf.
 */
enum PixelFormat
{
    PF_RGB_888,      ///< 3 bytes per pixel, byte 0 is red
    PF_RGBA_8888,    ///< 4 bytes per pixel, byte 0 is red, byte 3 is alpha
    PF_Z16,          ///< 2 bytes per pixel, single Z value

    PF_COUNT
};


/**
 * An object representing a single color.
 */
struct Color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;    ///< (may be ignored in some cases)

    Color() : red(0), green(0), blue(0), alpha(255) {}

    Color(uint8_t r, uint8_t g, uint8_t b)
        : red(r), green(g), blue(b), alpha(255)
    {}
};


/**
 * Color and alpha information for a vertex.
 * All values should be in [0.0, 1.0].
 */
struct VertexColor
{
    float red, green, blue; ///< vertex color
    float alpha;            ///< vertex opacity

    VertexColor() : red(0.0f), green(0.0f), blue(0.0f), alpha(1.0f) {}

    VertexColor(float r, float g, float b) : red(r), green(g), blue(b), alpha(1.0f) {}

    void clamp()
    {
        red = (red < 0.0f ? 0.0f : red > 1.0f ? 1.0f : red);
        green = (green < 0.0f ? 0.0f : green > 1.0f ? 1.0f : green);
        blue = (blue < 0.0f ? 0.0f : blue > 1.0f ? 1.0f : blue);
    }
};


/**
 * An object representing a vertex.
 * For now, the location is in viewport coordinates; if any coordinate
 * is outside [-1.0, 1.0], the vertex is outside of the view frustrum.
 * Colors and alpha are in [0.0, 1.0].
 */
struct Vertex
{
    float x, y, z;      ///< location
    VertexColor color;  ///< color and opacity
    float xn, yn, zn;   ///< normal
    float s, t;         ///< texture coordinates

    Vertex()
        : x(0.0f), y(0.0f), z(0.0f)
        , xn(0.0f), yn(0.0f), zn(0.0f)
        , s(0.0f), t(0.0f)
    {}

    Vertex(float _x, float _y, float _z, const VertexColor& _color, float _s=0.0f, float _t=0.0f)
        : x(_x), y(_y), z(_z), color(_color)
        , xn(0.0f), yn(0.0f), zn(0.0f)
        , s(_s), t(_t)
    {}
};


/* A few well-known ROPs for the bitBlt() method */

static const uint8_t BITBLT_ROP_BLACKNESS = 0x0;
static const uint8_t BITBLT_ROP_DSTINVERT = 0x5;
static const uint8_t BITBLT_ROP_SRCCOPY   = 0xC;
static const uint8_t BITBLT_ROP_WHITENESS = 0xF;


/**
 * Base class for all (non-runtime) exceptions thrown by ctxgraf
 */
class Exception: public std::exception
{
public:

    explicit Exception(const char* message) : m_message(message) {}
    virtual ~Exception() {}

    virtual const char* what() const noexcept { return m_message.c_str(); }

protected:

    std::string m_message;
};

/**
 * ctxgraf exception indicating an invalid parameter
 */
class ParameterException: public Exception
{
public:

    explicit ParameterException(const char* message) : Exception(message) {}
};

/**
 * ctxgraf exception indicating an unimplemented method
 */
class NotImplementedException: public Exception
{
public:

    explicit NotImplementedException(const char* message) : Exception(message) {}
};

/**
 * ctxgraf exception indicating an internal failure
 */
class InternalException: public Exception
{
public:

    explicit InternalException(const char* message) : Exception(message) {}
};

/**
 * ctxgraf exception indicating an operation attempted in an incorrect state
 */
class BadStateException: public Exception
{
public:

    explicit BadStateException(const char* message) : Exception(message) {}
};



/**
 * The interface to a surface object.
 * A surface object contains a block of memory for surface data (pixels),
 * plus metadata describing the surface.  The surface memory will be released
 * when the object is destroyed.
 */
class ISurface
{
public:

    virtual ~ISurface() {}

    /**
     * Set every pixel in the surface to clearColor.
     */
    virtual void clear(Color clearColor) = 0;

    /**
     * Set the pixel at (x,y) to pixelColor.
     * If (x,y) is outside the surface boundaries, do nothing.
     */
    virtual void drawPixel(uint32_t x, uint32_t y, Color pixelColor) = 0;

    /**
     * Return the color value of the pixel at (x,y).
     *
     * @throw BadParameterException if (x,y) is outside the surface boundaries.
     */
    virtual Color getPixel(uint32_t x, uint32_t y) const = 0;

    /**
     * Perform a BitBlt operation with this surface as the destination.
     *
     * @param[in] width Width of the blitted region, in pixels.
     * @param[in] height Height of the blitted region, in pixels.
     * @param[in] dstX X coordinate of the upper-left corner of the
     * destination rectangle.
     * @param[in] dstY Y coordinate of the upper-left corner of the
     * destination rectangle.
     * @param[in] src The source surface for the blit.
     * May be NULL if the rop does not require source data.
     * @param[in] srcX X coordinate of the upper-left corner of the
     * source rectangle.
     * @param[in] srcY Y coordinate of the upper-left corner of the
     * source rectangle.
     * @param[in] rop The raster operation for the blit.
     * Valid values are between 0 and 15, inclusive.
     *
     * @throws ParameterException if src is NULL (assuming that src is needed),
     * or rop is invalid.
     * @throws NotImplementedException if rop is unsupported by the implementation.
     */
    virtual void bitBlt(uint32_t width, uint32_t height,
                        uint32_t dstX, uint32_t dstY,
                        const ISurface* src, uint32_t srcX, uint32_t srcY,
                        uint8_t rop) = 0;

    /**
     * Return a pointer to surface memory.
     */
    virtual void* getStart() = 0;

    /**
     * Return a pointer to surface memory (const version).
     */
    virtual const void* getStart() const = 0;

    /**
     * Return the width of the surface (in pixels).
     */
    virtual uint32_t getWidth() const = 0;

    /**
     * Return the height of the surface (in pixels).
     */
    virtual uint32_t getHeight() const = 0;

    /**
     * Return the pitch of the surface (in bytes).
     */
    virtual uint32_t getPitch() const = 0;

    /**
     * Return the format of the pixels in the surface.
     */
    virtual PixelFormat getFormat() const = 0;

protected:

    /**
     * Disallow external creation of an object of this type.
     * (Objects of this type should never be created anyway; implementation
     * classes should inherit from this class.)
     */
    ISurface() {}
};

/**
 * The interface to a Z buffer object.
 * A Z buffer contains a block of memory for surface data (Z values),
 * plus metadata describing the surface.  The surface memory will be released
 * when the object is destroyed.
 */
class IZBuffer
{
public:

    virtual ~IZBuffer() {}

    /**
    * Set every pixel in the surface to clearValue.
    */
    virtual void clear(uint32_t clearValue) = 0;

    /**
    * Set the pixel at (x,y) to zValue.
    * If (x,y) is outside the surface boundaries, do nothing.
    */
    virtual void setZ(uint32_t x, uint32_t y, uint32_t zValue) = 0;

    /**
    * Return the Z value of the pixel at (x,y).
    *
    * @throws ParameterException if (x,y) is outside the surface boundaries.
    */
    virtual uint32_t getZ(uint32_t x, uint32_t y) const = 0;

    /**
    * Return the width of the surface (in pixels).
    */
    virtual uint32_t getWidth() const = 0;

    /**
    * Return the height of the surface (in pixels).
    */
    virtual uint32_t getHeight() const = 0;

    /**
    * Return the pitch of the surface (in bytes).
    */
    virtual uint32_t getPitch() const = 0;

    /**
    * Return the format of the pixels in the surface.
    */
    virtual PixelFormat getFormat() const = 0;

protected:

    /**
    * Disallow external creation of an object of this type.
    * (Objects of this type should never be created anyway; implementation
    * classes should inherit from this class.)
    */
    IZBuffer() {}
};


/** Available modes for line shading */
enum LineShadingMode
{
    LINE_SHADING_MODE_CONSTANT,  ///< draw lines with the constant line color
    LINE_SHADING_MODE_SMOOTH,    ///< draw lines with smooth shading between vertex colors

    LINE_SHADING_MODE_COUNT
};

/** Available modes for texture wrapping */
enum TextureWrappingMode
{
    TEXTURE_WRAPPING_MODE_CLAMP,    ///< clamp s and t values to [0,1]
    TEXTURE_WRAPPING_MODE_REPEAT,   ///< s' = s MOD 1, t' = t MOD 1
    TEXTURE_WRAPPING_MODE_MIRROR,   ///< mirror

    TEXTURE_WRAPPING_MODE_COUNT
};

/** Available modes for texture blending */
enum TextureBlendingMode
{
    TEXTURE_BLENDING_MODE_DECAL,    ///< replace pixel color with texture value
    TEXTURE_BLENDING_MODE_MODULATE, ///< multiply pixel color by texture value

    TEXTURE_BLENDING_MODE_COUNT
};

/** Available modes for texture filtering */
enum TextureFilteringMode
{
    TEXTURE_FILTERING_MODE_NEAREST,     ///< point sampling
    TEXTURE_FILTERING_MODE_BILINEAR,    ///< bilinear filtering

    TEXTURE_FILTERING_MODE_COUNT
};

/**
 * The interface to a drawing context.
 * A drawing context contains the logic for drawing 3D primitives (lines and triangles),
 * and all the associated settings for doing so.
 */
class IDrawingContext
{
public:

    virtual ~IDrawingContext() {}

    /**
     * Draw a series of line segments that connect every adjacent pair of vertices.
     * That is, the first line segment will connect vertices[0] and vertices[1],
     * the second will connect vertices[1] and vertices[2], and so on.
     *
     * @param[in] drawingSurface The surface to draw into.
     * @param[in] vertices The array of vertices in the polyline.
     * @param[in] vertexCount The number of vertices in the array.
     *
     * @throws ParameterException if vertices is NULL, or vertexCount < 2.
     */
    virtual void polyline(ISurface* drawingSurface, const Vertex* vertices, uint32_t vertexCount) const = 0;

    /**
     * Set the constant line color.
     * This color is used for all lines if LINE_SHADING_MODE_CONSTANT is being used.
     *
     * @param[in] vertexColor The constant line color.
     */
    virtual void setLineColor(VertexColor lineColor) = 0;

    /**
     * Return the current constant line color.
     */
    virtual VertexColor getLineColor() const = 0;

    /**
     * Set the line shading mode.
     * This mode is used for all polyline() commands until the next call to setLineShadingMode().
     *
     * @param[in] mode The new line shading mode.
     *
     * @throws ParameterException if mode is not a legal LineShadingMode value.
     */
    virtual void setLineShadingMode(LineShadingMode mode) = 0;

    /**
     * Return the current line shading mode.
     */
    virtual LineShadingMode getLineShadingMode() const = 0;

    /**
     * Draw a single triangle.
     *
     * @param[in] drawingSurface The surface to draw into.
     * @param[in] zBuffer The Z buffer to use. (May be null for no Z buffering)
     * @param[in] v1 The first triangle vertex.
     * @param[in] v2 The second triangle vertex.
     * @param[in] v3 The third triangle vertex.
     *
     * @throws ParameterException if any of the parameters except zBuffer is NULL.
     */
    virtual void triangle(ISurface* drawingSurface, IZBuffer* zBuffer, const Vertex* v1, const Vertex* v2, const Vertex* v3) const = 0;

    /**
     * Set the texture map to use for drawing triangles.
     * It defaults to NULL (i.e. no texture map).
     *
     * @throws ParameterException if textureSurface isn't supported as a texture map
     * (for example, if the pixel format isn't acceptable).
     */
    virtual void setTextureMap(ISurface* textureSurface) = 0;

    /**
     * Set the texture wrapping mode.
     * It defaults to TEXTURE_WRAPPING_CLAMP.
     *
     * @throws ParameterException if wrapMode is invalid.
     */
    virtual void setTextureWrappingMode(TextureWrappingMode wrapMode) = 0;

    /**
     * Get the current texture wrapping mode.
     */
    virtual TextureWrappingMode getTextureWrappingMode() const = 0;

    /**
     * Set the texture blending mode.
     * It defaults to TEXTURE_BLENDING_DECAL.
     *
     * @throws ParameterException if blendMode is invalid.
     */
    virtual void setTextureBlendingMode(TextureBlendingMode blendMode) = 0;

    /**
     * Return the current texture blending mode.
     */
    virtual TextureBlendingMode getTextureBlendingMode() const = 0;

    /**
     * Set the texture filtering mode.
     * It defaults to TEXTURE_FILTERING_NEAREST.
     *
     * @throws ParameterException if filterMode is invalid.
     */
    virtual void setTextureFilteringMode(TextureFilteringMode filterMode) = 0;

    /**
     * Return the current texture filtering mode.
     */
    virtual TextureFilteringMode getTextureFilteringMode() const = 0;

protected:

    /**
    * Disallow external creation of an object of this type.
    * (Objects of this type should never be created anyway; implementation
    * classes should inherit from this class.)
    */
    IDrawingContext() {}
};


/**
 * Top-level object for ctxgraf.
 * This object should be a singleton (that is, there should never be more
 * than one of them in any process.)
 */
class ITop
{
public:

    /**
     * Create and return a new surface object.
     * The caller is responsible for freeing the object when done using it.
     *
     * @param[in] format The pixel format of the new surface.
     * @param[in] width The width of the new surface (in pixels).
     * @param[in] height The height of the new surface (in pixels).
     * @return The newly-created surface object.
     *
     * @throws ParameterException if format is invalid, or width or height
     * is zero.
     */
    virtual ISurface* createSurface(PixelFormat format,
                                    uint32_t width, uint32_t height) = 0;

    /**
     * Create and return a new drawing context object.
     * The caller is responsible for freeing the object when done using it.
     */
    virtual IDrawingContext* createDrawingContext() = 0;

    /**
     * Create and return a new Z buffer object.
     * The caller is responsible for freeing the object when done using it.
     *
     * @param[in] width The width of the new surface (in pixels).
     * @param[in] height The height of the new surface (in pixels).
     * @return The newly-created Z buffer object.
     *
     * @throws ParameterException if width or height is zero.
     */
    virtual IZBuffer* createZBuffer(uint32_t width, uint32_t height) = 0;

protected:

    ITop() {}
    virtual ~ITop() {}
};


/**
 * Function to get the (single) ITop object for this process.
 */
extern "C"
{
    ITop* getTop();
}

} // namespace ctxgraf

#endif // API_INCLUDED
