#pragma once

#include <cinolib/gl/glcanvas.h>

namespace RSE {

    class App final
    {

    private:

        cinolib::GLcanvas m_canvas;

    public:

        App();

        int launch();

    };

}