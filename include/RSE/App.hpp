#pragma once

#include <cinolib/gl/glcanvas.h>
#include <RSE/AxesGuiItem.hpp>
#include <RSE/AppSidebarItem.hpp>

namespace RSE
{

	class App final
	{

	private:


		cinolib::GLcanvas m_canvas;
		AxesGuiItem m_axesWidget;
		AppSidebarItem m_appWidget;

	public:

		App();

		int launch();

	};

}