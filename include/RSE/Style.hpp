#pragma once

#include <cinolib/color.h>
#include <imgui.h>

namespace RSE
{

	class Style final
	{

	private:

		float m_h, m_s, m_v;

		ImVec4 col(float _saturation, float _value, float _alpha = 1.0f) const;

	public:

		Style(float _hue, float _saturation = 1.0f, float _value = 1.0f);

		void pushImGui() const;

		static void popImGui();

		float hue() const;
		float saturation() const;
		float value() const;

		cinolib::Color color(float _saturation = 1.0f, float _value = 1.0f, float _alpha = 1.0f) const;

	};

}