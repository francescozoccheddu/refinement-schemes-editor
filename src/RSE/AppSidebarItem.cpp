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

	AppSidebarItem::AppSidebarItem() : cinolib::SideBarItem{ "App" }, m_children{}, m_sourceSize{ 3 }, m_sourceControl{ RHexControl::cubeVerts(-c_maxSourceExtent, c_maxSourceExtent) }, m_activeChild{}, m_expandedChild{}, m_expandSingle{ false }, onSourceUpdate{}, m_hasAnySolo{ false }
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

	void AppSidebarItem::doSave() const
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

	void AppSidebarItem::doClear()
	{
		for (ChildControl* child : m_children)
		{
			delete child;
		}
		m_children.clear();
		m_expandedChild = m_activeChild = nullptr;
		m_hasAnySolo = false;
		onChildrenClear();
	}

	void AppSidebarItem::doLoad()
	{
		const std::string filename{ cinolib::file_dialog_open() };
		if (!filename.empty())
		{
			doClear();
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

	void AppSidebarItem::doExport() const
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
		return _child.visible() && (!m_hasAnySolo || _child.solo());
	}

	void AppSidebarItem::setActiveVert(std::size_t _child, std::optional<std::size_t> _vert)
	{
		if (_vert.has_value())
		{
			if (m_activeChild != m_children[_child])
			{
				if (m_activeChild)
				{
					m_activeChild->setActiveVert(std::nullopt);
				}
				m_activeChild = m_children[_child];
				m_children[_child]->setActiveVert(_vert);
			}
		}
	}

	void AppSidebarItem::draw()
	{
		ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::Spacing();
		// source
		if (ImGui::CollapsingHeader("Source"))
		{
			bool sourceUpdated{ false };
			ImGui::Spacing();
			if (ImGui::DragInt("Size", &m_sourceSize, 1.0f / 100.0f, minRequiredSize(), c_maxSize, "%d", ImGuiSliderFlags_AlwaysClamp))
			{
				sourceUpdated = true;
			}
			ImGui::Spacing();
			if (m_sourceControl.draw(-c_maxSourceExtent, c_maxSourceExtent))
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
			bool shouldUpdateColors{ false };
			std::optional<HexVertsU> copiedVerts{ IHexControl::pasteVerts() };
			std::optional<IVec3> copiedVert{ IHexControl::pasteVert() };
			for (std::size_t i{}; i < m_children.size(); i++)
			{
				ChildControl& child{ *m_children[i] };
				ImGui::PushID(&child);
				bool shouldUpdateChild{ false };
				const bool wasVisible{ visible(child) };
				const bool wasExpanded{ child.expanded() };
				const ChildControl::EResult result{ child.draw(m_sourceSize, m_hasAnySolo, copiedVerts, copiedVert) };
				shouldUpdateChild |= wasVisible != visible(child);
				shouldUpdateChild |= wasExpanded != child.expanded();
				if (&child != m_expandedChild)
				{
					if (child.expanded())
					{
						if (m_expandedChild && m_expandSingle)
						{
							m_expandedChild->setExpanded(false);
							if (!shouldUpdateColors)
							{
								onChildUpdate(std::distance(m_children.begin(), std::find(m_children.begin(), m_children.end(), m_expandedChild)));
							}
						}
						m_expandedChild = &child;
					}
				}
				else if (!child.expanded())
				{
					m_expandedChild = nullptr;
				}
				if (child.hexControl().activeVert().has_value())
				{
					if (m_activeChild != &child)
					{
						if (m_activeChild)
						{
							m_activeChild->setActiveVert(std::nullopt);
						}
						m_activeChild = &child;
					}
				}
				else if (m_activeChild == &child)
				{
					m_activeChild = nullptr;
				}
				switch (result)
				{
					case ChildControl::EResult::Updated:
						shouldUpdateChild = true;
						m_sourceSize = std::max(m_sourceSize, child.maxSize());
						break;
					case ChildControl::EResult::Removed:
						shouldUpdateChild = false;
						onChildRemove(i);
						m_children.erase(m_children.begin() + i);
						if (&child == m_activeChild)
						{
							m_activeChild = nullptr;
						}
						shouldUpdateColors = true;
						delete& child;
						--i;
						break;
				}
				if (shouldUpdateChild && !shouldUpdateColors)
				{
					onChildUpdate(i);
				}
				ImGui::Spacing();
				ImGui::PopID();
			}
			// action bar
			if (ImGui::Button("Clear"))
			{
				doClear();
			}
			ImGui::SameLine();
			if (ImGui::Button("Add"))
			{
				m_children.push_back(new ChildControl{ m_sourceSize });
				shouldUpdateColors = true;
				onChildAdd();
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Single mode", &m_expandSingle) && m_expandSingle)
			{
				for (std::size_t i{}; i < m_children.size(); i++)
				{
					const bool wasExpanded{ m_children[i]->expanded() };
					m_children[i]->setExpanded(false);
					if (!shouldUpdateColors && wasExpanded)
					{
						onChildUpdate(i);
					}
				}
				m_expandedChild = nullptr;
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
			if (shouldUpdateColors)
			{
				doUpdateColors();
			}
			else if (m_hasAnySolo != hadAnySolo)
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
			doSave();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			doLoad();
		}
		ImGui::SameLine();
		if (ImGui::Button("Export"))
		{
			doExport();
		}
		ImGui::Spacing();
	}

}