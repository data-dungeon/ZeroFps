// zguicursor.h: interface for the ZGuiCursor class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUICURSOR_H_
#define _GUI_ZGUICURSOR_H_

#include "gui_x.h"
#include "../basic/rect.h"

class GUI_API ZGuiCursor 
{
public:

	ZGuiCursor();
	virtual ~ZGuiCursor();

	void Show(bool bShow);
	bool IsVisible();
	void SetSkin(ZGuiSkin* pkSkin);
	void SetSize(int Width, int Height);
	void SetPos(int x, int y);
	void Render();

private:
	Rect m_kRect;
	ZGuiSkin* m_pkSkin;
	ZGuiRender* m_pkRenderer;
	bool m_bVisible;
};

#endif 
