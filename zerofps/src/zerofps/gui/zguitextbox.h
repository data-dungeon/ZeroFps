// Textbox.h: interface for the Textbox class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUITEXTBOX_H_
#define _GUI_ZGUITEXTBOX_H_

#include "gui_x.h"
#include "zguiwindow.h"

class ZGuiRender;
class ZGuiScrollbar;

class GUI_API ZGuiTextbox : public ZGuiWnd
{
public:
	bool IsMultiLine();
	int GetRowLength(int iRow);
	int GetNumRows();

	ZGuiTextbox(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, 
		int iID=0, bool m_bMultiLine=false);
	virtual ~ZGuiTextbox();

	bool Notify(ZGuiWnd* pkWnd, int iCode);
	void CreateInternalControls();
	virtual bool Render( ZGuiRender* renderer );
	bool ProcessKBInput(int nKey);
	void SetFocus();
	void KillFocus();
	void SetText(char* strText, bool bResizeWnd=false); //överlagrad
	void SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, 
		ZGuiSkin* pkSkinThumbButton, ZGuiSkin* pkSkinThumbButtonHighLight);
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded
	void CopyNonUniqueData(const ZGuiWnd* pkSrc);
	void SetFont(ZGuiFont* pkFont); //överlagrad

private:
	int m_iRenderDistFromTop;
	bool UpdateScrollbar();
	pair<int,int> GetWordLength(char *text, int offset);
	int GetNumRows(char* szText);
	bool IgnoreKey(int Key);
	void ScrollText(ZGuiScrollbar* pkScrollbar);
	void ResizeTextBuffer( int nCharacters );
	bool m_bBlinkCursor;
	bool m_bSingleLine;
	bool m_bScrollbarUpdated;
	int m_iCursorPos, m_iCurrMaxText;
	int m_iFontSize;
	int m_iMaxCharsOneRow;
	int m_iMaxVisibleRows;
	int m_iStartrow;
	int m_iNumRows;
	int m_iCursorRow;

	ZGuiScrollbar* m_pkScrollbarVertical;
	bool m_bMultiLine;
};

#endif 


