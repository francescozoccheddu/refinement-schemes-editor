#ifndef RSE_HEXCONTROL_TPP
#error __FILE__ cannot be included directly
#endif

#include <RSE/HexControl.hpp>

#include <RSE/hexUtils.hpp>
#include <cpputils/serialization/StringSerializer.hpp>
#include <cpputils/serialization/StringDeserializer.hpp>
#include <imgui.h>
#include <cinolib/gl/gl_glfw.h>
#include <stdexcept>

namespace RSE::internal
{

	template<bool TInt>
	void HexControl<TInt>::update()
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

	template<bool TInt>
	HexControl<TInt>::Verts HexControl<TInt>::cubeVerts(const Vert& _min, const Vert& _max)
	{
		return hexUtils::cubeVerts<Value>(_min, _max);
	}

	template<bool TInt>
	HexControl<TInt>::HexControl(const Verts& _verts, bool _vertSelection) :
		m_verts{ _verts },
		m_ids{ 0,1,2,3,4,5,6,7 },
		m_valid{},
		m_activeVert{}
	{
		update();
	}

	template<bool TInt>
	void HexControl<TInt>::copyVert(const Vert& _vert)
	{
		cpputils::serialization::StringSerializer s{};
		s.serializer() << _vert.x() << _vert.y() << _vert.z();
		glfwSetClipboardString(nullptr, (std::string{ c_cbVertPrefix } + s.string()).c_str());
	}

	template<bool TInt>
	std::optional<typename HexControl<TInt>::Vert> HexControl<TInt>::pasteVert()
	{
		const char* const clipboard{ glfwGetClipboardString(nullptr) };
		if (clipboard)
		{
			const std::string_view clipboardStr{ clipboard };
			if (clipboardStr.starts_with(c_cbVertPrefix))
			{
				cpputils::serialization::StringDeserializer s{ clipboardStr.substr(c_cbVertPrefix.size()) };
				Vert vert;
				s.deserializer() >> vert.x() >> vert.y() >> vert.z();
				return vert;
			}
		}
		return std::nullopt;
	}

	template<bool TInt>
	void HexControl<TInt>::copyVerts(const Verts& _verts)
	{
		cpputils::serialization::StringSerializer s{};
		for (const Vert& vert : _verts)
		{
			s.serializer() << vert.x() << vert.y() << vert.z();
		}
		glfwSetClipboardString(nullptr, (std::string{ c_cbVertsPrefix } + s.string()).c_str());
	}

	template<bool TInt>
	std::optional<typename HexControl<TInt>::Verts> HexControl<TInt>::pasteVerts()
	{
		const char* const clipboard{ glfwGetClipboardString(nullptr) };
		if (clipboard)
		{
			const std::string_view clipboardStr{ clipboard };
			if (clipboardStr.starts_with(c_cbVertsPrefix))
			{
				cpputils::serialization::StringDeserializer s{ clipboardStr.substr(c_cbVertsPrefix.size()) };
				Verts verts;
				for (Vert& vert : verts)
				{
					s.deserializer() >> vert.x() >> vert.y() >> vert.z();
				}
				return verts;
			}
		}
		return std::nullopt;
	}

	template<bool TInt>
	std::optional<HexControl<TInt>> HexControl<TInt>::paste()
	{
		std::optional<Verts> verts{ pasteVerts() };
		if (verts.has_value())
		{
			return HexControl{ verts.value() };
		}
		return std::nullopt;
	}

	template<bool TInt>
	void HexControl<TInt>::copy() const
	{
		copyVerts(m_verts);
	}

	template<bool TInt>
	void HexControl<TInt>::setVerts(const Verts& _verts)
	{
		m_verts = _verts;
		update();
	}

	template<bool TInt>
	void HexControl<TInt>::setActiveVert(std::size_t _index)
	{
		if (_index >= m_verts.size())
		{
			throw std::domain_error{ "out of range" };
		}
		m_activeVert = _index;
	}

	template<bool TInt>
	void HexControl<TInt>::setActiveVert(const Vert& _vert)
	{
		m_verts[m_activeVert] = _vert;
		update();
	}

	template<bool TInt>
	std::size_t HexControl<TInt>::activeVert() const
	{
		return m_activeVert;
	}

	template<bool TInt>
	const typename HexControl<TInt>::Verts& HexControl<TInt>::verts() const
	{
		return m_verts;
	}

	template<bool TInt>
	bool HexControl<TInt>::valid() const
	{
		return m_valid;
	}

	template<bool TInt>
	HexVertData<std::size_t> HexControl<TInt>::firstOccurrenceIndices() const
	{
		HexVertData<std::size_t> indices;
		for (std::size_t i{}; i < m_verts.size(); i++)
		{
			indices[i] = i;
			for (std::size_t j{ 0 }; j < i; j++)
			{
				if (m_verts[i] == m_verts[j])
				{
					indices[i] = j;
					break;
				}
			}
		}
		return indices;
	}

	template<bool TInt>
	bool HexControl<TInt>::draw(bool _activeVertSel, const Vert& _min, const Vert& _max, std::optional<Verts>& _copiedVerts, std::optional<Vert>& _copiedVert)
	{
		bool updated{ false };
		// cube
		if (ImGui::SmallButton("Cube"))
		{
			m_verts = cubeVerts(_min, _max);
			updated = true;
		}
		if (m_valid)
		{
			ImGui::SameLine();
			if (ImGui::SmallButton("Sort"))
			{
				m_verts = hexUtils::sortVerts(m_verts);
				updated = true;
			}
		}
		// copy/paste
		ImGui::SameLine();
		if (ImGui::SmallButton("C"))
		{
			_copiedVerts = m_verts;
		}
		if (_copiedVerts.has_value())
		{
			ImGui::SameLine();
			if (ImGui::SmallButton("P"))
			{
				m_verts = _copiedVerts.value();
				updated = true;
			}
		}
		// verts
		ImGui::Spacing();
		const ImVec2 lineSize{ ImGui::GetColumnWidth(), ImGui::GetFrameHeight() };
		const float textYOffs{ (lineSize.y - ImGui::GetTextLineHeight()) / 2 };
		const HexVertData<std::size_t> firstIs{ firstOccurrenceIndices() };
		for (std::size_t i{}; i < m_verts.size(); i++)
		{
			ImGui::PushID(m_ids[i]);
			// handle
			const ImVec2 cursor{ ImGui::GetCursorPos() };
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.2f);
			bool selected{ false };
			ImGui::Selectable("##handle", &selected, ImGuiSelectableFlags_AllowItemOverlap, lineSize);
			const bool dragging{ ImGui::IsItemActive() };
			if (dragging)
			{
				const float dragDeltaItems{ (ImGui::GetMousePos().y - cursor.y) / lineSize.y };
				const int targetI = std::clamp(static_cast<int>(i + std::floor(dragDeltaItems)), 0, static_cast<int>(m_verts.size() - 1));
				std::swap(m_ids[i], m_ids[targetI]);
				std::swap(m_verts[i], m_verts[targetI]);
				if (i != targetI)
				{
					updated = true;
				}
			}
			ImGui::PopStyleVar();
			ImGui::SetCursorPos(cursor);
			// text
			ImGui::SetCursorPosY(cursor.y + textYOffs);
			if (dragging)
			{
				ImGui::Text("  <--- %d", i);
			}
			else
			{
				if (firstIs[i] == i)
				{
					ImGui::TextDisabled("--- %d", i);
				}
				else
				{
					ImGui::TextColored(ImVec4{ 1.0f,1.0f,0.0f,1.0f }, "--- %d", firstIs[i]);
				}
			}
			ImGui::SameLine();
			ImGui::SetCursorPosY(cursor.y);
			// selection
			if (_activeVertSel)
			{
				int activeI{ static_cast<int>(m_activeVert) };
				if (ImGui::RadioButton("", &activeI, static_cast<int>(i)))
				{
					m_activeVert = i;
				}
				ImGui::SameLine();
			}
			// value
			Vert& vert{ m_verts[i] };
			{
				using ImGuiValue = std::conditional_t<TInt, int, float>;
				const Value minMin{ std::min({_min.x(), _min.y(), _min.z()}) };
				const Value maxMax{ std::max({ _max.x(), _max.y(), _max.z() }) };
				ImGuiValue xyz[3]{ static_cast<ImGuiValue>(vert.x()), static_cast<ImGuiValue>(vert.y()), static_cast<ImGuiValue>(vert.z()) };
				const float speed{ static_cast<float>(maxMax - minMin) / 100.0f };
				bool vertUpdated;
				if constexpr (TInt)
				{
					vertUpdated = ImGui::DragInt3("", xyz, speed, static_cast<int>(minMin), static_cast<int>(maxMax), "%d", ImGuiSliderFlags_AlwaysClamp);
				}
				else
				{
					vertUpdated = ImGui::DragFloat3("", xyz, speed, static_cast<float>(minMin), static_cast<float>(maxMax), "%f", ImGuiSliderFlags_AlwaysClamp);
				}
				if (vertUpdated)
				{
					for (unsigned int d{}; d < 3; d++)
					{
						vert[d] = std::clamp(static_cast<Value>(xyz[d]), _min[d], _max[d]);
					}
					updated = true;
				}
			}
			// copy/paste
			ImGui::SameLine();
			if (ImGui::SmallButton("C"))
			{
				_copiedVert = vert;
			}
			if (_copiedVert.has_value())
			{
				ImGui::SameLine();
				if (ImGui::SmallButton("P"))
				{
					updated = vert != _copiedVert.value();
					vert = _copiedVert.value();
				}
			}
			ImGui::PopID();
		}
		if (updated)
		{
			update();
		}
		return updated;
	}

	template<bool TInt>
	bool HexControl<TInt>::draw(bool _activeVertSel, const Vert& _min, const Vert& _max, const std::optional<Verts>& _copiedVerts, const std::optional<Vert>& _copiedVert)
	{
		std::optional<Verts> tempVerts{ _copiedVerts };
		std::optional<Vert> tempVert{ _copiedVert };
		const bool updated{ draw(_activeVertSel, _min, _max, tempVerts, tempVert) };
		if (tempVert.has_value() && tempVert != _copiedVert)
		{
			copyVert(tempVert.value());
		}
		if (tempVerts.has_value() && tempVerts != _copiedVerts)
		{
			copyVerts(tempVerts.value());
		}
		return updated;
	}

	template<bool TInt>
	bool HexControl<TInt>::draw(bool _activeVertSel, const Vert& _min, const Vert& _max)
	{
		return draw(_activeVertSel, _min, _max, pasteVerts(), pasteVert());
	}

	template<bool TInt>
	bool HexControl<TInt>::draw(bool _activeVertSel, Value _min, Value _max)
	{
		return draw(_activeVertSel, Vert{_min, _min, _min}, Vert{_max, _max, _max});
	}

}

