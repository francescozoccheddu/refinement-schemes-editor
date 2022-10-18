#include <RSE/App.hpp>
#include <cinolib/color.h>

namespace RSE {

    App::App() : m_canvas{} {
        m_canvas.background = cinolib::Color::hsv2rgb(0.0f, 0.0f, 0.1f);
    }

    int App::launch() {
        return m_canvas.launch();
    }

}