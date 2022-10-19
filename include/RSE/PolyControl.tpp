#ifndef RSE_POLYCONTROL_TPP
#error __FILE__ cannot be included directly
#endif

#include <RSE/PolyControl.hpp>

#include <imgui.h>
#include <glfw/glfw3.h>
#include <sstream>

namespace RSE::internal
{

	template<bool TInt>
	void PolyControl<TInt>::update()
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
	PolyControl<TInt>::Verts PolyControl<TInt>::cubeVerts(Value _size)
	{
		return Verts{ Vert{ 0,0,0 }, Vert{ 0,0,_size }, Vert{ 0,_size,0 }, Vert{ 0,_size,_size }, Vert{ _size,0,0 }, Vert{ _size,0,_size }, Vert{ _size,_size,0 }, Vert{ _size,_size,_size } };
	}

	template<bool TInt>
	PolyControl<TInt> PolyControl<TInt>::cube(Value _size)
	{
		return PolyControl{ cubeVerts(_size) };
	}

	template<bool TInt>
	PolyControl<TInt>::PolyControl(const Verts& _verts) :
		m_verts{ _verts },
		m_ids{ 0,1,2,3,4,5,6,7 },
		m_valid{}
	{
		update();
	}

	template<bool TInt>
	void PolyControl<TInt>::copyVert(const Vert& _vert)
	{
		std::ostringstream ss{};
		ss << c_cbVertPrefix;
		ss << _vert;
		glfwSetClipboardString(nullptr, ss.str().c_str());
	}

	template<bool TInt>
	std::optional<typename PolyControl<TInt>::Vert> PolyControl<TInt>::pasteVert()
	{
		const char* const clipboard{ glfwGetClipboardString(nullptr) };
		if (clipboard)
		{
			const std::string_view clipboardStr{ clipboard };
			if (clipboardStr.starts_with(c_cbVertPrefix))
			{
				Vert vert;
				std::istringstream ss{ clipboard + c_cbVertPrefix.size() };
				ss >> vert;
				return vert;
			}
		}
		return std::nullopt;
	}

	template<bool TInt>
	void PolyControl<TInt>::copyVerts(const Verts& _verts)
	{
		std::ostringstream ss{};
		ss << c_cbVertsPrefix;
		for (const Vert& vert : _verts)
		{
			ss << vert << "\n";
		}
		glfwSetClipboardString(nullptr, ss.str().c_str());
	}

	template<bool TInt>
	std::optional<typename PolyControl<TInt>::Verts> PolyControl<TInt>::pasteVerts()
	{
		const char* const clipboard{ glfwGetClipboardString(nullptr) };
		if (clipboard)
		{
			const std::string_view clipboardStr{ clipboard };
			if (clipboardStr.starts_with(c_cbVertsPrefix))
			{
				Verts verts;
				std::istringstream ss{ clipboard + c_cbVertsPrefix.size() };
				for (Vert& vert : verts)
				{
					ss >> vert;
				}
				return verts;
			}
		}
		return std::nullopt;
	}

	template<bool TInt>
	std::optional<typename PolyControl<TInt>> PolyControl<TInt>::paste()
	{
		std::optional<Verts> verts{ pasteVerts() };
		if (verts.has_value())
		{
			return PolyControl{ verts.value() };
		}
		return std::nullopt;
	}

	template<bool TInt>
	void PolyControl<TInt>::copy() const
	{
		copyVerts(m_verts);
	}

	template<bool TInt>
	void PolyControl<TInt>::setVerts(const Verts& _verts)
	{
		m_verts = _verts;
		update();
	}

	template<bool TInt>
	const typename PolyControl<TInt>::Verts& PolyControl<TInt>::verts() const
	{
		return m_verts;
	}

	template<bool TInt>
	bool PolyControl<TInt>::valid() const
	{
		return m_valid;
	}

	template<bool TInt>
	PolyVertData<std::size_t> PolyControl<TInt>::firstOccurrenceIndices() const
	{
		PolyVertData<std::size_t> indices;
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
	bool PolyControl<TInt>::draw(Value _min, Value _max, std::optional<Vert>& _copiedVert, bool& _copied)
	{
		if (_min > _max)
		{
			throw std::logic_error{ "min > max" };
		}
		_copied = false;
		bool updated{ false };
		const ImVec2 lineSize{ ImGui::GetColumnWidth(), ImGui::GetFrameHeight() };
		const float textYOffs{ (lineSize.y - ImGui::GetTextLineHeight()) / 2 };
		const PolyVertData<std::size_t> firstIs{ firstOccurrenceIndices() };
		for (std::size_t i{}; i < m_verts.size(); i++)
		{
			ImGui::PushID(m_ids[i]);
			Vert& vert{ m_verts[i] };
			std::conditional_t<TInt, int, float> xyz[3]{ static_cast<Value>(vert.x()), static_cast<Value>(vert.y()), static_cast<Value>(vert.z()) };
			const ImVec2 cursor{ ImGui::GetCursorPos() };
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.2f);
			bool selected;
			ImGui::Selectable("##handle", &selected, ImGuiSelectableFlags_AllowItemOverlap, lineSize);
			const bool dragging{ ImGui::IsItemActive() };
			if (dragging)
			{
				const float dragDeltaItems{ (ImGui::GetMousePos().y - cursor.y) / lineSize.y };
				const int targetI = std::clamp(static_cast<int>(i + std::floorf(dragDeltaItems)), 0, static_cast<int>(m_verts.size() - 1));
				std::swap(m_ids[i], m_ids[targetI]);
				std::swap(m_verts[i], m_verts[targetI]);
				if (i != targetI)
				{
					updated = true;
				}
			}
			ImGui::PopStyleVar();
			ImGui::SetCursorPos(cursor);
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
			{
				const float speed{ static_cast<float>(_max - _min) / 50.0f};
				bool vertUpdated;
				if constexpr (TInt)
				{
					vertUpdated = ImGui::DragInt3("", xyz, speed, static_cast<int>(_min), static_cast<int>(_max), "%d", ImGuiSliderFlags_AlwaysClamp);
				}
				else
				{
					vertUpdated = ImGui::DragFloat3("", xyz, speed, static_cast<float>(_min), static_cast<float>(_max), "%f", ImGuiSliderFlags_AlwaysClamp);
				}
				if (vertUpdated)
				{
					vert.x() = static_cast<Value>(xyz[0]);
					vert.y() = static_cast<Value>(xyz[1]);
					vert.z() = static_cast<Value>(xyz[2]);
					updated = true;
				}
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("C"))
			{
				_copiedVert = vert;
				_copied = true;
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
	bool PolyControl<TInt>::draw(Value _min, Value _max, const std::optional<Vert>& _copiedVert)
	{
		bool copied{ false };
		std::optional<Vert> tempVert{ _copiedVert };
		const bool updated{ draw(_min, _max, tempVert, copied) };
		if (copied)
		{
			copyVert(tempVert.value());
		}
		return updated;
	}

	template<bool TInt>
	bool PolyControl<TInt>::draw(Value _min, Value _max)
	{
		return draw(_min, _max, pasteVert());
	}

}

