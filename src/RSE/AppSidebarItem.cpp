#include <RSE/AppSidebarItem.hpp>

#include <cinolib/color.h>
#include <cinolib/gl/file_dialog_save.h>
#include <cinolib/gl/file_dialog_open.h>
#include <cinolib/gl/gl_glfw.h>
#include <cpputils/serialization/Serializer.hpp>
#include <cpputils/serialization/Deserializer.hpp>
#include <stdexcept>
#include <imgui.h>
#include <algorithm>
#include <fstream>
#include <cctype>
#include <string>
#include <RSE/Style.hpp>
#include <RSE/CppExporter.hpp>
#include <iterator>

namespace RSE
{

	AppSidebarItem::AppSidebarItem() : cinolib::SideBarItem{ "App" }, m_children{}, m_sourceControl{}, m_activeChild{}, onSourceUpdate{}, m_hasAnySelected{ false }, m_singleMode{ false }, m_file{}, editDim{ hexUtils::EDim::X }
	{
	}

	Int AppSidebarItem::minRequiredSize() const
	{
		Int minSize{ 1 };
		for (const ChildControl* child : m_children)
		{
			if (child->maxSize() > minSize)
			{
				minSize = child->maxSize();
			}
		}
		return minSize;
	}

	void AppSidebarItem::save(bool _new)
	{
		const std::string filename{ (_new || !m_file) ? cinolib::file_dialog_save() : m_file.value_or("") };
		if (!filename.empty())
		{
			m_file = filename;
			onFileChange();
			std::ofstream file{};
			file.open(filename);
			cpputils::serialization::Serializer s{ file };
			s << m_sourceControl.size() << m_children.size();
			for (const ChildControl* child : m_children)
			{
				for (const IVec3& vert : child->hexControl().verts())
				{
					s << vert.x() << vert.y() << vert.z();
				}
			}
			file.close();
			std::cout << "Saved to '" << filename << "'" << std::endl;
		}
	}

	const std::optional<std::string>& AppSidebarItem::file() const
	{
		return m_file;
	}

	void AppSidebarItem::clear()
	{
		if (m_activeChild)
		{
			m_activeChild = std::nullopt;
			onActiveVertChange();
		}
		for (ChildControl* child : m_children)
		{
			delete child;
		}
		m_children.clear();
		m_hasAnySelected = false;
		onChildrenClear();
	}

	void AppSidebarItem::load(const std::string& _filename)
	{
		clear();
		m_file = _filename;
		onFileChange();
		std::ifstream file{};
		file.open(_filename);
		cpputils::serialization::Deserializer s{ file };
		Int size;
		s >> size;
		m_sourceControl.setSize(size);
		std::size_t childrenSize{};
		s >> childrenSize;
		m_children.reserve(childrenSize);
		while (childrenSize > 0)
		{
			ChildControl& child{ *new ChildControl{m_sourceControl.clipMin(), m_sourceControl.clipMax()} };
			m_children.push_back(&child);
			HexVertsU verts;
			for (IVec3& vert : verts)
			{
				s >> vert.x() >> vert.y() >> vert.z();
			}
			child.setVerts(verts);
			childrenSize--;
			onChildAdd();
		}
		file.close();
		doUpdateColors();
	}

	void AppSidebarItem::load()
	{
		const std::string filename{ cinolib::file_dialog_open() };
		if (!filename.empty())
		{
			load(filename);
		}
	}

	std::string AppSidebarItem::exportCode() const
	{
		std::vector<HexVerts> children{};
		children.reserve(m_children.size());
		for (const ChildControl* child : m_children)
		{
			const HexVertsU& iVerts{ child->hexControl().verts() };
			HexVerts rVerts;
			for (std::size_t i{}; i < 8; i++)
			{
				rVerts[i] = iVerts[i].cast<Real>() / static_cast<Real>(m_sourceControl.size());
			}
			children.push_back(rVerts);
		}
		CppExporter exporter{};
		exporter.name = "";
		if (m_file)
		{
			std::string project{ *m_file };
			project = project.substr(project.find_last_of("/\\") + 1);
			project = project.substr(0, project.find_last_of("."));
			std::cout << project;
			if (!project.empty() && std::isdigit(project[0]))
			{
				project[0] = '_';
			}
			for (char& c : project)
			{
				if (!(std::isalnum(c) || c == '_'))
				{
					c = '_';
				}
			}
			exporter.name = project;
		}
		if (exporter.name.empty())
		{
			exporter.name = m_file.value_or("unnamed");
		}
		return exporter(children);
	}

	void AppSidebarItem::exportCodeToClipboard() const
	{
		glfwSetClipboardString(nullptr, exportCode().c_str());
		std::cout << "Exported " << m_children.size() << " children to the clipboard" << std::endl;
	}

	void AppSidebarItem::exportCodeToFile() const
	{
		const std::string filename{ cinolib::file_dialog_save() };
		if (!filename.empty())
		{
			std::ofstream file{};
			file.open(filename);
			file << exportCode();
			file.close();
			std::cout << "Exported " << m_children.size() << " children to '" << filename << "'" << std::endl;
		}
	}

	void AppSidebarItem::addChild()
	{
		addChild(m_sourceControl.clipMin(), m_sourceControl.clipMax());
	}

	void AppSidebarItem::addChild(const IVec3& _min, const IVec3& _max)
	{
		addChild(IHexControl::cubeVerts(_min, _max));
	}

	void AppSidebarItem::addChild(const HexVertsU& _verts)
	{
		m_children.push_back(new ChildControl{ _verts });
		m_children.back()->setActive(false);
		onChildAdd();
		doUpdateColors();
	}

	void AppSidebarItem::removeChild(std::size_t _child)
	{
		if (_child >= m_children.size())
		{
			throw std::logic_error{ "index out of bounds" };
		}
		setChildSelected(_child, false);
		if (m_activeChild == _child)
		{
			m_activeChild = std::nullopt;
			onActiveVertChange();
		}
		delete m_children[_child];
		m_children.erase(m_children.begin() + _child);
		if (m_activeChild > _child)
		{
			m_activeChild = *m_activeChild - 1;
		}
		onChildRemove(_child);
		doUpdateColors();
	}

	void AppSidebarItem::setActiveVert(std::size_t _vert)
	{
		if (m_activeChild)
		{
			m_children[*m_activeChild]->setActiveVert(_vert);
			onActiveVertChange();
		}
	}

	void AppSidebarItem::setActiveVert(const IVec3& _vert)
	{
		if (m_activeChild)
		{
			m_children[*m_activeChild]->setActiveVert(_vert);
			m_sourceControl.setSize(std::max(m_sourceControl.size(), m_children[*m_activeChild]->maxSize()));
			onChildUpdate(*m_activeChild);
			onActiveVertChange();
		}
	}

	std::optional<std::size_t> AppSidebarItem::activeChildIndex() const
	{
		return m_activeChild;
	}

	const ChildControl& AppSidebarItem::activeChild() const
	{
		return *m_children[*m_activeChild];
	}

	std::optional<std::size_t> AppSidebarItem::activeVertIndex() const
	{
		return m_activeChild ? std::optional{ activeChild().hexControl().activeVert() } : std::nullopt;
	}

	void AppSidebarItem::setActiveChild(std::optional<std::size_t> _child)
	{
		if (m_activeChild != _child)
		{
			const std::optional<std::size_t> old{ m_activeChild };
			m_activeChild = _child;
			if (old)
			{
				m_children[*old]->setActive(false);
				onChildUpdate(*old);
			}
			if (_child)
			{
				if (*_child >= m_children.size())
				{
					throw std::logic_error{ "index out of bounds" };
				}
				m_activeChild = _child;
				m_children[*_child]->setActive(true);
				onChildUpdate(*_child);
			}
			onActiveVertChange();
		}
	}

	const SourceControl& AppSidebarItem::source() const
	{
		return m_sourceControl;
	}

	AppSidebarItem::ChildControls AppSidebarItem::children() const
	{
		return ChildControls{ m_children };
	}

	void AppSidebarItem::doUpdateColors()
	{
		for (std::size_t i{}; i < m_children.size(); i++)
		{
			m_children[i]->style() = Style{ (i / static_cast<float>(m_children.size())) * 360.0f };
			onChildUpdate(i);
		}
	}

	bool AppSidebarItem::shown(const ChildControl& _child) const
	{
		return m_singleMode ? _child.active() : _child.active() || (_child.visible() && (!m_hasAnySelected || _child.selected()));
	}

	bool AppSidebarItem::singleMode() const
	{
		return m_singleMode;
	}

	void AppSidebarItem::setSingleMode(bool _enabled)
	{
		if (_enabled != m_singleMode)
		{
			m_singleMode = _enabled;
			m_hasAnySelected = false;
			for (std::size_t i{}; i < m_children.size(); i++)
			{
				m_children[i]->setVisible(true);
				m_children[i]->setSelected(false);
				onChildUpdate(i);
			}
		}
	}

	void AppSidebarItem::cubeActive()
	{
		if (m_activeChild)
		{
			ChildControl& child{ *m_children[*m_activeChild] };
			const IVec3 oldActiveVert{ child.hexControl().verts()[child.hexControl().activeVert()] };
			child.setVerts(hexUtils::cubeVerts(m_sourceControl.clipMin(), m_sourceControl.clipMax()));
			onChildUpdate(*m_activeChild);
			if (child.hexControl().verts()[child.hexControl().activeVert()] != oldActiveVert)
			{
				onActiveVertChange();
			}
		}
	}

	void AppSidebarItem::setClip(const IVec3& _min, const IVec3& _max)
	{
		const IVec3 oldMin{ m_sourceControl.clipMin() }, oldMax{ m_sourceControl.clipMax() };
		m_sourceControl.setClip(_min, _max);
		if (oldMin != _min || oldMax != _max)
		{
			onSourceClipUpdate();
		}
	}

	void AppSidebarItem::addChildrenClipGrid()
	{
		const IVec3 min{ m_sourceControl.clipMin() };
		const IVec3 max{ m_sourceControl.clipMax() };
		IVec3 a, b;
		for (Int x{ min.x() }; x < max.x(); x++)
		{
			a.x() = x;
			b.x() = x + 1;
			for (Int y{ min.y() }; y < max.y(); y++)
			{
				a.y() = y;
				b.y() = y + 1;
				for (Int z{ min.z() }; z < max.z(); z++)
				{
					a.z() = z;
					b.z() = z + 1;
					addChild(a, b);
				}
			}
		}
	}

	void AppSidebarItem::flipShown()
	{
		for (std::size_t i{}; i < m_children.size(); i++)
		{
			ChildControl& child{ *m_children[i] };
			if (shown(child))
			{
				HexVertsU verts{ child.hexControl().verts() };
				hexUtils::flipVerts(verts, editDim, m_sourceControl.size());
				child.setVerts(verts);
				onChildUpdate(i);
			}
		}
	}

	void AppSidebarItem::cloneShown()
	{
		const std::size_t maxI{ m_children.size() };
		for (std::size_t i{}; i < maxI; i++)
		{
			ChildControl& child{ *m_children[i] };
			if (shown(child))
			{
				addChild(child.hexControl().verts());
			}
		}
	}

	void AppSidebarItem::translateClip(bool _advance)
	{
		const unsigned int dim{ static_cast<unsigned int>(editDim) };
		const Int size{ source().size() };
		IVec3 min{ source().clipMin() };
		IVec3 max{ source().clipMax() };
		Int& mind{ min[dim] }, & maxd{ max[dim] };
		if ((_advance && maxd < size) || (!_advance && mind > 0))
		{
			if (_advance)
			{
				mind++;
				maxd++;
			}
			else
			{
				mind--;
				maxd--;
			}
		}
		setClip(min, max);
	}

	void AppSidebarItem::scaleClip(bool _advance)
	{
		const unsigned int dim{ static_cast<unsigned int>(editDim) };
		const Int size{ source().size() };
		IVec3 min{ source().clipMin() };
		IVec3 max{ source().clipMax() };
		Int& mind{ min[dim] }, & maxd{ max[dim] };
		if (mind > size - maxd)
		{
			if (_advance)
			{
				mind = std::max(0, mind - 1);
			}
			else
			{
				maxd = std::max(mind, maxd - 1);
			}
		}
		else
		{
			if (_advance)
			{
				maxd = std::min(size, maxd + 1);
			}
			else
			{
				mind = std::min(maxd, mind + 1);
			}
		}
		setClip(min, max);
	}

	void AppSidebarItem::translateShown(bool _advance)
	{
		const unsigned int dim{ static_cast<unsigned int>(editDim) };
		for (std::size_t i{}; i < m_children.size(); i++)
		{
			ChildControl& child{ *m_children[i] };
			if (shown(child))
			{
				for (const IVec3& vert : child.hexControl().verts())
				{
					if ((_advance && vert[dim] >= source().size()) || (!_advance && vert[dim] <= 0))
					{
						return;
					}
				}
			}
		}
		IVec3 offset{0,0,0};
		offset[dim] = _advance ? 1 : static_cast<Int>(-1);
		for (std::size_t i{}; i < m_children.size(); i++)
		{
			ChildControl& child{ *m_children[i] };
			if (shown(child))
			{
				HexVertsU verts{ child.hexControl().verts() };
				hexUtils::translateVerts(verts, offset);
				child.setVerts(verts);
				onChildUpdate(i);
			}
		}
	}

	void AppSidebarItem::removeShown()
	{
		for (std::size_t iPlusOne{ m_children.size() }; iPlusOne > 0; iPlusOne--)
		{
			if (shown(*m_children[iPlusOne - 1]))
			{
				removeChild(iPlusOne - 1);
			}
		}
	}

	void AppSidebarItem::rotateShown()
	{
		for (std::size_t i{}; i < m_children.size(); i++)
		{
			ChildControl& child{ *m_children[i] };
			if (shown(child))
			{
				HexVertsU verts{ child.hexControl().verts() };
				hexUtils::rotateVerts(verts, editDim, m_sourceControl.size());
				child.setVerts(verts);
				onChildUpdate(i);
			}
		}
	}

	void AppSidebarItem::setChildSelected(std::size_t _child, bool _selected)
	{
		if (!m_singleMode)
		{
			ChildControl& child{ *m_children[_child] };
			if (child.selected() != _selected)
			{
				const bool wasShown{ shown(child) };
				m_children[_child]->setSelected(_selected);
				updateSelection();
				if (wasShown != shown(child))
				{
					onChildUpdate(_child);
				}
			}
		}
	}

	void AppSidebarItem::setAllSelected(bool _selected)
	{
		for (std::size_t i{}; i < m_children.size(); i++)
		{
			setChildSelected(i, _selected);
		}
	}

	void AppSidebarItem::setActiveSelected(bool _selected)
	{
		if (m_activeChild)
		{
			setChildSelected(*m_activeChild, _selected);
		}
	}

	void AppSidebarItem::updateSelection()
	{
		const bool hadAnySelected{ m_hasAnySelected };
		m_hasAnySelected = false;
		for (const ChildControl* child : m_children)
		{
			if (child->selected())
			{
				m_hasAnySelected = true;
				break;
			}
		}
		if (m_hasAnySelected != hadAnySelected)
		{
			for (std::size_t i{}; i < m_children.size(); i++)
			{
				onChildUpdate(i);
			}
		}
	}

	void AppSidebarItem::draw()
	{
		ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::Spacing();
		// source
		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::CollapsingHeader("Source"))
		{
			ImGui::Spacing();
			switch (m_sourceControl.draw(minRequiredSize()))
			{
				case SourceControl::EResult::DoubledSize:
					for (ChildControl* child : m_children)
					{
						HexVertsU verts{ child->hexControl().verts() };
						hexUtils::scaleVerts(verts, IVec3{ 2,2,2 });
						child->setVerts(verts);
					}
				case SourceControl::EResult::Updated:
					onSourceUpdate();
				case SourceControl::EResult::ClipUpdated:
					onSourceClipUpdate();
					break;
			}
		}
		// edit
		ImGui::Spacing();
		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::CollapsingHeader("Edit shown"))
		{
			ImGui::Spacing();
			int dim{ static_cast<int>(editDim) };
			ImGui::RadioButton("x", &dim, 0);
			ImGui::SameLine();
			ImGui::RadioButton("y", &dim, 1);
			ImGui::SameLine();
			ImGui::RadioButton("z", &dim, 2);
			editDim = static_cast<hexUtils::EDim>(dim);
			ImGui::Spacing();
			if (ImGui::SmallButton("Clone"))
			{
				cloneShown();
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("Remove"))
			{
				removeShown();
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("Flip"))
			{
				flipShown();
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("Rotate"))
			{
				rotateShown();
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("Trans-"))
			{
				translateShown(false);
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("Trans+"))
			{
				translateShown(true);
			}
			if (!m_singleMode)
			{
				ImGui::Spacing();
				if (ImGui::SmallButton("Select all"))
				{
					setAllSelected(true);
				}
				ImGui::SameLine();
				if (ImGui::SmallButton("Deselect all"))
				{
					setAllSelected(false);
				}
			}
		}
		// children
		ImGui::Spacing();
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::CollapsingHeader("Children"))
		{
			// text
			ImGui::Spacing();
			if (m_children.empty())
			{
				ImGui::TextDisabled("No children");
			}
			else
			{
				if (m_children.size() == 1)
				{
					ImGui::TextDisabled("1 child:");
				}
				else
				{
					ImGui::TextDisabled("%d children:", m_children.size());
				}
			}
			ImGui::Spacing();
			// children
			std::optional<HexVertsU> copiedVerts{ IHexControl::pasteVerts() };
			std::optional<IVec3> copiedVert{ IHexControl::pasteVert() };
			const std::optional<std::size_t> oldActiveVert{ activeVertIndex() }, oldActiveChild{ activeChildIndex() };
			const IVec3 oldActiveVertValue{ m_activeChild ? activeChild().hexControl().verts()[*activeVertIndex()] : IVec3{0,0,0} };
			for (std::size_t i{}; i < m_children.size(); i++)
			{
				ChildControl& child{ *m_children[i] };
				ImGui::PushID(&child);
				const bool wasShown{ shown(child) };
				const bool wasActive{ child.active() };
				const bool wasSelected{ child.selected() };
				const ChildControl::EVisibilityMode mode{
					m_singleMode
					? ChildControl::EVisibilityMode::Hidden
					: m_hasAnySelected
						? ChildControl::EVisibilityMode::SomeSelected
						: ChildControl::EVisibilityMode::Default
				};
				const ChildControl::EResult result{ child.draw(m_sourceControl.clipMin(), m_sourceControl.clipMax(), copiedVerts, copiedVert, mode) };
				if (wasSelected != child.selected())
				{
					updateSelection();
				}
				if (wasActive != child.active())
				{
					setActiveChild(wasActive ? std::nullopt : std::optional{ i });
				}
				if (wasShown != shown(child))
				{
					onChildUpdate(i);
				}
				switch (result)
				{
					case ChildControl::EResult::Updated:
						onChildUpdate(i);
						if (wasActive && child.active() && child.hexControl().verts()[*activeVertIndex()] != oldActiveVertValue)
						{
							onActiveVertChange();
						}
						m_sourceControl.setSize(std::max(m_sourceControl.size(), child.maxSize()));
						break;
					case ChildControl::EResult::Removed:
						removeChild(i);
						--i;
						break;
				}
				ImGui::Spacing();
				ImGui::PopID();
			}
			if (activeVertIndex() != oldActiveVert || activeChildIndex() != oldActiveChild)
			{
				onActiveVertChange();
			}
			// action bar
			if (ImGui::Button("Clear"))
			{
				clear();
			}
			ImGui::SameLine();
			if (ImGui::Button("Add"))
			{
				addChild();
			}
			ImGui::SameLine();
			if (ImGui::Button("Dense"))
			{
				addChildrenClipGrid();
			}
			ImGui::SameLine();
			bool singleMode{ m_singleMode };
			if (ImGui::Checkbox("Single mode", &singleMode))
			{
				setSingleMode(singleMode);
			}
		}
		// command bar
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		if (m_file)
		{
			ImGui::TextDisabled(m_file->c_str());
			ImGui::Spacing();
		}
		if (m_file)
		{
			if (ImGui::Button("Save"))
			{
				save(false);
			}
			ImGui::SameLine();
		}
		if (ImGui::Button("Save as"))
		{
			save(true);
		}
		ImGui::SameLine();
		if (ImGui::Button("Open"))
		{
			load();
		}
		ImGui::SameLine();
		if (ImGui::Button("Export"))
		{
			exportCodeToFile();
		}
		ImGui::SameLine();
		if (ImGui::Button("Export to clipboard"))
		{
			exportCodeToClipboard();
		}
		ImGui::Spacing();
	}

}