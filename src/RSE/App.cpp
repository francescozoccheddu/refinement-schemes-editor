#include <RSE/App.hpp>

#include <cinolib/color.h>
#include <glfw/glfw3.h>
#include <type_traits>

namespace RSE
{

	void App::onGridUpdate()
	{
		m_grid.make(m_appWidget.sourceSize(), m_appWidget.sourceControl().verts());
		const Int layers{ m_appWidget.sourceSize() + 1 };
		m_gridObj.resize(static_cast<std::size_t>(layers * layers * 3 * 2));
		std::size_t i{};
		for (std::size_t d{}; d < 3; d++)
		{
			IVec3 coord;
			for (Int x{}; x < layers; x++)
			{
				coord[(d + 2) % 3] = x;
				for (Int y{}; y < layers; y++)
				{
					coord[(d + 1) % 3] = y;
					for (Int z{}; z < 2; z++)
					{
						coord[d] = z ? layers - 1 : 0;
						m_gridObj[i++] = m_grid.get(coord);
					}
				}
			}
		}
		m_gridObj.update_bbox();
		for (std::size_t i{}; i < m_appWidget.childControls().size(); i++)
		{
			onChildUpdate(i);
		}
	}

	void App::onChildUpdate(std::size_t _child) {}
	void App::onChildVisibilityUpdate(std::size_t _child) {}
	void App::onChildAdd() {}
	void App::onChildrenClear() {}
	void App::onChildRemove(std::size_t _child) {}

	void App::onSetVert() {
	}

	App::App() : m_canvas{}, m_axesWidget{ m_canvas.camera }, m_appWidget{}, m_grid{}
	{
		onGridUpdate();
		m_canvas.background = cinolib::Color::hsv2rgb(0.0f, 0.0f, 0.1f);
		m_canvas.push(&m_axesWidget);
		m_canvas.push(&m_appWidget);
		m_canvas.show_sidebar(true);
		m_canvas.push(&m_gridObj);
		m_appWidget.show_open = true;
		m_appWidget.onSourceUpdate += [this]() { onGridUpdate(); };
		m_gridObj.set_color(cinolib::Color::WHITE());
		m_gridObj.set_cheap_rendering(true);
		m_gridObj.set_thickness(1);
		m_canvas.callback_key_pressed = [this](int _key, int _modifiers) {
			switch (_key)
			{
				case GLFW_KEY_E:
					onSetVert();
					return true;
			}
			return false;
		};
	}

	int App::launch()
	{
		return m_canvas.launch();
	}

}