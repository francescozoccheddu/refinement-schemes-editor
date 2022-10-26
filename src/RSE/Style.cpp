#include <RSE/Style.hpp>

#include <cmath>
#include <stdexcept>
#include <assert.h>

namespace RSE
{

	ImVec4 Style::col(float _saturation, float _value, float _alpha) const
	{
		const cinolib::Color hsv{ color(_saturation, _value, _alpha)};
		return ImVec4{ hsv.r(), hsv.g(), hsv.b(), hsv.a()};
	}

	Style::Style(float _hue, float _saturation, float _value)
	{
		m_h = static_cast<float>(std::fmod(_hue, 360.0f));
		if (m_h < 0.0f)
		{
			m_h += 360.0f;
		}
		if (_saturation < 0.0f || _saturation > 1.0f)
		{
			throw std::domain_error{ "saturation not in unit range" };
		}
		if (_value < 0.0f || _value > 1.0f)
		{
			throw std::domain_error{ "value not in unit range" };
		}
		m_s = _saturation;
		m_v = _value;
	}

	void Style::pushImGui() const
	{
		ImGui::PushStyleColor(ImGuiCol_Text, col(0.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, col(0.0f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, col(0.0f, 0.06f, 0.94f));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, col(0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, col(0.0f, 0.08f, 0.94f));
		ImGui::PushStyleColor(ImGuiCol_Border, col(0.14f, 0.5f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, col(0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, col(0.6666666666666666f, 0.48f, 0.54f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, col(0.7346938775510204f, 0.98f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, col(0.7346938775510204f, 0.98f, 0.67f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, col(0.0f, 0.04f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, col(0.6666666666666666f, 0.48f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, col(0.0f, 0.0f, 0.51f));
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, col(0.0f, 0.14f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, col(0.0f, 0.02f, 0.53f));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, col(0.0f, 0.31f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, col(0.0f, 0.41f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, col(0.0f, 0.51f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, col(0.7346938775510204f, 0.98f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, col(0.7272727272727273f, 0.88f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, col(0.7346938775510204f, 0.98f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, col(0.7346938775510204f, 0.98f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col(0.7346938775510204f, 0.98f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, col(0.9387755102040816f, 0.98f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Header, col(0.7346938775510204f, 0.98f, 0.31f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, col(0.7346938775510204f, 0.98f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, col(0.7346938775510204f, 0.98f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Separator, col(0.14f, 0.5f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, col(0.8666666666666667f, 0.75f, 0.78f));
		ImGui::PushStyleColor(ImGuiCol_SeparatorActive, col(0.8666666666666667f, 0.75f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ResizeGrip, col(0.7346938775510204f, 0.98f, 0.2f));
		ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, col(0.7346938775510204f, 0.98f, 0.67f));
		ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, col(0.7346938775510204f, 0.98f, 0.95f));
		ImGui::PushStyleColor(ImGuiCol_Tab, col(0.689655172413793f, 0.58f, 0.86f));
		ImGui::PushStyleColor(ImGuiCol_TabHovered, col(0.7346938775510204f, 0.98f, 0.8f));
		ImGui::PushStyleColor(ImGuiCol_TabActive, col(0.7058823529411765f, 0.68f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocused, col(0.5333333333333333f, 0.15f, 0.97f));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, col(0.6666666666666666f, 0.42f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_PlotLines, col(0.0f, 0.61f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_PlotLinesHovered, col(0.65f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col(1.0f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, col(1.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, col(0.050000000000000044f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, col(0.11428571428571424f, 0.35f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TableBorderLight, col(0.07999999999999996f, 0.25f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, col(0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, col(0.0f, 1.0f, 0.06f));
		ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, col(0.7346938775510204f, 0.98f, 0.35f));
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, col(1.0f, 1.0f, 0.9f));
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, col(0.7346938775510204f, 0.98f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_NavWindowingHighlight, col(0.0f, 1.0f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_NavWindowingDimBg, col(0.0f, 0.8f, 0.2f));
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, col(0.0f, 0.8f, 0.35f));
	}

	void Style::popImGui()
	{
		ImGui::PopStyleColor(53);
	}

	float Style::hue() const
	{
		return m_h;
	}

	float Style::saturation() const
	{
		return m_s;
	}

	float Style::value() const
	{
		return m_v;
	}

	cinolib::Color Style::color(float _saturation, float _value, float _alpha) const
	{
		assert(_saturation >= 0.0f && _saturation <= 1.0f);
		assert(_value >= 0.0f && _value <= 1.0f);
		assert(_alpha >= 0.0f && _alpha <= 1.0f);
		return cinolib::Color::hsv2rgb(m_h / 360.0f, m_s * _saturation, m_v * _value, _alpha);
	}

}