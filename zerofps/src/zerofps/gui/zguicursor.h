// zguicursor.h: interface for the ZGuiCursor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUICURSOR_H__6456F063_B56D_440F_AAB2_BCCA4B48C77B__INCLUDED_)
#define AFX_ZGUICURSOR_H__6456F063_B56D_440F_AAB2_BCCA4B48C77B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gui_x.h"
#include "../basic/rect.h"

class GUI_API ZGuiCursor 
{
public:

	ZGuiCursor();
	virtual ~ZGuiCursor();

	void Show(bool bShow);
	bool IsVisible();
	void SetSkin(ZGuiSkin* pkSkin/*, int iMaskTexture=-1*/);
	void SetSize(int Width, int Height);
	void SetPos(int x, int y);
	void Render();

private:
	Rect m_kRect;
	ZGuiSkin* m_pkSkin;
//	int m_iMaskTexture;
	ZGuiRender* m_pkRenderer;
	bool m_bVisible;
};

#endif // !defined(AFX_ZGUICURSOR_H__6456F063_B56D_440F_AAB2_BCCA4B48C77B__INCLUDED_)
