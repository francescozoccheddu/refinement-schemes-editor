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

		class Child final
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

			using Verts = HexVertData<std::vector<Influence>>;

		private:

			Verts m_verts;

		public:

			static std::vector<Influence> influences(const IVec3& _coord, Int _size);

			static Child fromVertsU(const HexVertsU& _verts, Int _size);

			Child(const Verts& _verts);

			Child(Verts&& _verts);

			const Verts& verts() const;

		};

	private:

		std::vector<Child> m_children;

	public:

		Refinement(const std::vector<Child>& _children);

		Refinement(std::vector<Child>&& _children);

		const std::vector<Child>& children() const;
		
		std::string cppCode() const;

	};

}

