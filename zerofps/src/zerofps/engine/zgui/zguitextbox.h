// Textbox.h: interface for the Textbox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTBOX_H__BEB2C88E_6983_4DF8_8EB3_506B3304FB7B__INCLUDED_)
#define AFX_TEXTBOX_H__BEB2C88E_6983_4DF8_8EB3_506B3304FB7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_x.h"
#include "zguicontrol.h"

class ZGuiRender;
class ZGuiScrollbar;

class ENGINE_API ZGuiTextbox : public ZGuiControl
{
public:
	ZGuiTextbox(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, int iID=0, bool m_bMultiLine=false);
	virtual ~ZGuiTextbox();

	bool Notify(ZGuiWnd* pkWnd, int iCode);
	void CreateInternalControls();
	virtual bool Render( ZGuiRender* renderer );
	bool ProcessKBInput(int nKey);
	void SetFocus();
	void KillFocus();
	void SetText(char* strText);
	void SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, ZGuiSkin* pkSkinThumbButton, 
		ZGuiSkin* pkSkinThumbButtonHighLight);

private:
	void ScrollText(ZGuiScrollbar* pkScrollbar);
	void ResizeTextBuffer( int nCharacters );
	bool m_bBlinkCursor;
	bool m_bSingleLine;
	int m_iCursorPos, m_iCurrMaxText;
	int m_iFontSize;
	int m_iMaxCharsOneRow;
	int m_iMaxVisibleRows;
	int m_iStartrow;

	ZGuiScrollbar* m_pkScrollbarVertical;
	bool m_bMultiLine;
};

#endif // !defined(AFX_TEXTBOX_H__BEB2C88E_6983_4DF8_8EB3_506B3304FB7B__INCLUDED_)


