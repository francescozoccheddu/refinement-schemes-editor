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
		for (std::size_t i{}; i < m_appWidget.children().size(); i++)
		{
			onChildUpdate(i);
		}
		m_mouseGridIndex = 0;
		m_canvas.markers[0].pos_3d = m_grid.point(0);
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
			m_canvas.markers[0].pos_3d = m_grid.point(m_mouseGridIndex);
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
			mesh.poly_set_color(child.style().color(1.0f, 1.0f, 0.75f));
			mesh.update_normals();
			mesh.updateGL();
			mesh.update_normals();
			mesh.updateGL();
		}
		mesh.show_mesh(valid && shown);
		const HexVertData<std::size_t> firstOccurrencies{ child.hexControl().firstOccurrenceIndices() };
		for (std::size_t i{}; i < 8; i++)
		{
			cinolib::Marker& marker{ m_canvas.markers[i + _child * 8 + 2] };
			const bool duplicate{ firstOccurrencies[i] != i };
			marker.pos_3d = verts[i];
			marker.color = duplicate ? cinolib::Color::YELLOW() : child.style().color(0.25f);
			marker.disk_radius = shown ? 5 : 0;
			marker.font_size = shown && child.active() ? 20 : 0;
		}
	}

	const HexVerts App::cubeVerts{ hexUtils::cubeVerts<Real>(RVec3{0,0,0}, RVec3{1,1,1}) };

	void App::onChildAdd()
	{
		cinolib::DrawableHexmesh<>& mesh{ *new cinolib::DrawableHexmesh<>{} };
		mesh.show_mesh_flat();
		mesh.show_in_wireframe(false);
		mesh.show_out_wireframe(false);
		mesh.draw_back_faces = false;
		for (const RVec3 vert : cubeVerts)
		{
			mesh.vert_add(vert);
		}
		mesh.poly_add(vertsOrder);
		m_childMeshes.push_back(&mesh);
		m_canvas.markers.resize(m_childMeshes.size() * 8 + 2);
		for (std::size_t i{}; i < 8; i++)
		{
			const std::size_t mi{ i + (m_childMeshes.size() - 1) * 8 + 2 };
			m_canvas.markers[mi] = cinolib::Marker{
				.pos_3d = RVec3{0,0,0},
				.text{std::to_string(i)},
				.filled{false}
			};
		}
		m_canvas.push(m_childMeshes.back(), false);
		onChildUpdate(m_childMeshes.size() - 1);
	}

	void App::onChildrenClear()
	{
		m_canvas.markers.resize(2);
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
		const auto begin{ m_canvas.markers.begin() + _child * 8 + 2 };
		m_canvas.markers.erase(begin, begin + 8);
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
		if (m_appWidget.activeChildIndex())
		{
			const ChildControl child{ m_appWidget.activeChild() };
			m_canvas.markers[1].pos_3d = m_grid.point(child.hexControl().verts()[child.hexControl().activeVert()]);
			m_canvas.markers[1].color = child.style().color(0.25);
			m_canvas.markers[1].disk_radius = 8u;
		}
		else
		{
			m_canvas.markers[1].disk_radius = 0u;
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
		m_canvas.markers[0].pos_3d = m_grid.point(m_mouseGridIndex);
	}

	bool App::onKeyPress(int _key, int _modifiers)
	{
		if (!_modifiers)
		{
			switch (_key)
			{
				case GLFW_KEY_A:
					m_appWidget.addChild();
					m_appWidget.setActiveChild(m_appWidget.children().size() - 1);
					return true;
				case GLFW_KEY_X:
					m_appWidget.editDim = hexUtils::EDim::X;
					return true;
				case GLFW_KEY_Y:
					m_appWidget.editDim = hexUtils::EDim::Y;
					return true;
				case GLFW_KEY_Z:
					m_appWidget.editDim = hexUtils::EDim::Z;
					return true;
				case GLFW_KEY_UP:
					onAdvanceActiveChild(false);
					return true;
				case GLFW_KEY_DOWN:
					onAdvanceActiveChild(true);
					return true;
				case GLFW_KEY_LEFT:
					onAdvanceActiveVert(false);
					return true;
				case GLFW_KEY_RIGHT:
					onAdvanceActiveVert(true);
					return true;
				case GLFW_KEY_KP_0:
				case GLFW_KEY_0:
					onSetActiveVert(0);
					return true;
				case GLFW_KEY_KP_1:
				case GLFW_KEY_1:
					onSetActiveVert(1);
					return true;
				case GLFW_KEY_KP_2:
				case GLFW_KEY_2:
					onSetActiveVert(2);
					return true;
				case GLFW_KEY_KP_3:
				case GLFW_KEY_3:
					onSetActiveVert(3);
					return true;
				case GLFW_KEY_KP_4:
				case GLFW_KEY_4:
					onSetActiveVert(4);
					return true;
				case GLFW_KEY_KP_5:
				case GLFW_KEY_5:
					onSetActiveVert(5);
					return true;
				case GLFW_KEY_KP_6:
				case GLFW_KEY_6:
					onSetActiveVert(6);
					return true;
				case GLFW_KEY_KP_7:
				case GLFW_KEY_7:
					onSetActiveVert(7);
					return true;
				case GLFW_KEY_S:
					m_appWidget.setSingleMode(!m_appWidget.singleMode());
					return true;
				case GLFW_KEY_H:
					m_appWidget.setHideCursor(!m_appWidget.source().hideCursor());
					return true;
				case GLFW_KEY_E:
					onSetVert();
					onAdvanceActiveVert(true);
					return true;
				case GLFW_KEY_W:
					onSetVert();
					return true;
				case GLFW_KEY_Q:
					m_appWidget.cubeActive();
					return true;
			}
		}
		else if (_modifiers == GLFW_MOD_SHIFT)
		{
			switch (_key)
			{
				case GLFW_KEY_A:
					m_appWidget.addChildrenCursorGrid();
					return true;
			}
		}
		else if (_modifiers == GLFW_MOD_ALT)
		{
			switch (_key)
			{
				case GLFW_KEY_LEFT:
					m_appWidget.scaleCursor(false);
					return true;
				case GLFW_KEY_RIGHT:
					m_appWidget.scaleCursor(true);
					return true;
				case GLFW_KEY_DOWN:
					m_appWidget.translateCursor(false);
					return true;
				case GLFW_KEY_UP:
					m_appWidget.translateCursor(true);
					return false;
			}
		}
		else if (_modifiers == GLFW_MOD_CONTROL)
		{
			switch (_key)
			{
				case GLFW_KEY_DELETE:
					m_appWidget.removeShown();
					return true;
				case GLFW_KEY_DOWN:
					m_appWidget.translateShown(false);
					return true;
				case GLFW_KEY_UP:
					m_appWidget.translateShown(true);
					return true;
				case GLFW_KEY_Q:
					m_appWidget.setActiveSelected(true);
					return true;
				case GLFW_KEY_D:
					m_appWidget.cloneShown();
					return true;
				case GLFW_KEY_R:
					m_appWidget.rotateShown();
					return true;
				case GLFW_KEY_F:
					m_appWidget.flipShown();
					return true;
				case GLFW_KEY_A:
					m_appWidget.setAllSelected(true);
					return true;
				case GLFW_KEY_S:
					m_appWidget.save(false);
					return true;
				case GLFW_KEY_O:
					m_appWidget.load();
					return true;
			}
		}
		else if (_modifiers == (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
		{
			switch (_key)
			{
				case GLFW_KEY_A:
					m_appWidget.setAllSelected(false);
					return true;
				case GLFW_KEY_Q:
					m_appWidget.setActiveSelected(false);
					return true;
				case GLFW_KEY_S:
					m_appWidget.save(true);
					return true;
			}
		}
		return false;
	}

	void App::onClick(int _modifiers)
	{
		if (_modifiers == GLFW_MOD_CONTROL)
		{
			onSetVert();
		}
		else if (_modifiers == GLFW_MOD_SHIFT)
		{
			if (m_appWidget.activeChildIndex())
			{
				const HexVertsU& verts{ m_appWidget.activeChild().hexControl().verts() };
				const auto it{ std::find(verts.begin(), verts.end(), m_grid.coord(m_mouseGridIndex)) };
				if (it != verts.end())
				{
					m_appWidget.setActiveVert(it - verts.begin());
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

	App::App() : m_canvas{}, m_axesWidget{ m_canvas.camera }, m_appWidget{}, m_grid{}, m_gridMesh{}, m_borderMesh{}, m_mouseGridIndex{}, vertsOrder{}
	{
		// vertsOrder
		vertsOrder.resize(8);
		for (std::size_t i{}; i < 8; i++)
		{
			vertsOrder[i] = static_cast<unsigned int>(hexUtils::cinolibHexInds[i]);
		}
		// markers
		m_canvas.markers.resize(2);
		m_canvas.markers[0] = cinolib::Marker{
			.pos_3d = RVec3{0,0,0},
			.color{cinolib::Color::WHITE()},
			.disk_radius{4u},
			.filled{true},
		};
		m_canvas.markers[1] = cinolib::Marker{
			.pos_3d = RVec3{0,0,0},
			.disk_radius{0u},
			.filled{false}
		};
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
		return m_canvas.launch();
	}

}