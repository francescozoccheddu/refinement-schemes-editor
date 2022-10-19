#pragma once

#include <cinolib/geometry/vec_mat.h>
#include <cstddef>
#include <array>

namespace RSE
{

	using Real = double;
	using Int = int;
	
	template<typename TData>
	using Vec2 = cinolib::vec<2, TData>;

	template<typename TData>
	using Vec3 = cinolib::vec<3, TData>;

	using RVec2 = Vec2<Real>;
	using IVec2 = Vec2<Int>;

	using RVec3 = Vec3<Real>;
	using IVec3 = Vec3<Int>;

	template<typename TData>
	using HexVertData = std::array<TData, 8>;

	template<typename TData>
	using FaceVertData = std::array<TData, 4>;

	template<typename TData>
	using EdgeVertData = std::array<TData, 2>;

	using HexVerts = HexVertData<RVec3>;
	using HexVertsU = HexVertData<IVec3>;
	using FaceVerts = FaceVertData<RVec3>;
	using FaceVertsU = FaceVertData<IVec3>;
	using EdgeVerts = FaceVertData<RVec3>;
	using EdgeVertsU = FaceVertData<IVec3>;

}