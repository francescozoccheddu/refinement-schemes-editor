#pragma once

#include <cinolib/gl/glcanvas.h>
#include <RSE/types.hpp>
#include <RSE/AxesWidget.hpp>
#include <RSE/Child.hpp>
#include <RSE/Style.hpp>
#include <vector>

namespace RSE
{

	class App final
	{

	private:

		static constexpr Int c_maxSize{ 10 };

		cinolib::GLcanvas m_canvas;
		AxesWidget m_axesWidget;
		std::vector<Child*> m_children;
		Int m_size;

		Int minRequiredSize() const;
		bool hasAnySolo() const;
		void drawControls();
		void doSave() const;
		void doLoad();
		void doExport() const;
		void updateColors();

	public:

		App();

		int launch();

	};

}