#pragma once

#include <cinolib/gl/side_bar_item.h>
#include <RSE/types.hpp>
#include <RSE/ChildControl.hpp>
#include <RSE/HexControl.hpp>
#include <vector>
#include <cpputils/collections/Event.hpp>
#include <cpputils/collections/DereferenceIterable.hpp>

namespace RSE
{

	class AppSidebarItem final : public cinolib::SideBarItem
	{

	public:

		using ChildControls = cpputils::collections::DereferenceIterable<const std::vector<ChildControl*>, const ChildControl&>;

	private:

		static constexpr Int c_maxSize{ 10 };
		static constexpr Real c_maxSourceExtent{ 5 };

		std::vector<ChildControl*> m_children;
		RHexControl m_sourceControl;
		Int m_sourceSize;
		ChildControl* m_activeChild, * m_expandedChild;
		bool m_expandSingle;
		bool m_hasAnySolo;

		Int minRequiredSize() const;
		void doClear();
		void doSave() const;
		void doLoad();
		void doExport() const;
		void doUpdateColors();

	public:

		AppSidebarItem();

		cpputils::collections::Event<AppSidebarItem> onSourceUpdate;
		cpputils::collections::Event<AppSidebarItem> onChildrenClear;
		cpputils::collections::Event<AppSidebarItem> onChildAdd;
		cpputils::collections::Event<AppSidebarItem, std::size_t> onChildRemove;
		cpputils::collections::Event<AppSidebarItem, std::size_t> onChildUpdate;

		void setActiveVert(std::size_t _child, std::optional<std::size_t> _vert);

		const RHexControl& sourceControl() const;

		Int sourceSize() const;

		bool visible(const ChildControl& _child) const;

		ChildControls childControls() const;

		void draw() override final;

	};

}