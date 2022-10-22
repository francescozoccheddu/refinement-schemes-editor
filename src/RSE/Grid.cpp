#include <RSE/Grid.hpp>

#include <utility>
#include <cmath>
#include <cinolib/geometry/lerp.hpp>

namespace RSE
{

	template<typename TOut, typename TIn, std::size_t TSize>
	typename std::conditional_t<std::is_same_v<TIn, TOut>, const std::array<Vec3<TOut>, TSize>&, std::array<Vec3<TOut>, TSize>> genCast(const std::array<Vec3<TIn>, TSize>& _vecs)
	{
		if constexpr (std::is_same_v<TIn, TOut>)
		{
			return _vecs;
		}
		else
		{
			std::array<Vec3<TOut>, TSize> out;
			for (std::size_t i{}; i < _vecs.size(); i++)
			{
				out[i] = _vecs[i].template cast<TOut>();
			}
			return out;
		}
	}

	template<typename TOut, typename TIn>
	typename std::conditional_t<std::is_same_v<TIn, TOut>, const std::vector<Vec3<TOut>>&, std::vector<Vec3<TOut>>> genCast(const std::vector<Vec3<TIn>>& _vecs)
	{
		if constexpr (std::is_same_v<TIn, TOut>)
		{
			return _vecs;
		}
		else
		{
			std::vector<Vec3<TOut>> out;
			out.resize(_vecs.size());
			for (std::size_t i{}; i < _vecs.size(); i++)
			{
				out[i] = _vecs[i].template cast<TOut>();
			}
			return out;
		}
	}

	typename Grid::CastResult<Grid::FastVert> Grid::cast(const RVec3& _vec)
	{
		return _vec.cast<FastValue>();
	}

	typename Grid::CastResult<Grid::FastVert> Grid::cast(const IVec3& _vec)
	{
		return _vec.cast<FastValue>();
	}

	typename Grid::CastResult<RVec3> Grid::cast(const FastVert& _vec)
	{
		return _vec.cast<Real>();
	}

	typename Grid::CastResult<Grid::FastHex> Grid::cast(const HexVerts& _verts)
	{
		return genCast<FastValue>(_verts);
	}

	typename Grid::FastValue Grid::pointLineOffset(const FastVert& _origin, const FastVert& _dir, const FastVert& _point)
	{
		const FastVert ab = _dir;
		const FastVert ap = _point - _origin;

		if (ap.dot(ab) <= FastValue{ 0 })
			return static_cast<FastValue>(- ap.norm());

		const FastVert bp = _point - (_origin + _dir);

		if (bp.dot(ab) >= FastValue{ 0 })
			return static_cast<FastValue>(bp.norm());

		return static_cast<FastValue>((ab.cross(ap)).norm() / ab.norm());
	}

	typename Grid::FastValue Grid::pointLineSqrDist(const FastVert& _origin, const FastVert& _normDir, const FastVert& _point, bool& _behind)
	{
		const FastValue offset{ pointLineOffset(_origin, _normDir, _point) };
		_behind = offset < 0;
		return _point.dist_sqrd(_origin + _normDir * offset);
	}

	Grid::Grid() : m_size{ 0 }, m_points{}
	{}

	Int Grid::size() const
	{
		return m_size;
	}

	void Grid::make(Int _size, const HexVerts& _verts)
	{
		if (_size < 1)
		{
			throw std::logic_error{ "size must be positive" };
		}
		m_size = _size;
		const FastHex sourceHex{ cast(_verts) };
		const Int layers{ _size + 1 };
		m_points.resize(static_cast<std::size_t>(layers * layers * layers));
		std::size_t i{};
		IVec3 coord;
		for (Int z{}; z < layers; z++)
		{
			coord.z() = z;
			for (Int y{}; y < layers; y++)
			{
				coord.y() = y;
				for (Int x{}; x < layers; x++)
				{
					coord.x() = x;
					m_points[i++] = cinolib::lerp3(sourceHex, cast(coord) / static_cast<FastValue>(_size));
				}
			}
		}
	}

	const std::vector<Grid::FastVert>& Grid::points() const
	{
		return m_points;
	}

	typename Grid::CastResult<std::vector<RVec3>> Grid::realPoints() const
	{
		return genCast<Real>(m_points);
	}

	std::size_t Grid::index(const IVec3& _coords) const
	{
		assert(_coords[0] >= 0 && _coords[0] <= m_size);
		assert(_coords[1] >= 0 && _coords[1] <= m_size);
		assert(_coords[2] >= 0 && _coords[2] <= m_size);
		const std::size_t layers{ static_cast<std::size_t>(m_size + 1) };
		return static_cast<std::size_t>(_coords.z() * layers * layers + _coords.y() * layers + _coords.x());
	}

	IVec3 Grid::coord(std::size_t _index) const
	{
		assert(_index <= m_points.size());
		Int index{ static_cast<Int>(_index) };
		IVec3 coord;
		const Int layers{ m_size + 1 };
		coord.x() = index % layers;
		index /= layers;
		coord.y() = index % layers;
		index /= layers;
		coord.z() = index;
		return coord;
	}

	HexVerts Grid::points(const HexVertsU& _coords) const
	{
		HexVerts verts;
		for (std::size_t i{}; i < verts.size(); i++)
		{
			verts[i] = point(_coords[i]);
		}
		return verts;
	}

	RVec3 Grid::point(std::size_t _index) const
	{
		assert(_index <= m_points.size());
		return cast(m_points[_index]);
	}

	RVec3 Grid::point(const IVec3& _coords) const
	{
		return point(index(_coords));
	}

	std::size_t Grid::closestToRay(const RVec3& _origin, const RVec3& _dir) const
	{
		return closestToRay(_origin, _dir, IVec3{ 0,0,0 }, IVec3{ m_size, m_size, m_size });
	}

	std::size_t Grid::closestToRay(const RVec3& _origin, const RVec3& _dir, const IVec3& _min, const IVec3& _max) const
	{
		FastValue minDist{ std::numeric_limits<FastValue>::infinity() };
		std::size_t minI{};
		const FastVert& forigin{ cast(_origin) }, & fdir{ cast(_dir) };
		IVec3 c;
		for (Int x{ _min.x() }; x <= _max.x(); x++)
		{
			c.x() = x;
			for (Int y{ _min.y() }; y <= _max.y(); y++)
			{
				c.y() = y;
				for (Int z{ _min.z() }; z <= _max.z(); z++)
				{
					c.z() = z;
					const std::size_t i{ index(c) };
					bool behind;
					const FastValue dist{ pointLineSqrDist(forigin, fdir, m_points[i], behind) };
					if (dist < minDist)
					{
						minDist = dist;
						minI = i;
					}
				}
			}
		}
		return minI;
	}

}