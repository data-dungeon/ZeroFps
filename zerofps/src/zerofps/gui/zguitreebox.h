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

class ZGuiCheckbox;
class ZGuiScrollbar;
class ZGuiRender;

class GUI_API ZGuiTreebox : public ZGuiWnd
{
public:
	void PrintHierarchy();
	ZGuiTreebox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID);
	virtual ~ZGuiTreebox();

	struct Node
	{
		ZGuiCheckbox* pkButton;

		unsigned char ucSkinIndex;	
		unsigned char ucSkinIndexSelected;
		unsigned int uiRootLevel;
		unsigned int uiChildIndex;
		
		Node *pkParent, *pkNext;

		bool bChildListIsOpen; // måste känna till detta för att kunna öppna/stänga tillräckligt många "steg".
		vector<Node*> kChilds;
	};

	typedef vector<Node*>::iterator itChild;
	typedef list<Node*>::iterator itNode;
	typedef list<Node*>::reverse_iterator ritNode;
	
	Node* AddItem(Node* pkParent, char* szText, unsigned char ucSkinIndex, 
		unsigned char ucSkinIndexSelected);

	ZGuiSkin* GetItemSkin(unsigned int uiIndex);

	bool InsertBranchSkin(unsigned int uiIndex, ZGuiSkin* pkSkin);
	unsigned int GetNumItemSkins();

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	
private:
	void MoveNode(Node* pkNode, int steps);
	void ShowNode(vector<Node*> itList, bool bShow);
	void OpenChilds(vector<Node*> kChilds, bool bOpen);
	void OpenNode(Node* pkNode, bool bOpen);
	
	void PrintNode(Node* pkNode);
	void PrintChilds(vector<Node*> kList);

	Node* CreateNode(Node* pkParent, char* szText, unsigned char ucSkinIndex,
		unsigned char ucSkinIndexSelected);

	void CreateInternalControls();
	int m_iID;

	list<ZGuiSkin*> m_kItemSkinList;
	list<Node*> m_kNodeList;

	ZGuiScrollbar* m_pkVertScrollbar;
	ZGuiScrollbar* m_pkHorzScrollbar;
};

#endif // !defined(AFX_ZGUITREEBOX_H__BF9294AD_DC4A_4533_9A72_1D79F95BFE4F__INCLUDED_)
