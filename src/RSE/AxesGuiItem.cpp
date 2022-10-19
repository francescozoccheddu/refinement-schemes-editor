#include <RSE/AxesGuiItem.hpp>

#include <imgui.h>
#include <cinolib/deg_rad.h>
#include <cinolib/gl/glcanvas.h>
#include <cmath>
#include <array>
#include <utility>
#include <algorithm>

namespace RSE
{

	AxesGuiItem::AxesGuiItem(const cinolib::FreeCamera<Real>& _camera)
		: m_camera{ &_camera }
	{}

	const cinolib::FreeCamera<Real>& AxesGuiItem::camera() const
	{
		return *m_camera;
	}

	void AxesGuiItem::draw()
	{
		ImDrawList& drawList{ *ImGui::GetWindowDrawList() };
		RVec3 origin;
		Real radius;
		if (m_camera->projection.perspective)
		{
			origin = m_camera->view.centerAt(3);
			radius = std::tan(cinolib::to_rad(m_camera->projection.verticalFieldOfView / 2));
		}
		else
		{
			origin = m_camera->view.centerAt(2);
			radius = m_camera->projection.verticalFieldOfView / 2 * 0.75;
		}
		const RVec3 right(origin + cinolib::GLcanvas::world_right * radius);
		const RVec3 up(origin + cinolib::GLcanvas::world_up * radius);
		const RVec3 forward(origin + cinolib::GLcanvas::world_forward * radius);
		const auto project{ [this](const RVec3& _point) -> RVec3 {
			RVec3 proj(m_camera->projectionViewMatrix() * _point);
			proj.x() *= m_camera->projection.aspectRatio;
			const Real size{ 100 };
			const ImVec2 windowOrigin{ ImGui::GetWindowPos() };
			const ImVec2 windowSize{ ImGui::GetWindowSize() };
			proj.x() = proj.x() * size + windowOrigin.x + windowSize.x - size;
			proj.y() = -proj.y() * size + windowOrigin.y + windowSize.y - size;
			return proj;
		} };
		constexpr auto toImVec{ [](const RVec3& _vec) {
			return ImVec2{static_cast<float>(_vec.x()), static_cast<float>(_vec.y())};
		} };
		origin = project(origin);
		std::array<std::pair<RVec3, ImColor>, 3> tips{
			std::pair<RVec3, ImColor>{project(right), IM_COL32(255,100,100,255)},
			std::pair<RVec3, ImColor>{project(up), IM_COL32(100,255,100,255)},
			std::pair<RVec3, ImColor>{project(forward), IM_COL32(100,100,255,255)}
		};
		std::sort(tips.begin(), tips.end(), [](const std::pair<RVec3, ImColor>& _a, const std::pair<RVec3, ImColor>& _b) { return _a.first.z() > _b.first.z(); });
		for (const auto& [tip, color] : tips)
		{
			drawList.AddLine(toImVec(origin), toImVec(tip), color, 3);
			drawList.AddCircleFilled(toImVec(tip), 5, color, 6);
		}
	}

}