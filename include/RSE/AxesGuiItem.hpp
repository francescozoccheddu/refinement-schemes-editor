#pragma once

#include <RSE/types.hpp>
#include <cinolib/gl/FreeCamera.hpp>
#include <cinolib/gl/canvas_gui_item.h>

namespace RSE
{

	class AxesGuiItem final : public cinolib::CanvasGuiItem
	{

	private:

		const cinolib::FreeCamera<Real>* m_camera;

	public:

		explicit AxesGuiItem(const cinolib::FreeCamera<Real>& _camera);

		const cinolib::FreeCamera<Real>& camera() const;

		void draw() override;

	};

}