#pragma once

#include <RSE/types.hpp>
#include <cinolib/color.h>
#include <array>

namespace RSE
{

	class Child final
	{

	private:

		bool m_valid;
		PolyVertsU m_verts;
		PolyVertData<char> m_vertIds;
		bool m_visible;
		bool m_solo;
		bool m_expanded;
		Int m_maxSize;

		void updateValid();
		void updateMaxSize();
		void update();

	public:

		enum class EResult
		{
			Updated, Removed, None 
		};

		Child(Int _size);

		const PolyVertsU& verts() const;

		bool visible() const;

		bool solo() const;

		bool expanded() const;

		bool valid() const;

		void setVerts(const PolyVertsU& _verts);

		void setExpanded(bool _expanded);

		void setSolo(bool _solo);
		
		void setVisible(bool _visible);

		Int maxSize() const;

		EResult draw(Int _size, bool _anySolo);

	};

}