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

namespace RSE
{

	AppSidebarItem::AppSidebarItem() : cinolib::SideBarItem{ "App" }, m_children{}, m_sourceSize{ 3 }, m_sourceControl{ RPolyControl::cubeVerts(-c_maxSourceExtent, c_maxSourceExtent) }, m_activeChild{}, m_expandedChild{}, m_expandSingle{ false }, onSourceUpdate{}
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

	bool AppSidebarItem::hasAnySolo() const
	{
		for (const ChildControl* child : m_children)
		{
			if (child->solo())
			{
				return true;
			}
		}
		return false;
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
				for (const IVec3& vert : child->polyControl().verts())
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
			}
			file.close();
			updateColors();
		}
	}

	void AppSidebarItem::doExport() const
	{}

	const RPolyControl& AppSidebarItem::sourceControl() const
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

	void AppSidebarItem::updateColors()
	{
		for (std::size_t i{}; i < m_children.size(); i++)
		{
			m_children[i]->style() = Style{ (i / static_cast<float>(m_children.size())) * 360.0f };
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
			const bool anySolo{ hasAnySolo() };
			std::optional<HexVertsU> copiedVerts{ IPolyControl::pasteVerts() };
			std::optional<IVec3> copiedVert{ IPolyControl::pasteVert() };
			for (std::size_t i{}; i < m_children.size(); i++)
			{
				ChildControl& child{ *m_children[i] };
				ImGui::PushID(&child);
				const ChildControl::EResult result{ child.draw(m_sourceSize, anySolo, copiedVerts, copiedVert) };
				if (&child != m_expandedChild)
				{
					if (child.expanded())
					{
						if (m_expandedChild && m_expandSingle)
						{
							m_expandedChild->setExpanded(false);
						}
						m_expandedChild = &child;
					}
				}
				else if (!child.expanded())
				{
					m_expandedChild = nullptr;
				}
				if (child.polyControl().activeVert().has_value())
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
						m_sourceSize = std::max(m_sourceSize, child.maxSize());
						break;
					case ChildControl::EResult::Removed:
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
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Single mode", &m_expandSingle) && m_expandSingle)
			{
				for (ChildControl* child : m_children)
				{
					child->setExpanded(false);
				}
				m_expandedChild = nullptr;
			}
			if (shouldUpdateColors)
			{
				updateColors();
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