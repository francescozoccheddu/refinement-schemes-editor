#include <RSE/ChildControl.hpp>

#include <stdexcept>
#include <imgui.h>
#include <sstream>
#include <string_view>
#include <algorithm>
#include <random>

namespace RSE
{

	void ChildControl::update()
	{
		m_maxSize = 0;
		for (const IVec3& vert : m_hexControl.verts())
		{
			m_maxSize = std::max({ m_maxSize, vert.x(), vert.y(), vert.z() });
		}
	}

	ChildControl::ChildControl(const HexVertsU& _verts) : m_active{ false }, m_visible{ true }, m_selected{ false }, m_hexControl{ _verts, true }, m_style{ 0.0f,0.0f,0.5f }, m_maxSize{}
	{
		for (const IVec3& vert : _verts)
		{
			m_maxSize = std::max({ m_maxSize, vert.x(), vert.y(), vert.z() });
		}
	}

	ChildControl::ChildControl(const IVec3& _min, const IVec3& _max) : ChildControl{ IHexControl::cubeVerts(_min, _max) }
	{}

	Style& ChildControl::style()
	{
		return m_style;
	}

	const Style& ChildControl::style() const
	{
		return m_style;
	}

	void ChildControl::randomColor()
	{
		std::random_device device;
		std::mt19937 gen(device());
		std::uniform_int_distribution<> distr(25, 63);
		m_style = Style{
			std::uniform_real_distribution<float>(0.0f,360.0f)(gen),
			std::uniform_real_distribution<float>(0.5f,1.0f)(gen),
			std::uniform_real_distribution<float>(0.8f,1.0f)(gen)
		};
	}

	const IHexControl& ChildControl::hexControl() const
	{
		return m_hexControl;
	}

	bool ChildControl::visible() const
	{
		return m_visible;
	}

	bool ChildControl::selected() const
	{
		return m_selected;
	}

	bool ChildControl::active() const
	{
		return m_active;
	}

	void ChildControl::setVerts(const HexVertsU& _verts)
	{
		m_hexControl.setVerts(_verts);
		update();
	}

	void ChildControl::setActiveVert(std::size_t _index)
	{
		m_hexControl.setActiveVert(_index);
	}

	void ChildControl::setActiveVert(const IVec3& _vert)
	{
		m_hexControl.setActiveVert(_vert);
		update();
	}

	void ChildControl::setActive(bool _active)
	{
		m_active = _active;
	}

	void ChildControl::setSelected(bool _selected)
	{
		m_visible |= m_selected = _selected;
	}

	void ChildControl::setVisible(bool _visible)
	{
		m_selected &= m_visible = _visible;
	}

	Int ChildControl::maxSize() const
	{
		return m_maxSize;
	}

	ChildControl::EResult ChildControl::draw(const IVec3& _min, const IVec3& _max, const std::optional<HexVertsU>& _copiedVerts, const std::optional<IVec3>& _copiedVert, EVisibilityMode _visibilityMode)
	{
		m_style.pushImGui();
		bool updated{ false }, colorUpdated{ false };
		// visibility
		if (_visibilityMode != EVisibilityMode::Hidden)
		{
			const ImGuiStyle& style{ ImGui::GetStyle() };
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.ItemSpacing.x / 2, style.ItemSpacing.y });
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style.Alpha * (_visibilityMode == EVisibilityMode::SomeSelected && !m_selected) ? 0.5f : 1.0f);
			ImGui::Checkbox("##visible", &m_visible);
			m_selected &= m_visible;
			ImGui::PopStyleVar();
			ImGui::SameLine();
			ImGui::Checkbox("##selected", &m_selected);
			m_visible |= m_selected;
			ImGui::PopStyleVar();
			ImGui::SameLine();
		}
		// header
		ImGui::SetNextItemOpen(m_active, ImGuiCond_Always);
		if (!m_hexControl.valid())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f,1.0f,0.0f,1.0f });
		}
		bool keep{ true };
		const bool wasActive{ m_active };
		m_active = ImGui::CollapsingHeader("Verts", &keep);
		if (!m_hexControl.valid())
		{
			ImGui::PopStyleColor();
		}
		// vertices
		if (m_active)
		{
			if (!wasActive)
			{
				m_hexControl.setActiveVert(0);
			}
			ImGui::Spacing();
			updated |= m_hexControl.draw(true, _min, _max, _copiedVerts, _copiedVert);
			ImGui::Spacing();
			if (ImGui::SmallButton("Random color"))
			{
				randomColor();
				colorUpdated = true;
			}
		}
		Style::popImGui();
		if (updated)
		{
			update();
		}
		if (!keep)
		{
			return EResult::Removed;
		}
		return updated || colorUpdated ? EResult::Updated : EResult::None;
	}

}