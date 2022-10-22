#pragma once

#include <cinolib/gl/side_bar_item.h>
#include <RSE/types.hpp>
#include <RSE/hexUtils.hpp>
#include <RSE/ChildControl.hpp>
#include <RSE/HexControl.hpp>
#include <RSE/SourceControl.hpp>
#include <vector>
#include <string>
#include <optional>
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
		bool m_hasAnySelected;
		bool m_singleMode;
		std::optional<std::string> m_file{};

		Int minRequiredSize() const;
		void doUpdateColors();
		void updateSelection();

		void addChild(const IVec3& _min, const IVec3& _max);
		void addChild(const HexVertsU& _verts);

	public:

		hexUtils::EDim editDim;

		AppSidebarItem();

		cpputils::collections::Event<AppSidebarItem> onSourceUpdate;
		cpputils::collections::Event<AppSidebarItem> onCursorUpdate;
		cpputils::collections::Event<AppSidebarItem> onChildrenClear;
		cpputils::collections::Event<AppSidebarItem> onChildAdd;
		cpputils::collections::Event<AppSidebarItem> onFileChange;
		cpputils::collections::Event<AppSidebarItem> onActiveVertChange;
		cpputils::collections::Event<AppSidebarItem, std::size_t> onChildRemove;
		cpputils::collections::Event<AppSidebarItem, std::size_t> onChildUpdate;

		void setHideCursor(bool _hidden);

		const std::optional<std::string>& file() const;

		void clear();

		void save(bool _new);

		void addChildrenCursorGrid();

		void flipShown();

		void rotateShown();

		void addChild();

		void cloneShown();

		void removeShown();

		void setChildSelected(std::size_t _child, bool _selected);

		void setAllSelected(bool _selected);

		void setActiveSelected(bool _selected);

		void translateShown(bool _advance);

		void translateCursor(bool _advance);

		void scaleCursor(bool _advance);

		void removeChild(std::size_t _child);

		void load(const std::string& _filename);

		void load();

		std::string exportCode() const;

		void exportCodeToFile() const;
		
		void exportCodeToClipboard() const;

		void setActiveVert(std::size_t _vert);

		void setActiveVert(const IVec3& _vert);

		bool singleMode() const;

		void setSingleMode(bool _enabled);

		void cubeActive();

		void setCursor(const IVec3& _min, const IVec3& _max);

		std::optional<std::size_t> activeChildIndex() const;

		const ChildControl& activeChild() const;

		std::optional<std::size_t> activeVertIndex() const;

		void setActiveChild(std::optional<std::size_t> _child);

		const SourceControl& source() const;

		bool shown(const ChildControl& _child) const;

		bool targeted(const ChildControl& _child) const;

		ChildControls children() const;

		void draw() override final;

	};

}