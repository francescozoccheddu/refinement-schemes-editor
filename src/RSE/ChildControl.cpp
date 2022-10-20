#include <RSE/ChildControl.hpp>

#include <stdexcept>
#include <imgui.h>
#include <sstream>
#include <string_view>
#include <algorithm>

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

	ChildControl::ChildControl(Int _size) : m_expanded{ false }, m_visible{ true }, m_solo{ false }, m_hexControl{ IHexControl::cubeVerts(0,_size), true }, m_maxSize{ _size }, m_style{ 0.0f,0.0f,0.5f }
	{
		if (_size <= 0)
		{
			throw std::logic_error{ "size must be positive" };
		}
	}

	Style& ChildControl::style()
	{
		return m_style;
	}

	const Style& ChildControl::style() const
	{
		return m_style;
	}

	const IHexControl& ChildControl::hexControl() const
	{
		return m_hexControl;
	}

	bool ChildControl::visible() const
	{
		return m_visible;
	}

	bool ChildControl::solo() const
	{
		return m_solo;
	}

	bool ChildControl::expanded() const
	{
		return m_expanded;
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


	void ChildControl::setExpanded(bool _expanded)
	{
		m_expanded = _expanded;
	}

	void ChildControl::setSolo(bool _solo)
	{
		m_visible |= m_solo = _solo;
	}

	void ChildControl::setVisible(bool _visible)
	{
		m_solo &= m_visible = _visible;
	}

	Int ChildControl::maxSize() const
	{
		return m_maxSize;
	}

	ChildControl::EResult ChildControl::draw(Int _size, EVisibilityMode _visibilityMode)
	{
		return draw(_size, IHexControl::pasteVerts(), IHexControl::pasteVert(), _visibilityMode);
	}

	ChildControl::EResult ChildControl::draw(Int _size, const std::optional<HexVertsU>& _copiedVerts, const std::optional<IVec3>& _copiedVert, EVisibilityMode _visibilityMode)
	{
		m_style.pushImGui();
		if (_size < m_maxSize)
		{
			throw std::logic_error{ "size < maxSize" };
		}
		bool updated{ false };
		// visibility
		if (_visibilityMode != EVisibilityMode::Hidden)
		{
			const ImGuiStyle& style{ ImGui::GetStyle() };
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.ItemSpacing.x / 2, style.ItemSpacing.y });
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style.Alpha * (_visibilityMode == EVisibilityMode::SomeSolo && !m_solo) ? 0.5f : 1.0f);
			ImGui::Checkbox("##visible", &m_visible);
			m_solo &= m_visible;
			ImGui::PopStyleVar();
			ImGui::SameLine();
			ImGui::Checkbox("##solo", &m_solo);
			m_visible |= m_solo;
			ImGui::PopStyleVar();
			ImGui::SameLine();
		}
		// header
		ImGui::SetNextItemOpen(m_expanded, ImGuiCond_Always);
		if (!m_hexControl.valid())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f,1.0f,0.0f,1.0f });
		}
		bool keep{ true };
		const bool wasExpanded{ m_expanded };
		m_expanded = ImGui::CollapsingHeader("Verts", &keep);
		if (!m_hexControl.valid())
		{
			ImGui::PopStyleColor();
		}
		// vertices
		if (m_expanded)
		{
			if (!wasExpanded)
			{
				m_hexControl.setActiveVert(0);
			}
			ImGui::Spacing();
			updated |= m_hexControl.draw(true, 0, _size, _copiedVerts, _copiedVert);
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
		return updated ? EResult::Updated : EResult::None;
	}

}