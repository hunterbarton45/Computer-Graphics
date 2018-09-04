#ifndef VIEWER_H_INCLUDED
#define VIEWER_H_INCLUDED

#include "ctxgraf_pub.h"

namespace ctxgraf
{

class Viewer
{
public:

    static Viewer* createViewer(ISurface* surface);
    ~Viewer();

    typedef void (*RenderCallback)(ISurface* surface, unsigned frame);

    void start(RenderCallback callback);
    void stop();

    ISurface* getSurface() { return m_surface; }
    RenderCallback getCallback() { return m_renderCallback; }

private:

    Viewer(ISurface* surface);

    void launchGlut();
    void stopGlut();

    ISurface* m_surface;
    bool m_running;
    RenderCallback m_renderCallback;
};


} // namespace ctxgraf

#endif // VIEWER_H_INCLUDED
