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

	AppSidebarItem::AppSidebarItem() : cinolib::SideBarItem{ "App" }, m_children{}, m_sourceControl{}, m_activeChild{}, onSourceUpdate{}, m_hasAnySolo{ false }, m_singleMode{ false }, m_file{}
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
		const std::string filename{ (_new || !m_file.has_value()) ? cinolib::file_dialog_save() : m_file.value_or("") };
		if (!filename.empty())
		{
			m_file = filename;
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

	void AppSidebarItem::clear()
	{
		if (m_activeChild.has_value())
		{
			onActiveVertChange();
			m_activeChild = std::nullopt;
		}
		for (ChildControl* child : m_children)
		{
			delete child;
		}
		m_children.clear();
		m_hasAnySolo = false;
		onChildrenClear();
	}

	void AppSidebarItem::load()
	{
		const std::string filename{ cinolib::file_dialog_open() };
		if (!filename.empty())
		{
			clear();
			m_file = filename;
			std::ifstream file{};
			file.open(filename);
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
		if (m_file.has_value())
		{
			std::string project{ m_file.value() };
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
		m_children.back()->setExpanded(false);
		onChildAdd();
		doUpdateColors();
	}

	void AppSidebarItem::removeChild(std::size_t _child)
	{
		if (_child >= m_children.size())
		{
			throw std::logic_error{ "index out of bounds" };
		}
		if (m_activeChild == _child)
		{
			m_activeChild = std::nullopt;
			onActiveVertChange();
		}
		delete m_children[_child];
		m_children.erase(m_children.begin() + _child);
		if (m_activeChild.has_value() && m_activeChild.value() > _child)
		{
			m_activeChild = m_activeChild.value() - 1;
		}
		onChildRemove(_child);
		doUpdateColors();
	}

	void AppSidebarItem::setActiveVert(std::size_t _vert)
	{
		if (!m_activeChild.has_value())
		{
			throw std::logic_error{ "no active child" };
		}
		m_children[m_activeChild.value()]->setActiveVert(_vert);
		onActiveVertChange();
	}

	void AppSidebarItem::setActiveVert(const IVec3& _vert)
	{
		if (!m_activeChild.has_value())
		{
			throw std::logic_error{ "no active child" };
		}
		m_children[m_activeChild.value()]->setActiveVert(_vert);
		m_sourceControl.setSize(std::max(m_sourceControl.size(), m_children[m_activeChild.value()]->maxSize()));
		onChildUpdate(m_activeChild.value());
		onActiveVertChange();
	}

	std::optional<std::size_t> AppSidebarItem::activeChildIndex() const
	{
		return m_activeChild;
	}

	const ChildControl& AppSidebarItem::activeChild() const
	{
		return *m_children[m_activeChild.value()];
	}

	std::optional<std::size_t> AppSidebarItem::activeVertIndex() const
	{
		return m_activeChild.has_value() ? std::optional{ activeChild().hexControl().activeVert() } : std::nullopt;
	}

	void AppSidebarItem::setActiveChild(std::optional<std::size_t> _child)
	{
		if (m_activeChild != _child)
		{
			const std::optional<std::size_t> old{ m_activeChild };
			m_activeChild = _child;
			if (old.has_value())
			{
				m_children[old.value()]->setExpanded(false);
				onChildUpdate(old.value());
			}
			if (_child.has_value())
			{
				if (_child.value() >= m_children.size())
				{
					throw std::logic_error{ "index out of bounds" };
				}
				m_activeChild = _child;
				m_children[_child.value()]->setExpanded(true);
				onChildUpdate(_child.value());
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
		}
		updateAllChildren();
	}

	bool AppSidebarItem::visible(const ChildControl& _child) const
	{
		return m_singleMode
			? m_activeChild.has_value() && m_children[m_activeChild.value()] == &_child
			: (_child.visible() && (!m_hasAnySolo || _child.solo()));
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
			for (ChildControl* child : m_children)
			{
				child->setVisible(true);
				child->setSolo(false);
			}
			updateAllChildren();
		}
	}


	void AppSidebarItem::cubeActive()
	{
		if (m_activeChild.has_value())
		{
			ChildControl& child{ *m_children[m_activeChild.value()] };
			const IVec3 oldActiveVert{ child.hexControl().verts()[child.hexControl().activeVert()] };
			child.setVerts(hexUtils::cubeVerts(m_sourceControl.clipMin(), m_sourceControl.clipMax()));
			onChildUpdate(m_activeChild.value());
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

	void AppSidebarItem::dense()
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
				for (Int z{ min.y() }; z < max.z(); z++)
				{
					a.z() = z;
					b.z() = z + 1;
					addChild(a, b);
				}
			}
		}
	}

	void AppSidebarItem::flip()
	{
		for (ChildControl* child : m_children)
		{
			HexVertsU verts{ child->hexControl().verts() };
			hexUtils::flipVerts(verts, editDim, m_sourceControl.size());
			child->setVerts(verts);
		}
		updateAllChildren();
	}

	void AppSidebarItem::mirror()
	{
		const std::size_t maxI{ m_children.size() };
		for (std::size_t i{}; i < maxI; i++)
		{
			HexVertsU verts{ m_children[i]->hexControl().verts() };
			hexUtils::flipVerts(verts, editDim, m_sourceControl.size());
			addChild(verts);
		}
	}

	void AppSidebarItem::rotate()
	{
		for (ChildControl* child : m_children)
		{
			HexVertsU verts{ child->hexControl().verts() };
			hexUtils::rotateVerts(verts, editDim, m_sourceControl.size());
			child->setVerts(verts);
		}
		updateAllChildren();
	}

	void AppSidebarItem::updateAllChildren()
	{
		for (std::size_t i{}; i < m_children.size(); i++)
		{
			onChildUpdate(i);
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
		if (ImGui::CollapsingHeader("Edit"))
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
			if (ImGui::SmallButton("Flip"))
			{
				flip();
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("Mirror"))
			{
				mirror();
			}
			ImGui::SameLine();
			if (ImGui::SmallButton("Rotate"))
			{
				rotate();
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
			const IVec3 oldActiveVertValue{ m_activeChild.has_value() ? activeChild().hexControl().verts()[activeVertIndex().value()] : IVec3{0,0,0} };
			for (std::size_t i{}; i < m_children.size(); i++)
			{
				ChildControl& child{ *m_children[i] };
				ImGui::PushID(&child);
				const bool wasVisible{ visible(child) };
				const bool wasActive{ child.expanded() };
				const ChildControl::EVisibilityMode mode{
					m_singleMode
					? ChildControl::EVisibilityMode::Hidden
					: m_hasAnySolo
						? ChildControl::EVisibilityMode::SomeSolo
						: ChildControl::EVisibilityMode::Default
				};
				const ChildControl::EResult result{ child.draw(m_sourceControl.clipMin(), m_sourceControl.clipMax(), copiedVerts, copiedVert, mode) };
				if (wasVisible != visible(child))
				{
					onChildUpdate(i);
				}
				if (wasActive != child.expanded())
				{
					if (wasActive)
					{
						setActiveChild(std::nullopt);
					}
					else
					{
						setActiveChild(i);
					}
				}
				switch (result)
				{
					case ChildControl::EResult::Updated:
						onChildUpdate(i);
						if (wasActive && child.expanded() && child.hexControl().verts()[activeVertIndex().value()] != oldActiveVertValue)
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
				dense();
			}
			ImGui::SameLine();
			bool singleMode{ m_singleMode };
			if (ImGui::Checkbox("Single mode", &singleMode))
			{
				setSingleMode(singleMode);
			}
			const bool hadAnySolo{ m_hasAnySolo };
			m_hasAnySolo = false;
			for (const ChildControl* child : m_children)
			{
				if (child->solo())
				{
					m_hasAnySolo = true;
					break;
				}
			}
			if (m_hasAnySolo != hadAnySolo)
			{
				updateAllChildren();
			}
		}
		// command bar
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		if (m_file.has_value())
		{
			ImGui::TextDisabled(m_file.value().c_str());
			ImGui::Spacing();
		}
		if (m_file.has_value())
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