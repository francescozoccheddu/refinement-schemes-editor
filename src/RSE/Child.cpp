#include <RSE/Child.hpp>

#include <stdexcept>
#include <imgui.h>
#include <sstream>
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
		// actions
		if (ImGui::Button("X"))
		{
			return EResult::Removed;
		}
		ImGui::SameLine();
		if (ImGui::SmallButton("C"))
		{
			std::ostringstream ss;
			for (const IVec& vert : m_verts)
			{
				ss << vert << "\n";
			}
			glfwSetClipboardString(nullptr, ss.str().c_str());
		}
		const char* const clipboard{ glfwGetClipboardString(nullptr) };
		if (clipboard)
		{
			ImGui::SameLine();
			if (ImGui::SmallButton("P"))
			{
				std::istringstream ss{ clipboard };
				for (IVec& vert : m_verts)
				{
					ss >> vert;
					ss.get();
				}
				update();
				return EResult::Updated;
			}
		}
		// visibility
		{
			ImGui::SameLine();
			bool wasVisible{ m_visible }, wasSolo{ m_solo };
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (_anySolo && !m_solo) ? 0.5f : 1.0f);
			ImGui::Checkbox("V", &m_visible);
			ImGui::PopStyleVar(1);
			if (m_visible != wasVisible)
			{
				m_solo &= m_visible;
				return EResult::Updated;
			}
			ImGui::SameLine();
			ImGui::Checkbox("S", &m_solo);
			if (m_solo != wasSolo)
			{
				m_visible |= m_solo;
				return EResult::Updated;
			}
		}
		// vertices
		ImGui::SameLine();
		ImGui::SetNextItemOpen(m_expanded, ImGuiCond_Once);
		m_expanded = ImGui::TreeNode("Verts", "Verts%s", m_valid ? "" : " (!)");
		if (m_expanded)
		{
			ImGui::Spacing();
			bool updated{ false };
			for (std::size_t i{}; i < m_verts.size(); i++)
			{
				ImGui::PushID(i);
				IVec& vert{ m_verts[i] };
				int xyz[3]{ vert.x(), vert.y(), vert.z() };
				ImGui::TextDisabled("%d", i);
				ImGui::SameLine();
				if (ImGui::DragInt3("", xyz, 1.0f / 20.0f, 0, _size, "%d", ImGuiSliderFlags_AlwaysClamp))
				{
					vert.x() = xyz[0];
					vert.y() = xyz[1];
					vert.z() = xyz[2];
					updated = true;
				}
				ImGui::SameLine();
				if (ImGui::SmallButton("C"))
				{
					std::ostringstream ss;
					ss << vert;
					glfwSetClipboardString(nullptr, ss.str().c_str());
				}
				if (clipboard)
				{
					ImGui::SameLine();
					if (ImGui::SmallButton("P"))
					{
						std::istringstream ss{ clipboard };
						ss >> vert;
						updated = true;
					}
				}
				ImGui::PopID();
			}
			if (updated)
			{
				update();
			}
			if (!m_valid)
			{
				ImGui::Spacing();
				ImGui::TextDisabled("Duplicated vertices!");
			}
			ImGui::Spacing();
			ImGui::TreePop();
			if (updated)
			{
				return EResult::Updated;
			}
		}
		return EResult::None;
	}

}