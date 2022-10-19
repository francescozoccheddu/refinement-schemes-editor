#include <RSE/App.hpp>

#include <cinolib/color.h>

namespace RSE
{

	App::App() : m_canvas{}, m_axesWidget{ m_canvas.camera }, m_appWidget{}
	{
		m_canvas.background = cinolib::Color::hsv2rgb(0.0f, 0.0f, 0.1f);
		m_canvas.push(&m_axesWidget);
		m_canvas.push(&m_appWidget);
		m_canvas.show_sidebar(true);
		m_appWidget.show_open = true;
	}

	int App::launch()
	{
		return m_canvas.launch();
	}

}