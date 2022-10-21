#pragma once

#include <RSE/types.hpp>
#include <cstddef>

namespace RSE::hexUtils
{

	enum class EDim
	{
		X=0, Y=1, Z=2
	};

	template<typename TValue>
	HexVertData<Vec3<TValue>> cubeVerts(const Vec3<TValue>& _min, const Vec3<TValue>& _max);

	template<typename TValue>
	HexVertData<Vec3<TValue>> sortVerts(const HexVertData<Vec3<TValue>>& _verts);

	constexpr HexVertData<std::size_t> cinolibHexInds{ 0,1,3,2, 4,5,7,6 };

	template<typename TValue>
	HexVertData<Vec3<TValue>> indexVerts(const HexVertData<Vec3<TValue>>& _verts, const HexVertData<std::size_t>& _indices);

	template<typename TValue>
	void flipVerts(HexVertData<Vec3<TValue>>& _verts, EDim _dim, TValue _doubleMid);

	template<typename TValue>
	void rotateVerts(HexVertData<Vec3<TValue>>& _verts, EDim _dim, TValue _doubleMid);

	template<typename TValue>
	void scaleVerts(HexVertData<Vec3<TValue>>& _verts, const Vec3<TValue>& _scale);

	template<typename TValue>
	void invScaleVerts(HexVertData<Vec3<TValue>>& _verts, const Vec3<TValue>& _scale);

	template<typename TValue>
	void translateVerts(HexVertData<Vec3<TValue>>& _verts, const Vec3<TValue>& _offset);

}

#define RSE_HEXUTILS_TPP
#include <RSE/hexUtils.tpp>
#undef RSE_HEXUTILS_TPP