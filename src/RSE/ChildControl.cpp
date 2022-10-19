#include <RSE/ChildControl.hpp>

#include <stdexcept>
#include <imgui.h>
#include <sstream>
#include <string_view>
#include <algorithm>
#include <glfw/glfw3.h>

namespace RSE
{

	void ChildControl::update()
	{
		m_maxSize = 0;
		for (const IVec& vert : m_polyControl.verts())
		{
			m_maxSize = std::max({ m_maxSize, vert.x(), vert.y(), vert.z() });
		}
	}

	ChildControl::ChildControl(Int _size) : m_expanded{ false }, m_visible{ true }, m_solo{ false }, m_polyControl{ IPolyControl::cubeVerts(0,_size), true }, m_maxSize{ _size }
	{
		if (_size <= 0)
		{
			throw std::logic_error{ "size must be positive" };
		}
	}

	const IPolyControl& ChildControl::polyControl() const
	{
		return m_polyControl;
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

	void ChildControl::setVerts(const PolyVertsU& _verts)
	{
		m_polyControl.setVerts(_verts);
		update();
	}

	void ChildControl::setActiveVert(std::optional<std::size_t> _index)
	{
		m_polyControl.setActiveVert(_index);
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

	ChildControl::EResult ChildControl::draw(Int _size, bool _anySolo, const std::optional<PolyVertsU>& _copiedVerts, const std::optional<IVec>& _copiedVert)
	{
		if (_size < m_maxSize)
		{
			throw std::logic_error{ "size < maxSize" };
		}
		bool updated{ false };
		// visibility
		{
			const ImGuiStyle& style{ ImGui::GetStyle() };
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.ItemSpacing.x / 2, style.ItemSpacing.y });
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, style.Alpha * (_anySolo && !m_solo) ? 0.5f : 1.0f);
			ImGui::Checkbox("##visible", &m_visible);
			m_solo &= m_visible;
			ImGui::PopStyleVar();
			ImGui::SameLine();
			ImGui::Checkbox("##solo", &m_solo);
			m_visible |= m_solo;
			ImGui::PopStyleVar();
		}
		// header
		ImGui::SameLine();
		ImGui::SetNextItemOpen(m_expanded, ImGuiCond_Always);
		if (!m_polyControl.valid())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f,1.0f,0.0f,1.0f });
		}
		bool keep{ true };
		m_expanded = ImGui::CollapsingHeader("Verts", &keep);
		if (!m_polyControl.valid())
		{
			ImGui::PopStyleColor();
		}
		// vertices
		if (m_expanded)
		{
			ImGui::Spacing();
			updated |= m_polyControl.draw(0, _size, _copiedVerts, _copiedVert);
		}
		else
		{
			m_polyControl.setActiveVert(std::nullopt);
		}
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