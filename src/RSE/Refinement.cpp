#include <RSE/Refinement.hpp>

#include <stdexcept>
#include <sstream>
#include <cmath>
#include <utility>
#include <array>

namespace RSE
{

	Refinement::Child::Influence::Influence(std::size_t _index, Real _weight) : m_index{ _index }, m_weight{ _weight }
	{
		if (_index < 0 || _index >= 8)
		{
			throw std::logic_error{ "index out of bounds" };
		}
		if (_weight <= 0 || !std::isfinite(_weight) || std::isnan(_weight))
		{
			throw std::logic_error{ "weight must be positive" };
		}
	}

	std::size_t Refinement::Child::Influence::index() const
	{
		return m_index;
	}

	Real Refinement::Child::Influence::weight() const
	{
		return m_weight;
	}

	std::vector<Refinement::Child::Influence> Refinement::Child::influences(const IVec3& _coord, Int _size)
	{
		std::array<bool, 3> extremaValue;
		std::array<bool, 3> extrema;
		std::size_t extremaCount{};
		for (std::size_t d{}; d < 3; d++)
		{
			const Int c{ _coord[d] };
			if (c < 0 || c > _size)
			{
				throw std::logic_error{ "coord out of bounds" };
			}
			extrema[d] = c == 0 || c == _size;
			extremaValue[d] = c == _size;
			if (extrema[d])
			{
				extremaCount++;
			}
		}
		switch (extremaCount)
		{
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
		}
	}

	Refinement::Child Refinement::Child::fromVertsU(const HexVertsU& _verts, Int _size)
	{
		Verts verts;
		for (std::size_t i{}; i < _verts.size(); i++)
		{
			verts[i] = influences(_verts[i], _size);
		}
		return Child{ std::move(verts) };
	}

	Refinement::Child::Child(const Refinement::Child::Verts& _verts) : m_verts{ _verts }
	{}

	Refinement::Child::Child(Refinement::Child::Verts&& _verts) : m_verts{ std::move(_verts) }
	{}

	const Refinement::Child::Verts& Refinement::Child::verts() const
	{
		return m_verts;
	}

	Refinement::Refinement(const std::vector<Refinement::Child>& _children) : m_children{ _children }
	{}

	Refinement::Refinement(std::vector<Refinement::Child>&& _children) : m_children{ std::move(_children) }
	{}

	const std::vector<Refinement::Child>& Refinement::children() const
	{
		return m_children;
	}

	std::string Refinement::cppCode() const 
	{
		return "";
	}

}

/*
const Refinement adapterEdgeSubdivide3x3{ {
		{{
			{{1, 0.333333}, {3, -0}, {0, 0.666667}, {7, -0}, },
			{{1, 0.333333}, {3, 0.666667}, {0, -0}, {7, -0}, },
			{{1, -0}, {3, 1}, {0, -0}, {7, -0}, },
			{{1, -0}, {3, -0}, {0, 1}, {7, -0}, },
			{{1, 0.333333}, {0, -0}, {4, 0.666667}, {7, -0}, },
			{{1, 0.333333}, {3, -0}, {0, -0}, {7, 0.666667}, },
			{{1, -0}, {3, -0}, {0, -0}, {7, 1}, },
			{{1, -0}, {0, -0}, {4, 1}, {7, -0}, },
		}},
		{{
			{{1, 0.666667}, {3, -0}, {0, 0.333333}, {7, -0}, },
			{{2, 0.333333}, {7, -0}, {3, 0.333333}, {1, 0.333333}, },
			{{1, 0.333333}, {3, 0.666667}, {0, -0}, {7, -0}, },
			{{1, 0.333333}, {3, -0}, {0, 0.666667}, {7, -0}, },
			{{1, 0.333333}, {4, 0.333333}, {5, 0.333333}, {7, -0}, },
			{{1, 0.333333}, {6, 0.333333}, {7, 0.333333}, {5, -0}, },
			{{1, 0.333333}, {3, -0}, {0, -0}, {7, 0.666667}, },
			{{1, 0.333333}, {0, -0}, {4, 0.666667}, {7, -0}, },
		}},
		{{
			{{1, 0.333333}, {4, 0.333333}, {5, 0.333333}, {7, -0}, },
			{{1, 0.333333}, {6, 0.333333}, {7, 0.333333}, {5, -0}, },
			{{1, 0.333333}, {3, -0}, {0, -0}, {7, 0.666667}, },
			{{1, 0.333333}, {0, -0}, {4, 0.666667}, {7, -0}, },
			{{1, -0}, {4, -0}, {5, 1}, {7, -0}, },
			{{1, -0}, {6, 1}, {7, -0}, {5, -0}, },
			{{1, -0}, {3, -0}, {0, -0}, {7, 1}, },
			{{1, -0}, {0, -0}, {4, 1}, {7, -0}, },
		}},
		{{
			{{1, 1}, {3, -0}, {0, -0}, {7, -0}, },
			{{2, 1}, {6, -0}, {7, -0}, {1, -0}, },
			{{2, 0.333333}, {7, -0}, {3, 0.333333}, {1, 0.333333}, },
			{{1, 0.666667}, {3, -0}, {0, 0.333333}, {7, -0}, },
			{{1, -0}, {4, -0}, {5, 1}, {7, -0}, },
			{{1, -0}, {6, 1}, {7, -0}, {5, -0}, },
			{{1, 0.333333}, {6, 0.333333}, {7, 0.333333}, {5, -0}, },
			{{1, 0.333333}, {4, 0.333333}, {5, 0.333333}, {7, -0}, },
		}},
		{{
			{{2, 0.333333}, {7, -0}, {3, 0.333333}, {1, 0.333333}, },
			{{2, 1}, {6, -0}, {7, -0}, {1, -0}, },
			{{1, -0}, {3, 1}, {0, -0}, {7, -0}, },
			{{1, 0.333333}, {3, 0.666667}, {0, -0}, {7, -0}, },
			{{1, 0.333333}, {6, 0.333333}, {7, 0.333333}, {5, -0}, },
			{{1, -0}, {6, 1}, {7, -0}, {5, -0}, },
			{{1, -0}, {3, -0}, {0, -0}, {7, 1}, },
			{{1, 0.333333}, {3, -0}, {0, -0}, {7, 0.666667}, },
		}},
	} };
*/

