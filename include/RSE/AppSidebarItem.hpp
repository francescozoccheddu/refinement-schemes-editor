#pragma once

#include <cinolib/gl/side_bar_item.h>
#include <RSE/types.hpp>
#include <RSE/ChildControl.hpp>
#include <RSE/PolyControl.hpp>
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
		RPolyControl m_sourceControl;
		Int m_sourceSize;
		ChildControl* m_activeChild, * m_expandedChild;
		bool m_expandSingle;

		Int minRequiredSize() const;
		bool hasAnySolo() const;
		void drawControls();
		void doClear();
		void doSave() const;
		void doLoad();
		void doExport() const;
		void updateColors();

	public:

		AppSidebarItem();

		cpputils::collections::Event<AppSidebarItem> onSourceUpdate;

		const RPolyControl& sourceControl() const;

		Int sourceSize() const;

		ChildControls childControls() const;

		void draw() override final;

	};

}