#pragma once

#include <cinolib/gl/side_bar_item.h>
#include <RSE/types.hpp>
#include <RSE/ChildControl.hpp>
#include <RSE/PolyControl.hpp>
#include <vector>

namespace RSE
{

	class AppSidebarItem final : public cinolib::SideBarItem
	{

	private:

		static constexpr Int c_maxSize{ 10 };

		std::vector<ChildControl*> m_children;
		RPolyControl m_polyControl;
		Int m_size;
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

		void draw() override final;

	};

}