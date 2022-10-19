#pragma once

#include <cinolib/gl/glcanvas.h>
#include <cinolib/drawable_segment_soup.h>
#include <RSE/AxesGuiItem.hpp>
#include <RSE/Grid.hpp>
#include <RSE/AppSidebarItem.hpp>
#include <cstddef>

namespace RSE
{

	class App final
	{

	private:

		cinolib::GLcanvas m_canvas;
		cinolib::DrawableSegmentSoup m_gridObj;
		AxesGuiItem m_axesWidget;
		AppSidebarItem m_appWidget;
		Grid m_grid;

		void onGridUpdate();
		void onChildUpdate(std::size_t _child);
		void onChildVisibilityUpdate(std::size_t _child);
		void onChildAdd();
		void onChildrenClear();
		void onChildRemove(std::size_t _child);
		void onSetVert();

	public:

		App();

		int launch();

	};

}