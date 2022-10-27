#pragma once

#include <cinolib/gl/glcanvas.h>
#include <cinolib/gl/key_bindings.hpp>
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

		static struct
		{
			static constexpr cinolib::KeyBinding addChild{ GLFW_KEY_A };
			static constexpr cinolib::KeyBinding toggleSolidMode{ GLFW_KEY_U };
			static constexpr cinolib::KeyBinding editDimX{ GLFW_KEY_X };
			static constexpr cinolib::KeyBinding editDimY{ GLFW_KEY_Y };
			static constexpr cinolib::KeyBinding editDimZ{ GLFW_KEY_Z };
			static constexpr cinolib::KeyBinding actNextChild{ GLFW_KEY_DOWN };
			static constexpr cinolib::KeyBinding actPrevChild{ GLFW_KEY_UP };
			static constexpr cinolib::KeyBinding actNextVert{ GLFW_KEY_RIGHT };
			static constexpr cinolib::KeyBinding actPrevVert{ GLFW_KEY_LEFT };
			static constexpr cinolib::KeyBinding actVert0{ GLFW_KEY_0 };
			static constexpr cinolib::KeyBinding actVert1{ GLFW_KEY_1 };
			static constexpr cinolib::KeyBinding actVert2{ GLFW_KEY_2 };
			static constexpr cinolib::KeyBinding actVert3{ GLFW_KEY_3 };
			static constexpr cinolib::KeyBinding actVert4{ GLFW_KEY_4 };
			static constexpr cinolib::KeyBinding actVert5{ GLFW_KEY_5 };
			static constexpr cinolib::KeyBinding actVert6{ GLFW_KEY_6 };
			static constexpr cinolib::KeyBinding actVert7{ GLFW_KEY_7 };
			static constexpr cinolib::KeyBinding toggleSingleMode{ GLFW_KEY_S };
			static constexpr cinolib::KeyBinding toggleCursor{ GLFW_KEY_H };
			static constexpr cinolib::KeyBinding setVertAndNextVert{ GLFW_KEY_E };
			static constexpr cinolib::KeyBinding setVert{ GLFW_KEY_W };
			static constexpr cinolib::KeyBinding cubeActChild{ GLFW_KEY_Q };
			static constexpr cinolib::KeyBinding addCursorChildGrid{ GLFW_KEY_A, GLFW_MOD_SHIFT };
			static constexpr cinolib::KeyBinding shrinkCursor{ GLFW_KEY_LEFT, GLFW_MOD_ALT };
			static constexpr cinolib::KeyBinding expandCursor{ GLFW_KEY_RIGHT, GLFW_MOD_ALT };
			static constexpr cinolib::KeyBinding translateCursorPlus{ GLFW_KEY_UP, GLFW_MOD_ALT };
			static constexpr cinolib::KeyBinding translateCursorMinus{ GLFW_KEY_DOWN, GLFW_MOD_ALT };
			static constexpr cinolib::KeyBinding removeShown{ GLFW_KEY_X, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding translateShownPlus{ GLFW_KEY_UP, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding translateShownMinus{ GLFW_KEY_DOWN, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding selActChild{ GLFW_KEY_Q, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding cloneShown{ GLFW_KEY_D, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding hideShown{ GLFW_KEY_H, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding rotateShown{ GLFW_KEY_R, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding flipShown{ GLFW_KEY_F, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding selAll{ GLFW_KEY_A, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding save{ GLFW_KEY_S, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding open{ GLFW_KEY_O, GLFW_MOD_CONTROL };
			static constexpr cinolib::KeyBinding deselAll{ GLFW_KEY_A, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT };
			static constexpr cinolib::KeyBinding deselActChild{ GLFW_KEY_Q, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT };
			static constexpr cinolib::KeyBinding saveNew{ GLFW_KEY_S, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT };
		} kb;

		static struct
		{
			static constexpr int modSetVert{ GLFW_MOD_CONTROL };
			static constexpr int modActChild{ GLFW_MOD_SHIFT };
			static constexpr int modActActChildVert{ GLFW_MOD_ALT };
			static constexpr int modActAnotherChild{ GLFW_MOD_ALT | GLFW_MOD_SHIFT };
		} mb;

		static constexpr std::size_t c_selectionMarkerSetInd{ 0 };
		static constexpr std::size_t c_mouseMarkerInd{ 0 };
		static constexpr std::size_t c_selectedVertMarkerInd{ 1 };
		static constexpr std::size_t c_vertsMarkerSetInd{ 1 };

		static void printBindings();

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