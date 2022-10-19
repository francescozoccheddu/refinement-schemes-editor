#include <RSE/Child.hpp>

#include <stdexcept>
#include <imgui.h>
#include <sstream>
#include <string_view>
#include <algorithm>
#include <glfw/glfw3.h>

namespace RSE
{

	void Child::updateValid()
	{
		m_valid = true;
		for (std::size_t i{}; i < m_verts.size(); i++)
		{
			for (std::size_t j{ i + 1 }; j < m_verts.size(); j++)
			{
				if (m_verts[i] == m_verts[j])
				{
					m_valid = false;
					return;
				}
			}
		}
	}

	void Child::updateMaxSize()
	{
		m_maxSize = 0;
		for (const IVec& vert : m_verts)
		{
			m_maxSize = std::max({ m_maxSize, vert.x(), vert.y(), vert.z() });
		}
	}

	void Child::update()
	{
		updateValid();
		updateMaxSize();
	}

	Child::Child(Int _size) :
		m_valid{ true }, m_expanded{ false }, m_visible{ true }, m_solo{ false }, m_maxSize{ _size },
		m_verts{ IVec{0,0,0}, IVec{0,0,_size}, IVec{0,_size,0}, IVec{0,_size,_size}, IVec{_size,0,0}, IVec{_size,0,_size}, IVec{_size,_size,0}, IVec{_size,_size,_size} }
	{
		if (_size <= 0)
		{
			throw std::logic_error{ "size must be positive" };
		}
		for (char i{}; i < m_verts.size(); i++)
		{
			m_vertIds[i] = i;
		}
	}

	const PolyVertsU& Child::verts() const
	{
		return m_verts;
	}

	bool Child::visible() const
	{
		return m_visible;
	}

	bool Child::solo() const
	{
		return m_solo;
	}

	bool Child::expanded() const
	{
		return m_expanded;
	}

	bool Child::valid() const
	{
		return m_valid;
	}

	void Child::setVerts(const PolyVertsU& _verts)
	{
		m_verts = _verts;
		update();
	}

	void Child::setExpanded(bool _expanded)
	{
		m_expanded = _expanded;
	}

	void Child::setSolo(bool _solo)
	{
		m_visible |= m_solo = _solo;
	}

	void Child::setVisible(bool _visible)
	{
		m_solo &= m_visible = _visible;
	}

	Int Child::maxSize() const
	{
		return m_maxSize;
	}

	Child::EResult Child::draw(Int _size, bool _anySolo)
	{
		if (_size < m_maxSize)
		{
			throw std::logic_error{ "size < maxSize" };
		}
		bool updated{ false };
		// clipboard
		constexpr std::string_view childCbPrefix{ "rse_child_cb_child" };
		constexpr std::string_view vertCbPrefix{ "rse_child_cb_vert" };
		const char* const clipboard{ glfwGetClipboardString(nullptr) };
		const std::string_view clipboardView{ clipboard ? clipboard : "" };
		const bool canPasteChild{ clipboard && clipboardView.starts_with(childCbPrefix) };
		const bool canPasteVert{ clipboard && clipboardView.starts_with(vertCbPrefix) };
		// actions
		{
			if (ImGui::Button("X"))
			{
				return EResult::Removed;
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("C"))
			{
				std::ostringstream ss;
				ss << childCbPrefix;
				for (const IVec& vert : m_verts)
				{
					ss << vert << "\n";
				}
				glfwSetClipboardString(nullptr, ss.str().c_str());
			}
			ImGui::SameLine();
			if (!canPasteChild)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::SmallButton("P") && canPasteChild)
			{
				std::istringstream ss{ clipboard };
				for (std::size_t i{}; i < childCbPrefix.size(); i++)
				{
					ss.get();
				}
				for (IVec& vert : m_verts)
				{
					ss >> vert;
					ss.get();
				}
				update();
				updated = true;
			}
			if (!canPasteChild)
			{
				ImGui::PopStyleVar();
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
			m_expanded = ImGui::TreeNode("Verts", "Verts%s", m_valid ? "" : " (!)");
			if (m_expanded)
			{
				ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
				ImGui::Spacing();
				bool manuallyUpdated{ false };
				const ImVec2 lineSize{ ImGui::GetColumnWidth(), ImGui::GetFrameHeight() };
				const float textYOffs{ (lineSize.y - ImGui::GetTextLineHeight()) / 2 };
				for (std::size_t i{}; i < m_verts.size(); i++)
				{
					ImGui::PushID(m_vertIds[i]);
					IVec& vert{ m_verts[i] };
					int xyz[3]{ vert.x(), vert.y(), vert.z() };
					bool s;
					const ImVec2 cursor{ ImGui::GetCursorPos() };
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.2f);
					ImGui::Selectable("##handle", &s, ImGuiSelectableFlags_AllowItemOverlap, lineSize);
					const bool dragging{ ImGui::IsItemActive() };
					if (dragging)
					{
						const float dragDeltaItems{ (ImGui::GetMousePos().y - cursor.y) / lineSize.y };
						const int targetI = std::clamp(static_cast<int>(i + std::floorf(dragDeltaItems)), 0, static_cast<int>(m_verts.size() - 1));
						std::swap(m_vertIds[i], m_vertIds[targetI]);
						std::swap(m_verts[i], m_verts[targetI]);
						if (i != targetI)
						{
							manuallyUpdated = true;
						}
					}
					ImGui::PopStyleVar();
					ImGui::SetCursorPos(cursor);
					ImGui::SetCursorPosY(cursor.y + textYOffs);
					ImGui::TextDisabled("%s--- %d", dragging ? "  <" : "", i);
					ImGui::SameLine();
					ImGui::SetCursorPosY(cursor.y);
					if (ImGui::DragInt3("", xyz, 1.0f / 20.0f, 0, _size, "%d", ImGuiSliderFlags_AlwaysClamp))
					{
						vert.x() = xyz[0];
						vert.y() = xyz[1];
						vert.z() = xyz[2];
						manuallyUpdated = true;
					}
					ImGui::SameLine();
					if (ImGui::SmallButton("C"))
					{
						std::ostringstream ss;
						ss << vertCbPrefix;
						ss << vert;
						glfwSetClipboardString(nullptr, ss.str().c_str());
					}
					ImGui::SameLine();
					if (!canPasteVert)
					{
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}
					if (ImGui::SmallButton("P") && canPasteVert)
					{
						std::istringstream ss{ clipboard };
						for (std::size_t i{}; i < vertCbPrefix.size(); i++)
						{
							ss.get();
						}
						ss >> vert;
						manuallyUpdated = true;
					}
					if (!canPasteVert)
					{
						ImGui::PopStyleVar();
					}
					ImGui::PopID();
				}
				if (manuallyUpdated)
				{
					update();
					updated = true;
				}
				if (!m_valid)
				{
					ImGui::Spacing();
					ImGui::TextDisabled("Duplicated vertices!");
				}
				ImGui::Spacing();
				ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
				ImGui::TreePop();
			}
		}
		return updated ? EResult::Updated : EResult::None;
	}

}