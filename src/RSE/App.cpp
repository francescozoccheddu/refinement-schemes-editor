#include <RSE/App.hpp>

#include <cinolib/color.h>
#include <type_traits>
#include <string>
#include <RSE/hexUtils.hpp>

namespace RSE
{

	void App::onGridUpdate()
	{
		m_grid.make(m_appWidget.sourceSize(), m_appWidget.sourceControl().verts());
		const Int layers{ m_appWidget.sourceSize() + 1 };
		m_gridMesh.resize(static_cast<std::size_t>(layers * layers * 3 * 2));
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
						coord[static_cast<unsigned int>(d)] = z ? layers - 1 : 0;
						m_gridMesh[i++] = m_grid.point(coord);
					}
				}
			}
		}
		m_gridMesh.update_bbox();
		m_mouseGridIndex = 0;
		for (std::size_t i{}; i < m_appWidget.childControls().size(); i++)
		{
			onChildUpdate(i);
		}
	}

	void App::onChildUpdate(std::size_t _child)
	{
		const ChildControl& child{ m_appWidget.childControls()[_child] };
		const bool valid{ child.hexControl().valid() && m_appWidget.sourceControl().valid() };
		const bool visible{ m_appWidget.visible(child) };
		const HexVerts verts{ m_grid.points(child.hexControl().verts()) };
		cinolib::DrawableHexmesh<>& mesh{ *m_childMeshes[_child] };
		if (valid && visible)
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
		mesh.show_mesh(valid && visible);
		const HexVertData<std::size_t> firstOccurrencies{ child.hexControl().firstOccurrenceIndices() };
		for (std::size_t i{}; i < 8; i++)
		{
			cinolib::Marker& marker{ m_canvas.markers[i + _child * 8] };
			const bool duplicate{ firstOccurrencies[i] != i };
			marker.pos_3d = verts[i];
			marker.color = child.style().color(0.5f);
			marker.disk_radius = !valid && visible ? (duplicate ? 8 : 5) : 0;
			marker.font_size = child.expanded() && visible && !duplicate ? (duplicate ? 24 : 20) : 0;
		}
	}

	void App::onChildAdd()
	{
		cinolib::DrawableHexmesh<>& mesh{ *new cinolib::DrawableHexmesh<>{} };
		mesh.show_mesh_flat();
		mesh.show_in_wireframe(false);
		mesh.show_out_wireframe(false);
		mesh.draw_back_faces = false;
		for (const RVec3 vert : hexUtils::cubeVerts<Real>())
		{
			mesh.vert_add(vert);
		}
		std::vector<unsigned int> vertsOrder{};
		vertsOrder.resize(8);
		for (std::size_t i{}; i < 8; i++)
		{
			vertsOrder[i] = static_cast<unsigned int>(hexUtils::cinolibVertsOrder[i]);
		}
		mesh.poly_add(vertsOrder);
		mesh.edge_set_color(cinolib::Color::BLACK());
		m_childMeshes.push_back(&mesh);
		m_canvas.markers.resize(m_childMeshes.size() * 8 + 1);
		for (std::size_t i{}; i < 8; i++)
		{
			const std::size_t mi{ i + (m_childMeshes.size() - 1) * 8 };
			m_canvas.markers[mi] = cinolib::Marker{
				.text{std::to_string(i)}
			};
		}
		m_canvas.push(m_childMeshes.back(), false);
		onChildUpdate(m_childMeshes.size() - 1);
		updateMouseMarker();
	}

	void App::onChildrenClear()
	{
		m_canvas.markers.resize(1);
		for (cinolib::DrawableHexmesh<>* mesh : m_childMeshes)
		{
			m_canvas.pop(mesh);
			delete mesh;
		}
		m_childMeshes.clear();
		updateMouseMarker();
	}

	void App::onChildRemove(std::size_t _child)
	{
		m_canvas.pop(m_childMeshes[_child]);
		delete m_childMeshes[_child];
		m_childMeshes.erase(m_childMeshes.begin() + _child);
		updateMouseMarker();
	}

	void App::onSetVert()
	{
		if (m_appWidget.activeChild().has_value())
		{
			m_appWidget.setActiveVert(m_grid.coord(m_mouseGridIndex));
		}
	}

	void App::onSetActiveChild(bool _advance)
	{
		if (!m_appWidget.activeChild().has_value())
		{
			m_appWidget.setActiveChild(0);
		}
		else
		{
			std::size_t index{ m_appWidget.activeChild().value() + 1 + (_advance ? 1 : -1) };
			if (index == 0)
			{
				index = m_appWidget.childControls().size();
			}
			index--;
			m_appWidget.setActiveChild(index % m_appWidget.childControls().size());
		}
	}

	void App::onSetActiveVert(bool _advance)
	{
		if (!m_appWidget.activeChild().has_value())
		{
			m_appWidget.setActiveChild(0);
		}
		std::size_t index{ m_appWidget.childControls()[m_appWidget.activeChild().value()].hexControl().activeVert() + 1 + (_advance ? 1 : -1) };
		if (index == 0)
		{
			index = 8;
		}
		index--;
		m_appWidget.setActiveVert(index % 8);
	}

	void App::onMouseMove()
	{
		const cinolib::Ray r{ m_canvas.eye_to_mouse_ray() };
		m_mouseGridIndex = m_grid.closestToRay(r.begin(), r.dir());
		updateMouseMarker();
	}

	void App::updateMouseMarker()
	{
		m_canvas.markers.back() = cinolib::Marker{
			.pos_3d{ m_grid.point(m_mouseGridIndex)},
			.color{cinolib::Color::WHITE()},
			.disk_radius{5u}
		};
	}

	App::App() : m_canvas{}, m_axesWidget{ m_canvas.camera }, m_appWidget{}, m_grid{}, m_gridMesh{}, m_mouseGridIndex{}
	{
		onGridUpdate();
		onChildrenClear();
		m_canvas.background = cinolib::Color::hsv2rgb(0.0f, 0.0f, 0.1f);
		m_canvas.push(&m_axesWidget);
		m_canvas.push(&m_appWidget);
		m_canvas.push(&m_gridMesh);
		m_canvas.depth_cull_markers = false;
		m_canvas.show_sidebar(true);
		m_canvas.key_bindings.pan_with_arrow_keys = false;
		m_appWidget.show_open = true;
		m_appWidget.onSourceUpdate += [this]() { onGridUpdate(); };
		m_appWidget.onChildAdd += [this]() { onChildAdd(); };
		m_appWidget.onChildrenClear += [this]() { onChildrenClear(); };
		m_appWidget.onChildRemove += [this](std::size_t _i) { onChildRemove(_i); };
		m_appWidget.onChildUpdate += [this](std::size_t _i) { onChildUpdate(_i); };
		m_gridMesh.set_color(cinolib::Color::WHITE());
		m_gridMesh.set_cheap_rendering(true);
		m_gridMesh.set_thickness(1);
		m_canvas.callback_key_pressed = [this](int _key, int _modifiers) {
			switch (_key)
			{
				case GLFW_KEY_A:
					m_appWidget.doAddChild();
					m_appWidget.setActiveChild(m_appWidget.childControls().size() - 1);
					return true;
				case GLFW_KEY_E:
					if (m_appWidget.activeChild().has_value())
					{
						onSetVert();
						onSetActiveVert(true);
					}
					return true;
				case GLFW_KEY_W:
					onSetVert();
					return true;
				case GLFW_KEY_UP:
					onSetActiveChild(false);
					return true;
				case GLFW_KEY_DOWN:
					onSetActiveChild(true);
					return true;
				case GLFW_KEY_LEFT:
					onSetActiveVert(false);
					return true;
				case GLFW_KEY_RIGHT:
					onSetActiveVert(true);
					return true;
			}
			return false;
		};
		m_canvas.callback_mouse_moved = [this](double _x, double _y)
		{
			onMouseMove();
			return false;
		};
	}

	int App::launch()
	{
		return m_canvas.launch();
	}

}