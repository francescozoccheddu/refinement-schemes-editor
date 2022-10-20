#ifndef RSE_HEXUTILS_TPP
#error __FILE__ cannot be included directly
#endif

#include <RSE/hexUtils.hpp>

#include <vector>

namespace RSE::hexUtils
{

	template<typename TValue>
	HexVertData<Vec3<TValue>> cubeVerts(TValue _min, TValue _max)
	{
		if (_min > _max)
		{
			throw std::logic_error{ "min > max" };
		}
		return { Vec3<TValue>{ _min,_min,_min }, Vec3<TValue>{ _max,_min,_min }, Vec3<TValue>{ _min,_max,_min }, Vec3<TValue>{ _max,_max,_min }, Vec3<TValue>{ _min,_min,_max }, Vec3<TValue>{ _max,_min,_max }, Vec3<TValue>{ _min,_max,_max }, Vec3<TValue>{ _max,_max,_max } };
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

