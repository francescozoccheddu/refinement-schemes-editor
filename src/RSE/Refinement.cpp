#include <RSE/Refinement.hpp>

#include <stdexcept>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <utility>
#include <array>
#include <bitset>
#include <unordered_map>
#include <functional>

namespace RSE
{

	Refinement::Influence::Influence(std::size_t _index, Real _weight) : m_index{ _index }, m_weight{ _weight }
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

	std::size_t Refinement::Influence::index() const
	{
		return m_index;
	}

	Real Refinement::Influence::weight() const
	{
		return m_weight;
	}

	Refinement::Vert Refinement::vert(const IVec3& _coord, Int _size)
	{
		std::array<bool, 3> exDims{};
		std::array<bool, 3> exVals{};
		std::size_t exDimsCount{};
		for (std::size_t d{}; d < 3; d++)
		{
			if (_coord[d] == 0 || _coord[d] == _size)
			{
				exDims[d] = true;
				exVals[d] = _coord[d] == _size;
				exDimsCount++;
			}
		}
		const std::size_t sourceCount{ static_cast<std::size_t>(1 << (3 - exDimsCount)) };
		std::vector<std::size_t> indices{};
		std::vector<double> weights{};
		weights.resize(sourceCount);
		indices.resize(sourceCount);
		double distSum{};
		const RVec3 rCoord{ static_cast<Real>(_coord[0]), static_cast<Real>(_coord[1]), static_cast<Real>(_coord[2]) };
		for (std::size_t i{}; i < sourceCount; i++)
		{
			RVec3 vert;
			std::size_t& ind{ indices[i] };
			Real& dist{ weights[i] };
			std::size_t fdi{};
			for (std::size_t d{}; d < 3; d++)
			{
				const bool set{ exDims[d] ? exVals[d] : ((i & (1 << fdi++)) != 0) };
				vert[d] = static_cast<Real>(set ? _size : 0);
				if (set)
				{
					ind |= 1 << d;
				}
				else
				{
					ind &= ~(1 << d);
				}
			}
			distSum += dist = rCoord.dist(vert);
		}
		if (sourceCount > 1)
		{
			for (double& weight : weights)
			{
				weight /= distSum;
			}
		}
		else
		{
			weights[0] = 1.0;
		}
		Vert out{};
		out.reserve(sourceCount);
		for (std::size_t i{}; i < sourceCount; i++)
		{
			out.push_back(Influence{ indices[i], weights[i] });
		}
		return out;
	}

	Refinement::Refinement(Verts&& _verts, Inds&& _inds) : m_verts{ std::move(_verts) }, m_inds{ std::move(_inds) }
	{}

	Refinement Refinement::build(const std::vector<HexVertsU>& _childs, Int _size)
	{
		const std::size_t vertsSizeHint{ static_cast<std::size_t>(std::min(_size * _size * _size * 0.25, _childs.size() * 8 * 0.25)) };
		const auto coordHash{ [_size](const IVec3& _vec) {
			static constexpr std::hash<Int> hash{};
			return hash(_vec.x() + _vec.y() * _size + _vec.z() * _size * _size);
		} };
		std::unordered_map<IVec3, std::size_t, decltype(coordHash)> vertMap{ 10, coordHash };
		Verts verts{};
		Inds inds{};
		vertMap.reserve(vertsSizeHint);
		verts.reserve(vertsSizeHint);
		inds.reserve(_childs.size());
		for (const HexVertsU& child : _childs)
		{
			HexInds childInds;
			for (std::size_t i{}; i < 8; i++)
			{
				const IVec3 coord{ child[i] };
				const std::unordered_map<IVec3, std::size_t>::iterator vertMapIt{ vertMap.find(coord) };
				if (vertMapIt != vertMap.end())
				{
					childInds[i] = vertMapIt->second;
				}
				else
				{
					vertMap.insert(vertMapIt, std::make_pair(coord, verts.size()));
					childInds[i] = verts.size();
					verts.push_back(vert(coord, _size));
				}
			}
			inds.push_back(childInds);
		}
		verts.shrink_to_fit();
		return Refinement{ std::move(verts), std::move(inds) };
	}

	const Refinement::Verts& Refinement::vertices() const
	{
		return m_verts;
	}

	const Refinement::Inds& Refinement::indices() const
	{
		return m_inds;
	}

	std::string Refinement::cppCode() const
	{
		std::ostringstream ss{};
		ss << "{\n";
		for (const HexInds& childInds : m_inds)
		{
			ss << "\t{\n";
			for (std::size_t vertInd : childInds)
			{
				ss << "\t\t{";
				for (const Influence& vert : m_verts[vertInd])
				{
					ss << "{" << vert.index() << ", " << vert.weight() << "},";
				}
				ss << "},\n";
			}
			ss << "\n\t},\n";
		}
		ss << "}";
		return ss.str();
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

