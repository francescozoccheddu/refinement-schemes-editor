#pragma once

#include <RSE/types.hpp>
#include <RSE/PolyControl.hpp>
#include <optional>

namespace RSE
{

	class ChildControl final
	{

	private:

		bool m_visible;
		bool m_solo;
		bool m_expanded;
		IPolyControl m_polyControl;
		Int m_maxSize;

		void update();

	public:

		enum class EResult
		{
			Updated, Removed, None 
		};

		ChildControl(Int _size);

		const IPolyControl& polyControl() const;

		void setVerts(const PolyVertsU& _verts);

		bool visible() const;

		bool solo() const;

		bool expanded() const;

		void setExpanded(bool _expanded);

		void setSolo(bool _solo);
		
		void setVisible(bool _visible);

		void setActiveVert(std::optional<std::size_t> _index);

		Int maxSize() const;

		EResult draw(Int _size, bool _anySolo, const std::optional<PolyVertsU>& _copiedVerts, const std::optional<IVec>& _copiedVert);

	};

}