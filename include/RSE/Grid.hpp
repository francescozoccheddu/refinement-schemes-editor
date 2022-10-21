#pragma once 

#include <RSE/types.hpp>
#include <vector>
#include <type_traits>

namespace RSE
{

	class Grid final
	{

	private:

		static constexpr bool c_useSingleFp{ true };

		using FastValue = std::conditional_t<c_useSingleFp, float, Real>;
		using FastVec2 = Vec2<FastValue>;
		using FastVert = Vec3<FastValue>;
		using FastHex = HexVertData<FastVert>;
		using FastQuad = QuadVertData<FastVert>;
		using FastEdge = EdgeVertData<FastVert>;

		static constexpr bool c_isReal{ std::is_same_v<Real, FastValue> };

		template<typename TData>
		using CastResult = std::conditional_t<c_isReal, const TData&, TData>;

		std::vector<FastVert> m_points;
		Int m_size;

		static FastVert lerp1(const FastEdge& _src, FastValue _alpha);

		static FastVert lerp2(const FastQuad& _src, const FastVec2& _alpha);

		static FastVert lerp3(const FastHex& _src, const FastVert& _alpha);

		static CastResult<RVec3> cast(const FastVert& _vec);

		static CastResult<FastVert> cast(const RVec3& _vec);

		static CastResult<FastVert> cast(const IVec3& _vec);

		static CastResult<FastHex> cast(const HexVerts& _verts);

		static FastValue pointLineOffset(const FastVert& _origin, const FastVert& _dir, const FastVert& _point);

		static FastValue pointLineSqrDist(const FastVert& _origin, const FastVert& _normDir, const FastVert& _point, bool& _behind);

	public:

		Grid();

		Int size() const;

		void make(Int _size, const HexVerts& _verts);

		const std::vector<FastVert>& points() const;

		CastResult<std::vector<RVec3>> realPoints() const;

		std::size_t index(const IVec3& _coords) const;

		IVec3 coord(std::size_t _index) const;

		HexVerts points(const HexVertsU& _coords) const;

		RVec3 point(std::size_t _index) const;

		RVec3 point(const IVec3& _coords) const;

		std::size_t closestToRay(const RVec3& _origin, const RVec3& _dir, const IVec3& _min, const IVec3& _max) const;

		std::size_t closestToRay(const RVec3& _origin, const RVec3& _dir) const;

	};

}