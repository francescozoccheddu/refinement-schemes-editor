#pragma once

#include <cinolib/gl/glcanvas.h>
#include <RSE/types.hpp>
#include <RSE/AxesWidget.hpp>
#include <RSE/ChildControl.hpp>
#include <RSE/PolyControl.hpp>
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
		std::vector<ChildControl*> m_children;
		RPolyControl m_polyControl;
		Int m_size;
		ChildControl* m_activeChild;

		Int minRequiredSize() const;
		bool hasAnySolo() const;
		void drawControls();
		void doClear();
		void doSave() const;
		void doLoad();
		void doExport() const;
		void updateColors();

	public:

		App();

		int launch();

	};

}