#include <RSE/AppSidebarItem.hpp>

#include <cinolib/color.h>
#include <cinolib/gl/file_dialog_save.h>
#include <cinolib/gl/file_dialog_open.h>
#include <cpputils/serialization/Serializer.hpp>
#include <cpputils/serialization/Deserializer.hpp>
#include <stdexcept>
#include <imgui.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <RSE/Style.hpp>
#include <RSE/Refinement.hpp>
#include <iterator>

namespace RSE
{

	AppSidebarItem::AppSidebarItem() : cinolib::SideBarItem{ "App" }, m_children{}, m_sourceSize{ 3 }, m_sourceControl{ RHexControl::cubeVerts(-c_maxSourceExtent, c_maxSourceExtent) }, m_activeChild{}, onSourceUpdate{}, m_hasAnySolo{ false }, m_singleMode{ false }
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

	void AppSidebarItem::save() const
	{
		const std::string filename{ cinolib::file_dialog_save() };
		if (!filename.empty())
		{
			std::ofstream file{};
			file.open(filename);
			cpputils::serialization::Serializer s{ file };
			s << m_sourceSize << m_children.size();
			for (const ChildControl* child : m_children)
			{
				for (const IVec3& vert : child->hexControl().verts())
				{
					s << vert.x() << vert.y() << vert.z();
				}
			}
			file.close();
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
			std::ifstream file{};
			file.open(filename);
			cpputils::serialization::Deserializer s{ file };
			s >> m_sourceSize;
			std::size_t childrenSize{};
			s >> childrenSize;
			m_children.reserve(childrenSize);
			while (childrenSize > 0)
			{
				ChildControl& child{ *new ChildControl{1} };
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

	void AppSidebarItem::exportCode() const
	{
		const std::string filename{ cinolib::file_dialog_save() };
		if (!filename.empty())
		{
			std::ofstream file{};
			file.open(filename);
			std::vector<HexVertsU> verts{};
			verts.reserve(m_children.size());
			for (const ChildControl* child : m_children)
			{
				verts.push_back(child->hexControl().verts());
			}
			file << Refinement::build(verts, m_sourceSize).cppCode();
			file.close();
		}
	}


	void AppSidebarItem::addChild()
	{
		m_children.push_back(new ChildControl{ m_sourceSize });
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
		onChildRemove(_child);
		m_children.erase(m_children.begin() + _child);
		if (m_activeChild.has_value() && m_activeChild.value() > _child)
		{
			m_activeChild = m_activeChild.value() - 1;
		}
		delete& m_children[_child];
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
		m_sourceSize = std::max(m_sourceSize, m_children[m_activeChild.value()]->maxSize());
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

	const RHexControl& AppSidebarItem::sourceControl() const
	{
		return m_sourceControl;
	}

	Int AppSidebarItem::sourceSize() const
	{
		return m_sourceSize;
	}

	AppSidebarItem::ChildControls AppSidebarItem::childControls() const
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

	bool AppSidebarItem::visible(const ChildControl& _child) const
	{
		return m_singleMode
			? m_activeChild.has_value() && m_children[m_activeChild.value()] == &_child
			: (_child.visible() && (!m_hasAnySolo || _child.solo()));
	}

	void AppSidebarItem::draw()
	{
		ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::Spacing();
		// source
		ImGui::SetNextItemOpen(false, ImGuiCond_Once);
		if (ImGui::CollapsingHeader("Source"))
		{
			bool sourceUpdated{ false };
			ImGui::Spacing();
			if (ImGui::DragInt("Size", &m_sourceSize, 1.0f / 100.0f, minRequiredSize(), c_maxSize, "%d", ImGuiSliderFlags_AlwaysClamp))
			{
				sourceUpdated = true;
			}
			ImGui::Spacing();
			if (m_sourceControl.draw(false, -c_maxSourceExtent, c_maxSourceExtent))
			{
				sourceUpdated = true;
			}
			if (sourceUpdated)
			{
				onSourceUpdate();
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
				const ChildControl::EResult result{ child.draw(m_sourceSize, copiedVerts, copiedVert, mode) };
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
						m_sourceSize = std::max(m_sourceSize, child.maxSize());
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
			if (ImGui::Checkbox("Single mode", &m_singleMode))
			{
				for (std::size_t i{}; i < m_children.size(); i++)
				{
					m_children[i]->setVisible(true);
					m_children[i]->setSolo(false);
					onChildUpdate(i);
				}
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
				for (std::size_t i{}; i < m_children.size(); i++)
				{
					onChildUpdate(i);
				}
			}
		}
		// command bar
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		if (ImGui::Button("Save"))
		{
			save();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			load();
		}
		ImGui::SameLine();
		if (ImGui::Button("Export"))
		{
			exportCode();
		}
		ImGui::Spacing();
	}

}