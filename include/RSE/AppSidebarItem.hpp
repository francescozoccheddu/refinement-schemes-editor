#pragma once

#include <cinolib/gl/side_bar_item.h>
#include <RSE/types.hpp>
#include <RSE/ChildControl.hpp>
#include <RSE/HexControl.hpp>
#include <RSE/SourceControl.hpp>
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
		SourceControl m_sourceControl;
		std::optional<std::size_t> m_activeChild;
		bool m_hasAnySolo;
		bool m_singleMode;

		Int minRequiredSize() const;
		void doUpdateColors();

	public:

		AppSidebarItem();

		cpputils::collections::Event<AppSidebarItem> onSourceUpdate;
		cpputils::collections::Event<AppSidebarItem> onSourceClipUpdate;
		cpputils::collections::Event<AppSidebarItem> onChildrenClear;
		cpputils::collections::Event<AppSidebarItem> onChildAdd;
		cpputils::collections::Event<AppSidebarItem> onActiveVertChange;
		cpputils::collections::Event<AppSidebarItem, std::size_t> onChildRemove;
		cpputils::collections::Event<AppSidebarItem, std::size_t> onChildUpdate;

		void clear();

		void save() const;

		void addChild();

		void removeChild(std::size_t _child);

		void load();

		void exportCode() const;

		void setActiveVert(std::size_t _vert);

		void setActiveVert(const IVec3& _vert);

		bool singleMode() const;

		void setSingleMode(bool _enabled);

		void cubeActive();

		void setClip(const IVec3& _min, const IVec3& _max);

		std::optional<std::size_t> activeChildIndex() const;

		const ChildControl& activeChild() const;

		std::optional<std::size_t> activeVertIndex() const;

		void setActiveChild(std::optional<std::size_t> _child);

		const SourceControl& source() const;

		bool visible(const ChildControl& _child) const;

		ChildControls children() const;

		void draw() override final;

	};

}