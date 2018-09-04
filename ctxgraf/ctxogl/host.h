/**
* Concordia University CSC3308
*
* Author: Brad Cain
*/


/**
* @file host.h
* The declaration of the Host class.
*/


namespace ctxogl
{

/**
 * The signature of the drawing callback used by the Host class.
 * The client must pass a pointer to a function of this type to the Host::start() method.
 *
 * @param[in] frameCounter The number of times this function has been previously called
 * since the start() call.
 * @param[in] aspectRatio The aspect ratio (width/height) of the drawing surface.
 */
typedef void DrawFunction(unsigned frameCounter, float aspectRatio);

typedef void KeyFunction(unsigned char key, int x, int y, bool shift, bool ctrl);

/**
 * The signature of the mouse movement callback used by the Host class.
 * When the user holds down the left mouse button, every movement will be indicated
 * by a call to this function.
 *
 * @param[in] x The X coordinate of the mouse position, relative to the window origin.
 * @param[in] y The Y coordinate of the mouse position, relative to the window origin.
 */
typedef void MouseFunction(int x, int y);

class __declspec(dllexport) Host
{
public:

    Host(unsigned width, unsigned height, const char* windowName="CTX OGL");
    ~Host();

    /**
     * Begin drawing.
     * This will result in repeated calls to drawFunction.
     *
     * @param[in] drawFunction The drawing callback function.
     * @param[in] keyFunction The keyboard callback function (optional).
     * @param[in] mouseFunction The mouse movement callback function (optional).
     */
    void start(DrawFunction* drawFunction, KeyFunction* keyFunction=nullptr, MouseFunction* mouseFunction = nullptr);

    /**
     * Stop drawing.
     * The window will remain in place, containing its last drawn contents.
     */
    void stop();

    /**
     * Make the drawing callback that's currently assigned.
     * If no calllback is available, do nothing.
     */
    void makeDrawingCallback();

    /**
     * Make the keyboard callback that's currently assigned.
     * If no callback is available, do nothing.
     */
    void makeKeyboardCallback(unsigned char key, int x, int y, bool shift, bool ctrl);

private:

    unsigned m_width;               ///< Width of the drawing surface, in pixels
    unsigned m_height;              ///< Height of the drawing surface, in pixels
    unsigned m_frameCounter;        ///< The number of times m_drawFunction has been called since start()
    DrawFunction* m_drawFunction;   ///< The drawing callback function (provided by the client)
    KeyFunction* m_keyFunction;     ///< The callback function for keyboard input (provided by the client)
    MouseFunction* m_mouseFunction; ///< The callback function for mouse input (provided by the client)
};


} // namespace ctxogl

