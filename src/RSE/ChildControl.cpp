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

	ChildControl::ChildControl(Int _size) : m_expanded{ false }, m_visible{ true }, m_solo{ false }, m_polyControl{ IPolyControl::cube(_size) }, m_maxSize{ _size }
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
		// actions
		{
			if (ImGui::Button("X"))
			{
				return EResult::Removed;
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("C"))
			{
				m_polyControl.copy();
			}
			if (_copiedVerts.has_value())
			{
				ImGui::SameLine();
				if (ImGui::SmallButton("P"))
				{
					m_polyControl.setVerts(_copiedVerts.value());
					updated = true;
				}
			}
		}
		// visibility
		{
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (_anySolo && !m_solo) ? 0.5f : 1.0f);
			ImGui::Checkbox("V", &m_visible);
			ImGui::PopStyleVar();
			m_solo &= m_visible;
			ImGui::SameLine();
			ImGui::Checkbox("S", &m_solo);
			m_visible |= m_solo;
		}
		// vertices
		{
			ImGui::SameLine();
			ImGui::SetNextItemOpen(m_expanded, ImGuiCond_Once);
			if (!m_polyControl.valid())
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f,1.0f,0.0f,1.0f });
			}
			m_expanded = ImGui::TreeNode("Verts", "Verts%s", m_polyControl.valid() ? "" : " (!)");
			if (!m_polyControl.valid())
			{
				ImGui::PopStyleColor();
			}
			if (m_expanded)
			{
				ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
				ImGui::Spacing();
				updated |= m_polyControl.draw(0, _size, _copiedVert);
				ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
				ImGui::TreePop();
			}
		}
		if (updated)
		{
			update();
		}
		return updated ? EResult::Updated : EResult::None;
	}

}