#include <RSE/Grid.hpp>

namespace RSE
{

	typename Grid::FastVert Grid::lerp1(const FastEdge& _src, FastValue _alpha)
	{
		return _src[0] * (1.0f - _alpha) + _src[1] * _alpha;
	}

	typename Grid::FastVert Grid::lerp2(const FastQuad& _src, const FastVec2& _alpha)
	{
		const FastVert y1 = lerp1({ _src[0], _src[1] }, _alpha.x());
		const FastVert y2 = lerp1({ _src[2], _src[3] }, _alpha.x());
		return lerp1({ y1, y2 }, _alpha.y());
	}

	typename Grid::FastVert Grid::lerp3(const FastHex& _src, const FastVert& _alpha)
	{
		const FastVert z1 = lerp2(FastQuad{ _src[0], _src[1], _src[2], _src[3] }, FastVec2{ _alpha.x(), _alpha.y() });
		const FastVert z2 = lerp2(FastQuad{ _src[4], _src[5], _src[6], _src[7] }, FastVec2{ _alpha.x(), _alpha.y() });
		return lerp1({ z1, z2 }, _alpha.z());
	}

	template<typename TOut, typename TIn>
	typename std::conditional_t<std::is_same_v<TIn, TOut>, const Vec3<TOut>&, Vec3<TOut>> genCast(const Vec3<TIn>& _vec)
	{
		if constexpr (std::is_same_v<TIn, TOut>)
		{
			return _vec;
		}
		else
		{
			Vec3<TOut> out;
			for (std::size_t d{}; d < 3; d++)
			{
				out[d] = static_cast<TOut>(_vec[d]);
			}
			return out;
		}
	}

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
				out[i] = genCast<TOut>(_vecs[i]);
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
				out[i] = genCast<TOut>(_vecs[i]);
			}
			return out;
		}
	}

	typename Grid::CastResult<Grid::FastVert> Grid::cast(const RVec3& _vec)
	{
		return genCast<FastValue>(_vec);
	}

	typename Grid::CastResult<Grid::FastVert> Grid::cast(const IVec3& _vec)
	{
		return genCast<FastValue>(_vec);
	}

	typename Grid::CastResult<RVec3> Grid::cast(const FastVert& _vec)
	{
		return genCast<Real>(_vec);
	}

	typename Grid::CastResult<Grid::FastHex> Grid::cast(const HexVerts& _verts)
	{
		return genCast<FastValue>(_verts);
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
					m_points[i++] = lerp3(sourceHex, cast(coord) / static_cast<FastValue>(_size));
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

	HexVerts Grid::points(const HexVertsU& _coords) const
	{
		HexVerts verts;
		for (std::size_t i{}; i < verts.size(); i++)
		{
			verts[i] = point(_coords[i]);
		}
		return verts;
	}

	RVec3 Grid::point(const IVec3& _coords) const
	{
		return cast(m_points[index(_coords)]);
	}

	IVec3 Grid::closestToRay(const RVec3& _origin, const RVec3 _dir) const
	{
		return IVec3{ 0,0,0 };
	}

}