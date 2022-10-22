#include <RSE/SourceControl.hpp>

#include <stdexcept>
#include <imgui.h>

namespace RSE
{

	SourceControl::SourceControl()
		: m_size{ 3 }, m_displacementControl{ RHexControl::cubeVerts(RVec3{-c_maxExtent,-c_maxExtent, -c_maxExtent}, RVec3{c_maxExtent,c_maxExtent, c_maxExtent}) }, m_cursorMin{ 0,0,0 }, m_cursorMax{ m_size, m_size, m_size }
	{}

	void SourceControl::setDisplacement(const HexVerts& _verts)
	{
		m_displacementControl.setVerts(_verts);
	}

	const RHexControl& SourceControl::displ() const
	{
		return m_displacementControl;
	}

	void SourceControl::setSize(Int _size)
	{
		if (_size < 1 || _size > c_maxSize)
		{
			throw std::logic_error{ "size out of bounds" };
		}
		m_size = _size;
	}

	Int SourceControl::size() const
	{
		return m_size;
	}

	void SourceControl::setCursor(const IVec3& _min, const IVec3& _max)
	{
		if (_min.x() < 0 || _min.y() < 0 || _min.z() < 0 ||
			_min.x() > _max.x() || _min.y() > _max.y() || _min.z() > _max.z() ||
			_max.x() > m_size || _max.y() > m_size || _max.z() > m_size)
		{
			throw std::logic_error{ "cursor out of bounds" };
		}
		m_cursorMin = _min;
		m_cursorMax = _max;
	}

	const IVec3& SourceControl::cursorMin() const
	{
		return m_cursorMin;
	}

	const IVec3& SourceControl::cursorMax() const
	{
		return m_cursorMax;
	}

	bool& SourceControl::hideCursor()
	{
		return m_hideCursor;
	}

	bool SourceControl::hideCursor() const
	{
		return m_hideCursor;
	}

	SourceControl::EResult SourceControl::draw(Int _minSize)
	{
		if (_minSize < 0 || _minSize > m_size)
		{
			throw std::logic_error{ "min size out of bounds" };
		}
		const Int oldSize{ m_size };
		ImGui::Spacing();
		const float speed{ (m_size - _minSize) / 100.0f };
		if (ImGui::DragInt("Size", &m_size, 1.0f / 100.0f, _minSize, c_maxSize, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			m_cursorMin = IVec3{ 0,0,0 };
			m_cursorMax = IVec3{ m_size, m_size, m_size };
		}
		if (m_size * 2 <= c_maxSize)
		{
			ImGui::SameLine();
			if (ImGui::SmallButton("Double"))
			{
				m_size *= 2;
				m_cursorMin *= 2;
				m_cursorMax *= 2;
			}
		}
		bool displUpdated{ false };
		ImGui::Spacing();
		if (ImGui::TreeNode("Displacement"))
		{
			ImGui::Spacing();
			ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
			if (m_displacementControl.draw(false, -c_maxExtent, c_maxExtent))
			{
				displUpdated = true;
			}
			ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
			ImGui::TreePop();
		}
		const auto dragInt3{ [this, speed](IVec3& _vec, const char* _label) {
			Vec3<int> ivec { _vec.cast<int>() };
			const bool updated{ ImGui::DragInt3(_label, ivec.ptr(), speed, 0, static_cast<int>(m_size), "%d",ImGuiSliderFlags_AlwaysClamp)};
			_vec = ivec.cast<Int>();
			return updated;
		} };
		ImGui::Spacing();
		ImGui::TextDisabled("Cursor:");
		ImGui::Spacing();
		bool cursorUpdated{ false };
		const bool cursorMinUpdated{ cursorUpdated = dragInt3(m_cursorMin, "Min") };
		cursorUpdated |= dragInt3(m_cursorMax, "Max");
		const auto limit{ [cursorMinUpdated](Int& _min, Int& _max) {
			if (_min > _max)
			{
				if (cursorMinUpdated)
				{
					_min = _max;
				}
				else
				{
					_max = _min;
				}
			}
		} };
		limit(m_cursorMin.x(), m_cursorMax.x());
		limit(m_cursorMin.y(), m_cursorMax.y());
		limit(m_cursorMin.z(), m_cursorMax.z());
		ImGui::Spacing();
		if (ImGui::Button("Reset"))
		{
			m_cursorMin = IVec3{ 0,0,0 };
			m_cursorMax = IVec3{ m_size, m_size, m_size };
			cursorUpdated = true;
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Hidden", &m_hideCursor))
		{
			cursorUpdated = true;
		}
		if (displUpdated)
		{
			return EResult::Updated;
		}
		if (m_size != oldSize)
		{
			return (m_size == oldSize * 2) ? EResult::DoubledSize : EResult::Updated;
		}
		if (cursorUpdated)
		{
			return EResult::CursorUpdated;
		}
		return EResult::None;
	}


}