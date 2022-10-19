#pragma once

#include <cinolib/geometry/vec_mat.h>
#include <cstddef>
#include <array>

namespace RSE
{

	using Real = double;
	using Int = int;
	
	template<typename TData>
	using Vec3 = cinolib::vec<3, TData>;

	using RVec = Vec3<Real>;
	using IVec = Vec3<Int>;

	using Id = unsigned int;
	constexpr Id noId{ static_cast<Id>(-1) };

	template<typename TData>
	using PolyFaceData = std::array<TData, 6>;

	template<typename TData>
	using PolyEdgeData = std::array<TData, 12>;

	template<typename TData>
	using PolyVertData = std::array<TData, 8>;

	template<typename TData>
	using FaceVertData = std::array<TData, 4>;

	template<typename TData>
	using FaceEdgeData = std::array<TData, 4>;

	using PolyVerts = PolyVertData<RVec>;
	using PolyVertsU = PolyVertData<IVec>;
	using PolyVertIds = PolyVertData<Id>;
	using PolyFaceIds = PolyFaceData<Id>;
	using PolyEdgeIds = PolyEdgeData<Id>;
	using FaceVerts = FaceVertData<RVec>;
	using FaceVertsU = FaceVertData<IVec>;
	using FaceVertIds = FaceVertData<Id>;
	using FaceEdgeIds = FaceEdgeData<Id>;

}