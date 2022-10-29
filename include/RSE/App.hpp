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

		static constexpr cinolib::KeyBinding c_kbAddChild{ GLFW_KEY_A };
		static constexpr cinolib::KeyBinding c_kbToggleSolidMode{ GLFW_KEY_U };
		static constexpr cinolib::KeyBinding c_kbEditDimX{ GLFW_KEY_X };
		static constexpr cinolib::KeyBinding c_kbEditDimY{ GLFW_KEY_Y };
		static constexpr cinolib::KeyBinding c_kbEditDimZ{ GLFW_KEY_Z };
		static constexpr cinolib::KeyBinding c_kbActNextChild{ GLFW_KEY_DOWN };
		static constexpr cinolib::KeyBinding c_kbActPrevChild{ GLFW_KEY_UP };
		static constexpr cinolib::KeyBinding c_kbActNextVert{ GLFW_KEY_RIGHT };
		static constexpr cinolib::KeyBinding c_kbActPrevVert{ GLFW_KEY_LEFT };
		static constexpr cinolib::KeyBinding c_kbActVert0{ GLFW_KEY_0 };
		static constexpr cinolib::KeyBinding c_kbActVert1{ GLFW_KEY_1 };
		static constexpr cinolib::KeyBinding c_kbActVert2{ GLFW_KEY_2 };
		static constexpr cinolib::KeyBinding c_kbActVert3{ GLFW_KEY_3 };
		static constexpr cinolib::KeyBinding c_kbActVert4{ GLFW_KEY_4 };
		static constexpr cinolib::KeyBinding c_kbActVert5{ GLFW_KEY_5 };
		static constexpr cinolib::KeyBinding c_kbActVert6{ GLFW_KEY_6 };
		static constexpr cinolib::KeyBinding c_kbActVert7{ GLFW_KEY_7 };
		static constexpr cinolib::KeyBinding c_kbToggleSingleMode{ GLFW_KEY_S };
		static constexpr cinolib::KeyBinding c_kbToggleCursor{ GLFW_KEY_H };
		static constexpr cinolib::KeyBinding c_kbSetVertAndNextVert{ GLFW_KEY_E };
		static constexpr cinolib::KeyBinding c_kbSetVert{ GLFW_KEY_W };
		static constexpr cinolib::KeyBinding c_kbCubeActChild{ GLFW_KEY_Q };
		static constexpr cinolib::KeyBinding c_kbAddCursorChildGrid{ GLFW_KEY_A, GLFW_MOD_SHIFT };
		static constexpr cinolib::KeyBinding c_kbShrinkCursor{ GLFW_KEY_LEFT, GLFW_MOD_ALT };
		static constexpr cinolib::KeyBinding c_kbExpandCursor{ GLFW_KEY_RIGHT, GLFW_MOD_ALT };
		static constexpr cinolib::KeyBinding c_kbTranslateCursorPlus{ GLFW_KEY_UP, GLFW_MOD_ALT };
		static constexpr cinolib::KeyBinding c_kbTranslateCursorMinus{ GLFW_KEY_DOWN, GLFW_MOD_ALT };
		static constexpr cinolib::KeyBinding c_kbRemoveShown{ GLFW_KEY_X, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbTranslateShownPlus{ GLFW_KEY_UP, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbTranslateShownMinus{ GLFW_KEY_DOWN, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbSelActChild{ GLFW_KEY_Q, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbCloneShown{ GLFW_KEY_D, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbHideShown{ GLFW_KEY_H, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbRotateShown{ GLFW_KEY_R, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbFlipShown{ GLFW_KEY_F, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbSelAll{ GLFW_KEY_A, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbSave{ GLFW_KEY_S, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbOpen{ GLFW_KEY_O, GLFW_MOD_CONTROL };
		static constexpr cinolib::KeyBinding c_kbDeselAll{ GLFW_KEY_A, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT };
		static constexpr cinolib::KeyBinding c_kbDeselActChild{ GLFW_KEY_Q, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT };
		static constexpr cinolib::KeyBinding c_kbSaveNew{ GLFW_KEY_S, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT };
		static constexpr int c_mbModSetVert{ GLFW_MOD_CONTROL };
		static constexpr int c_mbModActChild{ GLFW_MOD_SHIFT };
		static constexpr int c_mbModActActChildVert{ GLFW_MOD_ALT };
		static constexpr int c_mbModActAnotherChild{ GLFW_MOD_ALT | GLFW_MOD_SHIFT };

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
		bool onClick(int _modifiers);
		void setWindowTitle();

	public:

		App();

		void open(const std::string& _file);

		int launch();

	};

}