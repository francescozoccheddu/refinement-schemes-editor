#include <RSE/SourceControl.hpp>

#include <stdexcept>
#include <imgui.h>

namespace RSE
{

	SourceControl::SourceControl() 
		: m_size{ 3 }, m_displacementControl{ RHexControl::cubeVerts(RVec3{-c_maxExtent,-c_maxExtent, -c_maxExtent}, RVec3{c_maxExtent,c_maxExtent, c_maxExtent}) }, m_clipMin{ 0,0,0 }, m_clipMax{ m_size, m_size, m_size }
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

	void SourceControl::setClip(const IVec3& _min, const IVec3& _max)
	{
		if (_min.x() < 0 || _min.y() < 0 || _min.z() < 0 ||
			_min.x() > _max.x() || _min.x() > _max.y() || _min.z() > _max.z() ||
			_max.x() > m_size || _max.y() > m_size || _max.z() > m_size)
		{
			throw std::logic_error{ "clip out of bounds" };
		}
		m_clipMin = _min;
		m_clipMax = _max;
	}

	const IVec3& SourceControl::clipMin() const
	{
		return m_clipMin;
	}

	const IVec3& SourceControl::clipMax() const
	{
		return m_clipMax;
	}

	SourceControl::EResult SourceControl::draw(Int _minSize)
	{
		if (_minSize < 0 || _minSize > m_size)
		{
			throw std::logic_error{ "min size out of bounds" };
		}
		bool displUpdated{ false };
		ImGui::Spacing();
		const float speed{ (m_size - _minSize) / 100.0f };
		if (ImGui::DragInt("Size", &m_size, 1.0f / 100.0f, _minSize, c_maxSize, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			m_clipMin = IVec3{ 0,0,0 };
			m_clipMax = IVec3{ m_size, m_size, m_size };
			displUpdated = true;
		}
		ImGui::Spacing();
		if (m_displacementControl.draw(false, -c_maxExtent, c_maxExtent))
		{
			displUpdated = true;
		}
		const auto dragInt3{ [this, speed](IVec3& _vec, const char* _label) {
			int xyz[3] { static_cast<int>(_vec.x()), static_cast<int>(_vec.y()), static_cast<int>(_vec.z())};
			const bool updated{ ImGui::DragInt3(_label, xyz, speed, 0, static_cast<int>(m_size), "%d",ImGuiSliderFlags_AlwaysClamp)};
			_vec.x() = static_cast<Int>(xyz[0]);
			_vec.y() = static_cast<Int>(xyz[1]);
			_vec.z() = static_cast<Int>(xyz[2]);
			return updated;
		} };
		ImGui::Spacing();
		ImGui::TextDisabled("Clipping:");
		ImGui::Spacing();
		bool clipUpdated{ false };
		const bool clipMinUpdated{ clipUpdated = dragInt3(m_clipMin, "Min") };
		clipUpdated |= dragInt3(m_clipMax, "Max");
		const auto limit{ [clipMinUpdated](Int& _min, Int& _max) {
			if (_min > _max)
			{
				if (clipMinUpdated)
				{
					_min = _max;
				}
				else
				{
					_max = _min;
				}
			}
		} };
		limit(m_clipMin.x(), m_clipMax.x());
		limit(m_clipMin.y(), m_clipMax.y());
		limit(m_clipMin.z(), m_clipMax.z());
		ImGui::Spacing();
		if (ImGui::Button("Reset"))
		{
			m_clipMin = IVec3{ 0,0,0 };
			m_clipMax = IVec3{ m_size, m_size, m_size };
			clipUpdated = true;
		}
		if (displUpdated)
		{
			return EResult::Updated;
		}
		if (clipUpdated)
		{
			return EResult::ClipUpdated;
		}
		return EResult::None;
	}


}