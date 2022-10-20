#pragma once

#include <RSE/types.hpp>
#include <cstddef>
#include <vector>
#include <string>

namespace RSE
{

	class Refinement final
	{

	public:

		class Influence final
		{

		private:

			std::size_t m_index;
			Real m_weight;

		public:

			Influence(std::size_t _index, Real _weight);

			std::size_t index() const;

			Real weight() const;

		};

		using Vert = std::vector<Influence>;
		using Verts = std::vector<Vert>;
		using HexInds = HexVertData<std::size_t>;
		using Inds = std::vector<HexInds>;

	private:

		Verts m_verts;
		Inds m_inds;

		Refinement(Verts&& _verts, Inds&& _inds);

		static Vert vert(const IVec3& _coord, Int _size);

	public:

		static Refinement build(const std::vector<HexVertsU>& _childs, Int _size);

		const Verts& vertices() const;

		const Inds& indices() const;

		std::string cppCode() const;

	};

}

