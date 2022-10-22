#ifndef RSE_HEXUTILS_TPP
#error __FILE__ cannot be included directly
#endif

#include <RSE/hexUtils.hpp>

#include <vector>
#include <utility>

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

	template<typename TValue>
	HexVertData<Vec3<TValue>> indexVerts(const HexVertData<Vec3<TValue>>& _verts, const HexVertData<std::size_t>& _indices)
	{
		HexVertData<Vec3<TValue>> out;
		for (std::size_t i{}; i < 8; i++)
		{
			out[i] = _verts[_indices[i]];
		}
		return out;
	}

	template<typename TValue>
	void flipVerts(HexVertData<Vec3<TValue>>& _verts, EDim _dim, TValue _doubleMid)
	{
		const unsigned int dim{ static_cast<unsigned int>(_dim) };
		for (Vec3<TValue>& vert : _verts)
		{
			vert[dim] = _doubleMid - vert[dim];
		}
		static constexpr std::array<HexVertData<std::size_t>, 3> inds{
			HexVertData<std::size_t>{1,0,3,2,5,4,7,6},
			HexVertData<std::size_t>{2,3,0,1,6,7,4,5},
			HexVertData<std::size_t>{4,5,6,7,0,1,2,3}
		};
		_verts = indexVerts(_verts, inds[static_cast<std::size_t>(_dim)]);
	}

	template<typename TValue>
	void rotateVerts(HexVertData<Vec3<TValue>>& _verts, EDim _dim, TValue _doubleMid)
	{
		for (Vec3<TValue>& vert : _verts)
		{
			switch (_dim)
			{
				case RSE::hexUtils::EDim::X:
					std::swap(vert.y(), vert.z());
					vert.y() = _doubleMid - vert.y();
					break;
				case RSE::hexUtils::EDim::Y:
					std::swap(vert.x(), vert.z());
					vert.z() = _doubleMid - vert.z();
					break;
				case RSE::hexUtils::EDim::Z:
					std::swap(vert.x(), vert.y());
					vert.x() = _doubleMid - vert.x();
					break;
			}
		}
		static constexpr std::array<HexVertData<std::size_t>, 3> inds{
			HexVertData<std::size_t>{4,5,0,1,6,7,2,3},
			HexVertData<std::size_t>{1,5,3,7,0,4,2,6},
			HexVertData<std::size_t>{2,0,3,1,6,4,7,5}
		};
		_verts = indexVerts(_verts, inds[static_cast<std::size_t>(_dim)]);
	}

	template<typename TValue>
	void scaleVerts(HexVertData<Vec3<TValue>>& _verts, const Vec3<TValue>& _scale)
	{
		for (Vec3<TValue>& vert : _verts)
		{
			vert.x() *= _scale.x();
			vert.y() *= _scale.y();
			vert.z() *= _scale.z();
		}
	}

	template<typename TValue>
	void invScaleVerts(HexVertData<Vec3<TValue>>& _verts, const Vec3<TValue>& _scale)
	{
		for (Vec3<TValue>& vert : _verts)
		{
			vert.x() /= _scale.x();
			vert.y() /= _scale.y();
			vert.z() /= _scale.z();
		}
	}

	template<typename TValue>
	void translateVerts(HexVertData<Vec3<TValue>>& _verts, const Vec3<TValue>& _offset)
	{
		for (Vec3<TValue>& vert : _verts)
		{
			vert += _offset;
		}
	}

}

