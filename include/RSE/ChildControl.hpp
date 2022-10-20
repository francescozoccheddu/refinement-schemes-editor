#pragma once

#include <RSE/types.hpp>
#include <RSE/HexControl.hpp>
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
		IHexControl m_hexControl;
		Int m_maxSize;
		Style m_style;

		void update();

	public:

		enum class EResult
		{
			Updated, Removed, None 
		};

		enum class EVisibilityMode
		{
			SomeSolo, Default, Hidden
		};

		ChildControl(Int _size);

		Style& style();

		const Style& style() const;

		const IHexControl& hexControl() const;

		void setVerts(const HexVertsU& _verts);

		bool visible() const;

		bool solo() const;

		bool expanded() const;

		void setExpanded(bool _expanded);

		void setSolo(bool _solo);
		
		void setVisible(bool _visible);

		void setActiveVert(std::size_t _index);

		void setActiveVert(const IVec3& _vert);

		Int maxSize() const;

		EResult draw(Int _size, EVisibilityMode _visibilityMode = EVisibilityMode::Default);

		EResult draw(Int _size, const std::optional<HexVertsU>& _copiedVerts, const std::optional<IVec3>& _copiedVert, EVisibilityMode _visibilityMode = EVisibilityMode::Default);

	};

}