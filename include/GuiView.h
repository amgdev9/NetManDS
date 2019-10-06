/**
 * @file GuiView.h
 * @brief 
 */

#ifndef SOURCE_GUIVIEW_H_
#define SOURCE_GUIVIEW_H_

namespace NetMan {

class GuiView {
	public:
		virtual processEvents(u32 held, u32 down, u32 up, touchPosition *touch) = 0;
		virtual drawTop() = 0;
		virtual drawBottom() = 0;
};

}

#endif
