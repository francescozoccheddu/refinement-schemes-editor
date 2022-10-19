#include <RSE/AppSidebarItem.hpp>

#include <cinolib/color.h>
#include <cinolib/gl/file_dialog_save.h>
#include <cinolib/gl/file_dialog_open.h>
#include <stdexcept>
#include <imgui.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <RSE/Style.hpp>

namespace RSE
{

	AppSidebarItem::AppSidebarItem() : cinolib::SideBarItem{ "App" }, m_children{}, m_size{ 3 }, m_polyControl{ RPolyControl::cubeVerts(1) }, m_activeChild{}, m_expandedChild{}, m_expandSingle{ false }
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
			constexpr char sep{ '\n' };
			file << m_size << sep;
			file << m_children.size() << sep;
			for (const ChildControl* child : m_children)
			{
				for (const IVec& vert : child->polyControl().verts())
				{
					file << vert << sep;
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
			file >> m_size;
			std::size_t childrenSize{};
			file >> childrenSize;
			m_children.reserve(childrenSize);
			while (childrenSize > 0)
			{
				ChildControl& child{ *new ChildControl{1} };
				m_children.push_back(&child);
				PolyVertsU verts;
				for (IVec& vert : verts)
				{
					file >> vert;
				}
				child.setVerts(verts);
				childrenSize--;
			}
			file.close();
		}
	}

	void AppSidebarItem::doExport() const
	{}

	void AppSidebarItem::updateColors()
	{}

	void AppSidebarItem::draw()
	{
		ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
		ImGui::Spacing();
		// source
		if (ImGui::CollapsingHeader("Source"))
		{
			ImGui::Spacing();
			ImGui::DragInt("Size", &m_size, 1.0f / 20.0f, minRequiredSize(), c_maxSize, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Spacing();
			if (m_polyControl.draw(-5.0, 5.0))
			{

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
			const bool anySolo{ hasAnySolo() };
			std::optional<PolyVertsU> copiedVerts{ IPolyControl::pasteVerts() };
			std::optional<IVec> copiedVert{ IPolyControl::pasteVert() };
			for (std::size_t i{}; i < m_children.size(); i++)
			{
				ChildControl& child{ *m_children[i] };
				ImGui::PushID(&child);
				const Style style{ (i / static_cast<float>(m_children.size())) * 360.0f };
				style.pushImGui();
				const ChildControl::EResult result{ child.draw(m_size, anySolo, copiedVerts, copiedVert) };
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
						m_size = std::max(m_size, child.maxSize());
						break;
					case ChildControl::EResult::Removed:
						m_children.erase(m_children.begin() + i);
						if (&child == m_activeChild)
						{
							m_activeChild = nullptr;
						}
						delete& child;
						--i;
						break;
				}
				ImGui::Spacing();
				Style::popImGui();
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
				m_children.push_back(new ChildControl{ m_size });
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