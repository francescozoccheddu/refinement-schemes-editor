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
		bool m_selected;
		bool m_active;
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
			SomeSelected, Default, Hidden
		};

		ChildControl(const HexVertsU& _verts);

		ChildControl(const IVec3& _min, const IVec3& _max);

		Style& style();

		const Style& style() const;

		const IHexControl& hexControl() const;

		void setVerts(const HexVertsU& _verts);

		bool visible() const;

		bool selected() const;

		bool active() const;

		void setActive(bool _active);

		void setSelected(bool _selected);
		
		void setVisible(bool _visible);

		void setActiveVert(std::size_t _index);

		void setActiveVert(const IVec3& _vert);

		Int maxSize() const;

		EResult draw(const IVec3& _min, const IVec3& _max, const std::optional<HexVertsU>& _copiedVerts, const std::optional<IVec3>& _copiedVert, EVisibilityMode _visibilityMode = EVisibilityMode::Default);

	};

}