#pragma once

#include <RSE/types.hpp>
#include <RSE/PolyControl.hpp>
#include <RSE/Style.hpp>
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
		Style m_style;

		void update();

	public:

		enum class EResult
		{
			Updated, Removed, None 
		};

		ChildControl(Int _size);

		Style& style();

		const Style& style() const;

		const IPolyControl& polyControl() const;

		void setVerts(const HexVertsU& _verts);

		bool visible() const;

		bool solo() const;

		bool expanded() const;

		void setExpanded(bool _expanded);

		void setSolo(bool _solo);
		
		void setVisible(bool _visible);

		void setActiveVert(std::optional<std::size_t> _index);

		Int maxSize() const;

		EResult draw(Int _size, bool _anySolo, const std::optional<HexVertsU>& _copiedVerts, const std::optional<IVec3>& _copiedVert);

	};

}