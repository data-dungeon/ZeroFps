// Textbox.h: interface for the Textbox class.

#ifndef _GUI_ZGUITEXTBOX_H
#define _GUI_ZGUITEXTBOX_H

#include "gui_x.h"
#include "zguiwindow.h"

class ZGuiRender;
class ZGuiScrollbar;

class GUI_API ZGuiTextbox : public ZGuiWnd
{
public:
	void ScrollRowIntoView(int row);
	bool SetCursorRow(int row, bool bUpdate);
	void ToggleMultiLine(bool bMultiLine);
	int GetCursorRow();
	bool IsMultiLine();
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
		ZGuiSkin* pkSkinThumbButton, ZGuiSkin* pkSkinThumbButtonHighLight,
		ZGuiSkin* pkSkinTopBnUp, ZGuiSkin* pkSkinTopBnDown,
		ZGuiSkin* pkSkinBottomBnUp, ZGuiSkin* pkSkinBottomBnDown);
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded
	void CopyNonUniqueData(const ZGuiWnd* pkSrc);
	void SetFont(ZGuiFont* pkFont); //överlagrad

	void SetReadOnly(bool bReadOnly);
	int GetRowCount() { return m_iNumRows; }

	bool m_bNumberOnly;

private:
	void MoveDownOneRow();
	void MoveUpOneRow();
	
	bool UpdateScrollbar();
	pair<int,int> GetWordLength(char *text, int offset, int max_width);
	int GetNumRows(char* szText);
	bool IgnoreKey(int Key);
	void ScrollText(ZGuiScrollbar* pkScrollbar);
	void ScrollText(int row);
	void ResizeTextBuffer( int nCharacters );
	
	vector<int> m_kRowOffsets;
	
	bool m_bReadOnly;
	bool m_bMultiLine;
	bool m_bBlinkCursor;
	bool m_bScrollbarUpdated;
	bool m_bTextFit;

	int m_iCursorPos; 
	int m_iCurrMaxText;
	int m_iStartrow;
	int m_iNumRows;
	int m_iCursorRow;
	int m_iRenderDistFromTop;
	
	int m_kHorzOffset;

	ZGuiScrollbar* m_pkScrollbarVertical;
};

#endif // #ifndef _GUI_ZGUITEXTBOX_H


