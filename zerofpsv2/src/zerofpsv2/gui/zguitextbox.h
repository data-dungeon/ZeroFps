// Textbox.h: interface for the Textbox class.

#ifndef _GUI_ZGUITEXTBOX_H
#define _GUI_ZGUITEXTBOX_H

#include "gui_x.h"
#include "zguiwindow.h"

class ZGuiRender;
class ZGuiScrollbar;

/** \brief	A Gui editbox.
	 \ingroup Gui
*/
class GUI_API ZGuiTextbox : public ZGuiWnd
{
public:
   void UpdateText();
	void ScrollRowIntoView(int row);
	bool SetCursorRow(int row, bool bUpdate);
	void ToggleMultiLine(bool bMultiLine);
	void SetLeftAlignedScrollbar(bool bLeftAligned);

	int GetCursorRow();
	bool IsMultiLine();
	//int GetNumRows();

	ZGuiTextbox(Rect kRectangle, ZGuiWnd* pkParent=NULL, bool bVisible=true, 
		int iID=0, bool m_bMultiLine=false);
	virtual ~ZGuiTextbox();

	bool Notify(ZGuiWnd* pkWnd, int iCode);
	void CreateInternalControls();
	virtual bool Render( ZGuiRender* renderer );
	bool ProcessKBInput(int nKey);
	void SetFocus(bool bSetCapture=true);
	void KillFocus();
	void SetText(char* strText, bool bResizeWnd=false); //överlagrad
	void SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, 
		ZGuiSkin* pkSkinThumbButton, ZGuiSkin* pkSkinThumbButtonHighLight,
		ZGuiSkin* pkSkinTopBnUp, ZGuiSkin* pkSkinTopBnDown,
		ZGuiSkin* pkSkinBottomBnUp, ZGuiSkin* pkSkinBottomBnDown);
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded
	void CopyNonUniqueData(const ZGuiWnd* pkSrc);
	void SetFont(ZGuiFont* pkFont); //överlagrad

   ZGuiScrollbar* GetScrollbar() { return m_pkScrollbarVertical; }

	bool IsReadOnly();
	void SetReadOnly(bool bReadOnly);
	int GetRowCount() { return m_iNumRows; }
	//bool Rescale(int iOldWidth, int iOldHeight, int iNewWidth, int iNewHeight);
	void Resize(int Width, int Height, bool bChangeMoveArea=true);
	bool m_bNumberOnly;

	//pair<int,int> GetWordLength(char *text, int offset, int max_width);

private:
	void MoveDownOneRow();
	void MoveUpOneRow();
	
	bool UpdateScrollbar();
	void UpdateDisplayList();
	
	//int GetNumRows(char* szText);
	void ScrollText(ZGuiScrollbar* pkScrollbar);
	void ScrollText(int row);
	void ResizeTextBuffer( int nCharacters );
	void SendUpdateMsg();
	
	vector<int> m_kRowOffsets;
	int* m_pkRowOffsets;
	
	bool m_bReadOnly;
	bool m_bMultiLine;
	bool m_bBlinkCursor;
	bool m_bLeftAlignedScrollbar;
	//bool m_bScrollbarUpdated;
	//bool m_bTextFit;

	int m_iCursorPos; 
	int m_iCurrMaxText;
	int m_iStartrow;
	int m_iNumRows;
	int m_iCursorRow;
	int m_iRenderDistFromTop;
	
	int m_kHorzOffset;
	int m_iTotalTextHeight;

	bool m_bUseDisplayList;
	int m_iDisplayListID;

	float m_fBlinkCursorTimer;
	int m_iBlinkCursorPos;

	ZGuiScrollbar* m_pkScrollbarVertical;
	ZGuiRender* m_pkGuiRender;



	bool BuildTagList();
	vector<TEXT_TAG> m_kTextTags;
	
	void BuildTextStrings();

	Rect m_rcOldScreenRect;
};

#endif // #ifndef _GUI_ZGUITEXTBOX_H


