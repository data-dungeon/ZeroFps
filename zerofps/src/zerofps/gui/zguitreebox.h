// zguitreebox.h: interface for the ZGuiTreebox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUITREEBOX_H__BF9294AD_DC4A_4533_9A72_1D79F95BFE4F__INCLUDED_)
#define AFX_ZGUITREEBOX_H__BF9294AD_DC4A_4533_9A72_1D79F95BFE4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
		ZGuiSkin* pkSkinThumbButton, ZGuiSkin* pkSkinThumbButtonHighLight);

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	
private:
	ZGuiTreeboxNode* RemoveNode(ZGuiTreeboxNode *pkNode);
	ZGuiTreeboxNode* FindLastChild(ZGuiTreeboxNode* pkFrom);

	void SetSelColor(ZGuiTreeboxNode* pkNode);
	void ScrollRows(bool bVertically);
	void ChangeScrollbarRange(int width, int height);
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
	int m_iStartrow;
	int m_iItemWidth, m_iItemHeight;
	map<string,ZGuiTreeboxNode*> m_kNodeMap;
};

#endif // !defined(AFX_ZGUITREEBOX_H__BF9294AD_DC4A_4533_9A72_1D79F95BFE4F__INCLUDED_)
