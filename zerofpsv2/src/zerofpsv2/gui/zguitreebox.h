// zguitreebox.h: interface for the ZGuiTreebox class.

#ifndef _GUI_ZGUITREEBOX_H
#define _GUI_ZGUITREEBOX_H

#include "gui_x.h"
#include "zguiwindow.h"
#include <map>

class ZGuiCheckbox;
class ZGuiScrollbar;
class ZGuiRender;

struct GUI_API ZGuiTreeboxNode
{
	ZGuiCheckbox* pkButton;

	unsigned char ucSkinIndex;	
	unsigned char ucSkinIndexSelected;
	ZGuiTreeboxNode *pkParent, *pkNext;
	list<ZGuiTreeboxNode*> kChilds;

	bool bIsOpen; // samma sak som m_bVisible fast ignoreras av renderingen
				  // och används när höjden på träden skall räknas fram.
	bool bChildListIsOpen; // måste känna till detta för att kunna öppna/stänga
							//  tillräckligt många "steg".

	ZGuiTreeboxNode* RemoveNode(ZGuiTreeboxNode* pkNode);
};

class GUI_API ZGuiTreebox : public ZGuiWnd
{
public:
	
	ZGuiTreeboxNode* Node(string strName);
	ZGuiTreeboxNode* Root();
	void PrintHierarchy();
	ZGuiTreebox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID);
	virtual ~ZGuiTreebox();

	//typedef vector<ZGuiTreeboxNode*>::iterator itChild;
	typedef list<ZGuiTreeboxNode*>::iterator itNode;
	typedef list<ZGuiTreeboxNode*>::reverse_iterator ritNode;
	
	ZGuiTreeboxNode* AddItem(ZGuiTreeboxNode* pkParent, char* szText, 
		unsigned char ucSkinIndex, unsigned char ucSkinIndexSelected,
		const char* szIDName=NULL);

	ZGuiTreeboxNode* AddItem(string strParent, char* szText, 
		unsigned char ucSkinIndex, unsigned char ucSkinIndexSelected,
		const char* szIDName=NULL);

	bool DeleteNode(string strIDName);
	
	ZGuiSkin* GetItemSkin(unsigned int uiIndex);

	bool InsertBranchSkin(unsigned int uiIndex, ZGuiSkin* pkSkin);
	unsigned int GetNumItemSkins();

	void SetScrollbarSkin(ZGuiSkin* pkSkinScrollArea, 
		ZGuiSkin* pkSkinThumbButton, ZGuiSkin* pkSkinThumbButtonHighLight,
		ZGuiSkin* pkSkinTopBnUp, ZGuiSkin* pkSkinTopBnDown,
		ZGuiSkin* pkSkinBottomBnUp, ZGuiSkin* pkSkinBottomBnDown);

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	
private:
	ZGuiTreeboxNode* RemoveNode(ZGuiTreeboxNode *pkNode);
	ZGuiTreeboxNode* FindLastChild(ZGuiTreeboxNode* pkFrom);

	bool ProcessKBInput(int iKey);
	void SetSelectionMarker(ZGuiTreeboxNode* pkNode);
	void ScrollRows();
	void ScrollCols();
	void ChangeScrollbarRange(int width, int height, bool bVerticalScrollbar);
	void MoveNode(ZGuiTreeboxNode* pkNode, int steps, bool bRecursive=true);
	void OpenChilds(list<ZGuiTreeboxNode*> kChilds, bool bOpen);
	void OpenNode(ZGuiTreeboxNode* pkNode, bool bOpen);
	
	void PrintNode(ZGuiTreeboxNode* pkNode);
	void PrintChilds(list<ZGuiTreeboxNode*> kList);

	ZGuiTreeboxNode* CreateNode(ZGuiTreeboxNode* pkParent, char* szText, 
		unsigned char ucSkinIndex,unsigned char ucSkinIndexSelected);

	bool DeleteNode(ZGuiTreeboxNode* pkNode, bool bRemoveFromMap);

	void CreateInternalControls();
	int m_iID;

	list<ZGuiSkin*> m_kItemSkinList;
	list<ZGuiTreeboxNode*> m_kNodeList;

	ZGuiTreeboxNode* m_pkSelectedNode;
	ZGuiScrollbar* m_pkVertScrollbar;
	ZGuiScrollbar* m_pkHorzScrollbar;
	ZGuiLabel* m_pkSelLabel;
	int m_iStartrow, m_iStartcol;
	int m_iItemWidth, m_iItemHeight;
	map<string,ZGuiTreeboxNode*> m_kNodeMap;

	int PREV_VERT_SCROLLROW, PREV_HORZ_SCROLLCOL;
};

#endif // #ifndef _GUI_ZGUITREEBOX_H
