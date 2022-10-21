#pragma once

#include <RSE/types.hpp>
#include <RSE/HexControl.hpp>
#include <vector>

namespace RSE
{

	class SourceControl final
	{

	private:

		static constexpr Int c_maxSize{ 10 };
		static constexpr Real c_maxExtent{ 5 };

		RHexControl m_displacementControl;
		Int m_size;
		IVec3 m_clipMin, m_clipMax;

	public:

		enum class EResult
		{
			None, Updated, ClipUpdated
		};

		SourceControl();

		void setDisplacement(const HexVerts& _verts);

		const RHexControl& displ() const;

		void setSize(Int _size);

		Int size() const;

		void setClip(const IVec3& _min, const IVec3& _max);

		const IVec3& clipMin() const;

		const IVec3& clipMax() const;

		EResult draw(Int _minSize = 0);

	};

}