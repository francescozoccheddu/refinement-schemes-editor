#pragma once

#include <RSE/types.hpp>
#include <cstddef>

namespace RSE::hexUtils
{

	template<typename TValue>
	HexVertData<Vec3<TValue>> cubeVerts(TValue _min, TValue _max);

	template<typename TValue>
	HexVertData<Vec3<TValue>> cubeVerts(TValue _size = TValue{ 2 });

	template<typename TValue>
	HexVertData<Vec3<TValue>> sortVerts(const HexVertData<Vec3<TValue>>& _verts);

	constexpr HexVertData<std::size_t> cinolibVertsOrder{ 0,1,3,2, 4,5,7,6 };

}

#define RSE_HEXUTILS_TPP
#include <RSE/hexUtils.tpp>
#undef RSE_HEXUTILS_TPP