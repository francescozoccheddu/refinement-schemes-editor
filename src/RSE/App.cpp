#include <RSE/App.hpp>

#include <cinolib/color.h>
#include <cinolib/gl/gl_glfw.h>
#include <type_traits>
#include <string>
#include <RSE/hexUtils.hpp>

namespace RSE
{

	void App::onGridUpdate()
	{
		const Int size{ m_appWidget.source().size() };
		m_grid.make(m_appWidget.source().size(), m_appWidget.source().displ().verts());
		const HexVerts bords{ hexUtils::cubeVerts(m_grid.point(IVec3{0,0,0}), m_grid.point(IVec3{size, size, size})) };
		m_borderMesh.clear();
		// x+, y+, z+
		m_borderMesh.push_seg(bords[0], bords[1]);
		m_borderMesh.push_seg(bords[2], bords[3]);
		m_borderMesh.push_seg(bords[4], bords[5]);
		m_borderMesh.push_seg(bords[6], bords[7]);
		// z+, x+, y+
		m_borderMesh.push_seg(bords[0], bords[4]);
		m_borderMesh.push_seg(bords[1], bords[5]);
		m_borderMesh.push_seg(bords[2], bords[6]);
		m_borderMesh.push_seg(bords[3], bords[7]);
		// y+, x+, z+
		m_borderMesh.push_seg(bords[0], bords[2]);
		m_borderMesh.push_seg(bords[1], bords[3]);
		m_borderMesh.push_seg(bords[4], bords[6]);
		m_borderMesh.push_seg(bords[5], bords[7]);
		m_borderMesh.update_bbox();
		for (std::size_t i{}; i < m_appWidget.children().size(); i++)
		{
			onChildUpdate(i);
		}
		m_mouseGridIndex = 0;
		m_canvas.marker_sets[c_selectionMarkerSetInd][c_mouseMarkerInd].enabled = false;
	}

	void App::onGridCursorUpdate()
	{
		m_gridMesh.clear();
		const IVec3& min{ m_appWidget.source().cursorMin() };
		const IVec3& max{ m_appWidget.source().cursorMax() };
		const IVec3 mouse{ m_grid.coord(m_mouseGridIndex) };
		if (mouse.x() < min.x() || mouse.y() < min.y() || mouse.z() < min.z() ||
			mouse.x() > max.x() || mouse.y() > max.y() || mouse.z() > max.z())
		{
			m_mouseGridIndex = m_grid.index(min);
			m_canvas.marker_sets[c_selectionMarkerSetInd][c_mouseMarkerInd].enabled = false;
		}
		if (!m_appWidget.source().hideCursor())
		{
			const Int layers{ m_appWidget.source().size() + 1 };
			m_gridMesh.reserve(static_cast<std::size_t>(layers * layers * 3 * 2 + 8));
			for (unsigned int d{}; d < 3; d++)
			{
				IVec3 coord;
				const unsigned int ds[3]{ d, (d + 1) % 3, (d + 2) % 3 };
				for (Int x{ min[ds[0]] }; x <= max[ds[0]]; x++)
				{
					coord[ds[0]] = x;
					for (Int y{ min[ds[1]] }; y <= max[ds[1]]; y++)
					{
						coord[ds[1]] = y;
						for (Int z{}; z < 2; z++)
						{
							coord[ds[2]] = z ? max[ds[2]] : min[ds[2]];
							m_gridMesh.push_back(m_grid.point(coord));
						}
					}
				}
			}
			m_gridMesh.update_bbox();
		}
	}

	void App::onChildUpdate(std::size_t _child)
	{
		const ChildControl& child{ m_appWidget.children()[_child] };
		const bool valid{ child.hexControl().valid() && m_appWidget.source().displ().valid() };
		const bool shown{ m_appWidget.shown(child) };
		const HexVerts verts{ m_grid.points(child.hexControl().verts()) };
		cinolib::DrawableHexmesh<>& mesh{ *m_childMeshes[_child] };
		if (valid && shown)
		{
			for (std::size_t i{}; i < 8; i++)
			{
				mesh.vert(static_cast<unsigned int>(i)) = verts[i];
			}
			mesh.poly_set_color(child.style().color(1.0f, 1.0f, m_appWidget.solidMode() ? 1.0f : 0.75f));
			mesh.show_in_wireframe(m_appWidget.solidMode());
			mesh.show_out_wireframe(m_appWidget.solidMode());
			mesh.update_normals();
			mesh.updateGL();
			mesh.update_normals();
			mesh.updateGL();
		}
		mesh.show_mesh(valid && shown);
		mesh.update_bbox();
		const HexVertData<std::size_t> firstOccurrencies{ child.hexControl().firstOccurrenceIndices() };
		for (std::size_t i{}; i < 8; i++)
		{
			cinolib::Marker& marker{ m_canvas.marker_sets[c_vertsMarkerSetInd][i + _child * 8] };
			const bool duplicate{ firstOccurrencies[i] != i };
			marker.pos_3d = verts[i];
			marker.color = duplicate ? cinolib::Color::YELLOW() : child.style().color(0.25f);
			marker.font_size = child.active() ? 18u : 0u;
			marker.enabled = shown && (child.active() || duplicate);
			marker.shape = !child.active() && duplicate ? cinolib::Marker::EShape::Cross90 : cinolib::Marker::EShape::CircleFilled;
		}
	}

	const HexVerts App::cubeVerts{ hexUtils::cubeVerts<Real>(RVec3{0,0,0}, RVec3{1,1,1}) };

	void App::onChildAdd()
	{
		cinolib::DrawableHexmesh<>& mesh{ *new cinolib::DrawableHexmesh<>{} };
		mesh.poly_set_color(cinolib::Color::BLACK());
		mesh.show_in_wireframe_width(2);
		mesh.show_out_wireframe_width(2);
		mesh.show_mesh_flat();
		mesh.draw_back_faces = false;
		for (const RVec3& vert : cubeVerts)
		{
			mesh.vert_add(vert);
		}
		mesh.poly_add(vertsOrder);
		m_childMeshes.push_back(&mesh);
		m_canvas.marker_sets[c_vertsMarkerSetInd].resize(m_childMeshes.size() * 8);
		for (std::size_t i{}; i < 8; i++)
		{
			const std::size_t mi{ i + (m_childMeshes.size() - 1) * 8 };
			m_canvas.marker_sets[c_vertsMarkerSetInd][mi] = {
				.text{std::to_string(i)},
				.shape_radius = 4u,
				.enabled = false,
			};
		}
		m_canvas.push(m_childMeshes.back(), false);
		onChildUpdate(m_childMeshes.size() - 1);
	}

	void App::onChildrenClear()
	{
		m_canvas.marker_sets[c_vertsMarkerSetInd].clear();
		for (cinolib::DrawableHexmesh<>* mesh : m_childMeshes)
		{
			m_canvas.pop(mesh);
			delete mesh;
		}
		m_childMeshes.clear();
	}

	void App::onChildRemove(std::size_t _child)
	{
		m_canvas.pop(m_childMeshes[_child]);
		const auto begin{ m_canvas.marker_sets[c_vertsMarkerSetInd].begin() + _child * 8 };
		m_canvas.marker_sets[c_vertsMarkerSetInd].erase(begin, begin + 8);
		delete m_childMeshes[_child];
		m_childMeshes.erase(m_childMeshes.begin() + _child);
	}

	void App::onSetVert()
	{
		if (m_appWidget.activeChildIndex())
		{
			m_appWidget.setActiveVert(m_grid.coord(m_mouseGridIndex));
		}
	}

	void App::onActiveVertChange()
	{
		cinolib::Marker& marker{ m_canvas.marker_sets[c_selectionMarkerSetInd][c_selectedVertMarkerInd] };
		if (m_appWidget.activeChildIndex())
		{
			const ChildControl child{ m_appWidget.activeChild() };
			marker.pos_3d = m_grid.point(child.hexControl().verts()[child.hexControl().activeVert()]);
			marker.color = child.style().color(0.25);
			marker.enabled = true;
		}
		else
		{
			marker.enabled = false;
		}
	}

	void App::onAdvanceActiveChild(bool _advance)
	{
		if (!m_appWidget.activeChildIndex())
		{
			if (!m_appWidget.children().empty())
			{
				m_appWidget.setActiveChild(0);
			}
		}
		else
		{
			std::size_t index{ m_appWidget.activeChildIndex().value() + 1 + (_advance ? 1 : -1) };
			if (index == 0)
			{
				index = m_appWidget.children().size();
			}
			index--;
			m_appWidget.setActiveChild(index % m_appWidget.children().size());
		}
	}

	void App::onSetActiveVert(std::size_t _vert)
	{
		if (!m_appWidget.activeChildIndex())
		{
			onAdvanceActiveChild(true);
		}
		if (m_appWidget.activeChildIndex())
		{
			m_appWidget.setActiveVert(_vert);
		}
	}

	void App::onAdvanceActiveVert(bool _advance)
	{
		if (!m_appWidget.activeChildIndex())
		{
			onAdvanceActiveChild(true);
		}
		if (m_appWidget.activeChildIndex())
		{
			std::size_t index{ *m_appWidget.activeVertIndex() + 1 + (_advance ? 1 : -1) };
			if (index == 0)
			{
				index = 8;
			}
			index--;
			m_appWidget.setActiveVert(index % 8);
		}
	}

	void App::onMouseMove()
	{
		const cinolib::Ray r{ m_canvas.eye_to_mouse_ray() };
		const IVec3 min{ m_appWidget.source().cursorMin() };
		const IVec3 max{ m_appWidget.source().cursorMax() };
		m_mouseGridIndex = m_grid.closestToRay(r.begin(), r.dir(), min, max);
		cinolib::Marker& marker{ m_canvas.marker_sets[c_selectionMarkerSetInd][c_mouseMarkerInd] };
		marker.pos_3d = m_grid.point(m_mouseGridIndex);
		marker.enabled = true;
	}

	bool App::onKeyPress(int _key, int _modifiers)
	{
		const cinolib::KeyBinding binding{ _key, _modifiers };
		if (binding == c_kbAddChild)
		{
			m_appWidget.addChild();
			m_appWidget.setActiveChild(m_appWidget.children().size() - 1);
		}
		else if (binding == c_kbToggleSolidMode)
		{
			m_appWidget.setSolidMode(!m_appWidget.solidMode());
		}
		else if (binding == c_kbEditDimX)
		{
			m_appWidget.editDim = hexUtils::EDim::X;
		}
		else if (binding == c_kbEditDimY)
		{
			m_appWidget.editDim = hexUtils::EDim::Y;
		}
		else if (binding == c_kbEditDimZ)
		{
			m_appWidget.editDim = hexUtils::EDim::Z;
		}
		else if (binding == c_kbActPrevChild)
		{
			onAdvanceActiveChild(false);
		}
		else if (binding == c_kbActNextChild)
		{
			onAdvanceActiveChild(true);
		}
		else if (binding == c_kbActPrevChild)
		{
			onAdvanceActiveVert(false);
		}
		else if (binding == c_kbActNextVert)
		{
			onAdvanceActiveVert(true);
		}
		else if (binding == c_kbActVert0)
		{
			onSetActiveVert(0);
		}
		else if (binding == c_kbActVert1)
		{
			onSetActiveVert(1);
		}
		else if (binding == c_kbActVert2)
		{
			onSetActiveVert(2);
		}
		else if (binding == c_kbActVert3)
		{
			onSetActiveVert(3);
		}
		else if (binding == c_kbActVert4)
		{
			onSetActiveVert(4);
		}
		else if (binding == c_kbActVert5)
		{
			onSetActiveVert(5);
		}
		else if (binding == c_kbActVert6)
		{
			onSetActiveVert(6);
		}
		else if (binding == c_kbActVert7)
		{
			onSetActiveVert(7);
		}
		else if (binding == c_kbToggleSingleMode)
		{
			m_appWidget.setSingleMode(!m_appWidget.singleMode());
		}
		else if (binding == c_kbToggleCursor)
		{
			m_appWidget.setHideCursor(!m_appWidget.source().hideCursor());
		}
		else if (binding == c_kbSetVertAndNextVert)
		{
			onSetVert();
			onAdvanceActiveVert(true);
		}
		else if (binding == c_kbSetVert)
		{
			onSetVert();
		}
		else if (binding == c_kbCubeActChild)
		{
			m_appWidget.cubeActive();
		}
		else if (binding == c_kbAddCursorChildGrid)
		{
			m_appWidget.addChildrenCursorGrid();
		}
		else if (binding == c_kbShrinkCursor)
		{
			m_appWidget.scaleCursor(false);
		}
		else if (binding == c_kbExpandCursor)
		{
			m_appWidget.scaleCursor(true);
		}
		else if (binding == c_kbTranslateCursorMinus)
		{
			m_appWidget.translateCursor(false);
		}
		else if (binding == c_kbTranslateCursorPlus)
		{
			m_appWidget.translateCursor(true);
		}
		else if (binding == c_kbRemoveShown)
		{
			m_appWidget.removeShown();
		}
		else if (binding == c_kbTranslateShownMinus)
		{
			m_appWidget.translateShown(false);
		}
		else if (binding == c_kbTranslateShownPlus)
		{
			m_appWidget.translateShown(true);
		}
		else if (binding == c_kbSelActChild)
		{
			m_appWidget.setActiveSelected(true);
		}
		else if (binding == c_kbCloneShown)
		{
			m_appWidget.cloneShown();
		}
		else if (binding == c_kbHideShown)
		{
			m_appWidget.hideActive();
		}
		else if (binding == c_kbRotateShown)
		{
			m_appWidget.rotateShown();
		}
		else if (binding == c_kbFlipShown)
		{
			m_appWidget.flipShown();
		}
		else if (binding == c_kbSelAll)
		{
			m_appWidget.setAllSelected(true);
		}
		else if (binding == c_kbSave)
		{
			m_appWidget.save(false);
		}
		else if (binding == c_kbOpen)
		{
			m_appWidget.load();
		}
		else if (binding == c_kbDeselAll)
		{
			m_appWidget.setAllSelected(false);
		}
		else if (binding == c_kbSelActChild)
		{
			m_appWidget.setActiveSelected(false);
		}
		else if (binding == c_kbSaveNew)
		{
			m_appWidget.save(true);
		}
		else
		{
			return false;
		}
		return true;
	}

	void App::onClick(int _modifiers)
	{

		if (_modifiers == c_mbModSetVert)
		{
			onSetVert();
		}
		else
		{
			const auto pickActive{ [this](std::size_t _child) {
				const HexVertsU& verts{ m_appWidget.children()[_child].hexControl().verts() };
				const auto it{ std::find(verts.begin(), verts.end(), m_grid.coord(m_mouseGridIndex)) };
				if (it != verts.end())
				{
					m_appWidget.setActiveChild(_child);
					m_appWidget.setActiveVert(it - verts.begin());
					return true;
				}
				return false;
			}
			};
			if (_modifiers == c_mbModActChild)
			{
				for (std::size_t i{}; i < m_appWidget.children().size(); i++)
				{
					if (m_appWidget.shown(m_appWidget.children()[i]) && pickActive(i))
					{
						break;
					}
				}
			}
			else if (_modifiers == c_mbModActActChildVert)
			{
				if (m_appWidget.activeChildIndex())
				{
					pickActive(*m_appWidget.activeChildIndex());
				}
			}
			else if (_modifiers == c_mbModActAnotherChild)
			{
				for (std::size_t i{ m_appWidget.activeChildIndex().value_or(-1) + 1 }; i < m_appWidget.children().size(); i++)
				{
					if (m_appWidget.shown(m_appWidget.children()[i]) && pickActive(i))
					{
						return;
					}
				}
				for (std::size_t i{ 0 }; i < m_appWidget.activeChildIndex().value_or(0); i++)
				{
					if (m_appWidget.shown(m_appWidget.children()[i]) && pickActive(i))
					{
						return;
					}
				}
			}
		}
	}

	void App::setWindowTitle()
	{
		const std::string appName{ "RSE" };
		static constexpr std::size_t maxFileNameSize{ 20 };
		std::string fileName{ m_appWidget.file().value_or("") };
		if (fileName.size() > maxFileNameSize)
		{
			fileName = "..." + fileName.substr(fileName.size() - maxFileNameSize);
		}
		const std::string title{ fileName.empty() ? appName : appName + " - " + fileName };
		glfwSetWindowTitle(m_canvas.window, title.c_str());
	}

	void App::printBindings()
	{
		std::cout << "----- Editor key bindings -----\n";
		cinolib::print_binding(c_kbToggleSingleMode.name(), "toggle single mode");
		cinolib::print_binding(c_kbToggleSolidMode.name(), "toggle solid mode");
		cinolib::print_binding(c_kbActNextChild.name(), "activate next child");
		cinolib::print_binding(c_kbActPrevChild.name(), "activate previous child");
		cinolib::print_binding(c_kbActNextVert.name(), "activate next vertex");
		cinolib::print_binding(c_kbActPrevVert.name(), "activate previous vertex");
		cinolib::print_binding(c_kbActVert0.name(), "activate vertex 0");
		cinolib::print_binding(c_kbActVert1.name(), "activate vertex 1");
		cinolib::print_binding(c_kbActVert2.name(), "activate vertex 2");
		cinolib::print_binding(c_kbActVert3.name(), "activate vertex 3");
		cinolib::print_binding(c_kbActVert4.name(), "activate vertex 4");
		cinolib::print_binding(c_kbActVert5.name(), "activate vertex 5");
		cinolib::print_binding(c_kbActVert6.name(), "activate vertex 6");
		cinolib::print_binding(c_kbActVert7.name(), "activate vertex 7");
		cinolib::print_binding(c_kbSetVertAndNextVert.name(), "set vertex and activate next");
		cinolib::print_binding(c_kbSetVert.name(), "set vertex");
		cinolib::print_binding(c_kbAddChild.name(), "add child");
		cinolib::print_binding(c_kbAddCursorChildGrid.name(), "add cursor children grid");
		cinolib::print_binding(c_kbToggleCursor.name(), "toggle cursor");
		cinolib::print_binding(c_kbCubeActChild.name(), "expand active child to fill cursor");
		cinolib::print_binding(c_kbShrinkCursor.name(), "shrink cursor along edit dim");
		cinolib::print_binding(c_kbExpandCursor.name(), "expand cursor along edit dim");
		cinolib::print_binding(c_kbTranslateCursorPlus.name(), "translate cursor along edit dim +");
		cinolib::print_binding(c_kbTranslateCursorMinus.name(), "translate cursor along edit dim -");
		cinolib::print_binding(c_kbHideShown.name(), "hide shown children");
		cinolib::print_binding(c_kbCloneShown.name(), "clone shown children");
		cinolib::print_binding(c_kbRemoveShown.name(), "remove shown children");
		cinolib::print_binding(c_kbTranslateShownPlus.name(), "translate shown children along edit dim +");
		cinolib::print_binding(c_kbTranslateShownMinus.name(), "translate shown children along edit dim -");
		cinolib::print_binding(c_kbRotateShown.name(), "rotate shown children along edit dim");
		cinolib::print_binding(c_kbFlipShown.name(), "flip shown children with respect to edit dim");
		cinolib::print_binding(c_kbSelAll.name(), "select all children");
		cinolib::print_binding(c_kbDeselAll.name(), "deselect all children");
		cinolib::print_binding(c_kbSelActChild.name(), "select active child");
		cinolib::print_binding(c_kbDeselActChild.name(), "deselect active child");
		cinolib::print_binding(c_kbEditDimX.name(), "set edit dim X");
		cinolib::print_binding(c_kbEditDimY.name(), "set edit dim Y");
		cinolib::print_binding(c_kbEditDimZ.name(), "set edit dim Z");
		cinolib::print_binding(c_kbSave.name(), "save");
		cinolib::print_binding(c_kbSaveNew.name(), "save as a new file");
		cinolib::print_binding(c_kbOpen.name(), "open");
		cinolib::print_binding(cinolib::KeyBinding::mod_names(c_mbModSetVert), "set vert (hold down and click)");
		cinolib::print_binding(cinolib::KeyBinding::mod_names(c_mbModActChild), "activate child (hold down and click)");
		cinolib::print_binding(cinolib::KeyBinding::mod_names(c_mbModActAnotherChild), "activate another child (hold down and click)");
		cinolib::print_binding(cinolib::KeyBinding::mod_names(c_mbModActActChildVert), "activate active child vertex (hold down and click)");
		std::cout << "-------------------------------" << std::endl;
	}

	App::App() : m_canvas{}, m_axesWidget{ m_canvas.camera }, m_appWidget{}, m_grid{}, m_gridMesh{}, m_borderMesh{}, m_mouseGridIndex{}, vertsOrder{}
	{
		// vertsOrder
		vertsOrder.resize(8);
		for (std::size_t i{}; i < 8; i++)
		{
			vertsOrder[i] = static_cast<unsigned int>(hexUtils::cinolibHexInds[i]);
		}
		// border
		m_borderMesh.set_color(cinolib::Color::GRAY());
		m_borderMesh.set_cheap_rendering(true);
		m_borderMesh.set_thickness(1);
		m_borderMesh.reserve(12 * 2);
		// grid
		m_gridMesh.set_color(cinolib::Color::WHITE());
		m_gridMesh.set_cheap_rendering(true);
		m_gridMesh.set_thickness(1);
		// app widget
		m_appWidget.show_open = true;
		m_appWidget.onSourceUpdate += [this]() { onGridUpdate(); };
		m_appWidget.onCursorUpdate += [this]() { onGridCursorUpdate(); };
		m_appWidget.onChildAdd += [this]() { onChildAdd(); };
		m_appWidget.onChildrenClear += [this]() { onChildrenClear(); };
		m_appWidget.onActiveVertChange += [this]() { onActiveVertChange(); };
		m_appWidget.onChildRemove += [this](std::size_t _i) { onChildRemove(_i); };
		m_appWidget.onChildUpdate += [this](std::size_t _i) { onChildUpdate(_i); };
		m_appWidget.onFileChange += [this]() { setWindowTitle(); };
		// markers
		m_canvas.marker_sets.resize(2, {});
		m_canvas.marker_sets[c_selectionMarkerSetInd].resize(2);
		m_canvas.marker_sets[c_selectionMarkerSetInd][c_mouseMarkerInd] = {
			.color{cinolib::Color::WHITE()},
			.shape_radius = 5u,
			.shape = cinolib::Marker::EShape::Cross45,
			.enabled = false,
			.line_thickness = 1.0f,
		};
		m_canvas.marker_sets[c_selectionMarkerSetInd][c_vertsMarkerSetInd] = {
			.shape_radius = 6u,
			.shape = cinolib::Marker::EShape::CircleOutline,
			.enabled = false,
			.line_thickness = 1.0f,
		};
		// state
		onGridUpdate();
		onGridCursorUpdate();
		onChildrenClear();
		// canvas
		m_canvas.background = cinolib::Color::hsv2rgb(0.0f, 0.0f, 0.1f);
		m_canvas.push(&m_axesWidget);
		m_canvas.push(&m_appWidget);
		m_canvas.push(&m_borderMesh);
		m_canvas.push(&m_gridMesh);
		m_canvas.depth_cull_markers = false;
		m_canvas.show_sidebar(true);
		m_canvas.key_bindings.pan_with_arrow_keys = false;
		m_canvas.callback_mouse_left_click = [this](int _modifiers) { onClick(_modifiers); return true; };
		m_canvas.callback_key_pressed = [this](int _key, int _modifiers) { return onKeyPress(_key, _modifiers); };
		m_canvas.callback_mouse_moved = [this](double _x, double _y) { onMouseMove(); return false; };
		setWindowTitle();
	}

	void App::open(const std::string& _file)
	{
		m_appWidget.load(_file);
	}

	int App::launch()
	{
		printBindings();
		return m_canvas.launch();
	}

}