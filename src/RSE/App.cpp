#include <RSE/App.hpp>

#include <cinolib/color.h>
#include <cinolib/gl/file_dialog_save.h>
#include <cinolib/gl/file_dialog_open.h>
#include <stdexcept>
#include <imgui.h>
#include <algorithm>
#include <fstream>
#include <string>

namespace RSE
{

	App::App() : m_canvas{}, m_axesWidget{ m_canvas.camera }, m_children{}, m_size{ 3 }
	{
		m_canvas.background = cinolib::Color::hsv2rgb(0.0f, 0.0f, 0.1f);
		m_canvas.callback_app_controls = [this]() { drawControls(); };
		m_canvas.show_sidebar(true);
		m_canvas.push(&m_axesWidget);
	}

	Int App::minRequiredSize() const
	{
		Int minSize{ 1 };
		for (const Child* child : m_children)
		{
			if (child->maxSize() > minSize)
			{
				minSize = child->maxSize();
			}
		}
		return minSize;
	}

	bool App::hasAnySolo() const
	{
		for (const Child* child : m_children)
		{
			if (child->solo())
			{
				return true;
			}
		}
		return false;
	}

	void App::doSave() const
	{
		const std::string filename{ cinolib::file_dialog_save() };
		if (!filename.empty())
		{
			std::ofstream file{};
			file.open(filename);
			constexpr char sep{ '\n' };
			file << m_size << sep;
			file << m_children.size() << sep;
			for (const Child* child : m_children)
			{
				for (const IVec& vert : child->verts())
				{
					file << vert << sep;
				}
			}
			file.close();
		}
	}

	void App::doLoad()
	{
		const std::string filename{ cinolib::file_dialog_open() };
		if (!filename.empty())
		{
			for (Child* child : m_children)
			{
				delete child;
			}
			m_children.clear();
			std::ifstream file{};
			file.open(filename);
			file >> m_size;
			std::size_t childrenSize{};
			file >> childrenSize;
			m_children.reserve(childrenSize);
			while (childrenSize > 0)
			{
				Child& child{ *new Child{1} };
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

	void App::doExport() const
	{}

	void App::updateColors()
	{}

	void App::drawControls()
	{
		ImGui::Spacing();
		ImGui::DragInt("Size", &m_size, 1.0f / 20.0f, minRequiredSize(), c_maxSize, "%d", ImGuiSliderFlags_AlwaysClamp);
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
		const bool anySolo{ hasAnySolo() };
		for (std::size_t i{}; i < m_children.size(); i++)
		{
			Child& child{ *m_children[i] };
			ImGui::PushID(&child);
			const Style style{ (i / static_cast<float>(m_children.size())) * 360.0f };
			style.pushImGui();
			switch (child.draw(m_size, anySolo))
			{
				case Child::EResult::Updated:
					m_size = std::max(m_size, child.maxSize());
					break;
				case Child::EResult::Removed:
					m_children.erase(m_children.begin() + i);
					delete& child;
					--i;
					break;
			}
			ImGui::Spacing();
			Style::popImGui();
			ImGui::PopID();
		}
		ImGui::Spacing();
		if (ImGui::Button("Add"))
		{
			m_children.push_back(new Child{ m_size });
		}
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
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
		ImGui::Spacing();
	}

	int App::launch()
	{
		return m_canvas.launch();
	}

}