#pragma once

#include <cinolib/gl/glcanvas.h>
#include <cinolib/drawable_segment_soup.h>
#include <cinolib/meshes/drawable_hexmesh.h>
#include <RSE/AxesGuiItem.hpp>
#include <RSE/Grid.hpp>
#include <RSE/AppSidebarItem.hpp>
#include <cstddef>
#include <vector>
#include <string>

namespace RSE
{

	class App final
	{

	private:

		static constexpr std::size_t c_selectionMarkerSetInd{0};
		static constexpr std::size_t c_mouseMarkerInd{0};
		static constexpr std::size_t c_selectedVertMarkerInd{1};
		static constexpr std::size_t c_vertsMarkerSetInd{1};

		static const HexVerts cubeVerts;
		cinolib::GLcanvas m_canvas;
		cinolib::DrawableSegmentSoup m_gridMesh;
		cinolib::DrawableSegmentSoup m_borderMesh;
		AxesGuiItem m_axesWidget;
		AppSidebarItem m_appWidget;
		std::vector<cinolib::DrawableHexmesh<>*> m_childMeshes;
		Grid m_grid;
		std::vector<unsigned int> vertsOrder;
		std::size_t m_mouseGridIndex{};

		void onGridUpdate();
		void onGridCursorUpdate();
		void onChildUpdate(std::size_t _child);
		void onChildAdd();
		void onChildrenClear();
		void onChildRemove(std::size_t _child);
		void onActiveVertChange();
		void onSetVert();
		void onAdvanceActiveChild(bool _advance);
		void onSetActiveVert(std::size_t _vert);
		void onAdvanceActiveVert(bool _advance);
		void onMouseMove();
		bool onKeyPress(int _key, int _modifiers);
		void onClick(int _modifiers);
		void setWindowTitle();

	public:

		App();

		void open(const std::string& _file);

		int launch();

	};

}