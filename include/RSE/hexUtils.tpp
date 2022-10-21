#ifndef RSE_HEXUTILS_TPP
#error __FILE__ cannot be included directly
#endif

#include <RSE/hexUtils.hpp>

#include <vector>

namespace RSE::hexUtils
{

	template<typename TValue>
	HexVertData<Vec3<TValue>> cubeVerts(const Vec3<TValue>& _min, const Vec3<TValue>& _max)
	{
		return { Vec3<TValue>{ _min.x(),_min.y(),_min.z() }, Vec3<TValue>{ _max.x(),_min.y(),_min.z() }, Vec3<TValue>{ _min.x(),_max.y(),_min.z() }, Vec3<TValue>{ _max.x(),_max.y(),_min.z() }, Vec3<TValue>{ _min.x(),_min.y(),_max.z() }, Vec3<TValue>{ _max.x(),_min.y(),_max.z() }, Vec3<TValue>{ _min.x(),_max.y(),_max.z() }, Vec3<TValue>{ _max.x(),_max.y(),_max.z() } };
	}

	template<typename TValue>
	HexVertData<Vec3<TValue>> cubeVerts(TValue _size)
	{
		return cubeVerts(-_size / 2, _size / 2);
	}

	template<typename TValue>
	HexVertData<Vec3<TValue>> sortVerts(const HexVertData<Vec3<TValue>>& _verts)
	{
		return _verts;
	}

}

