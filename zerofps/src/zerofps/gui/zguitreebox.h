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
	ZGuiTreebox(Rect kArea, ZGuiWnd* pkParent, bool bVisible, int iID);
	virtual ~ZGuiTreebox();

	struct Node
	{
		unsigned char ucState;			 // 0 = Normal, 1 = Closed, 2 = Open. 
		unsigned char ucSkinIndexNormal; // Kan varken st�ngas eller �ppnas. 
										 // Endast grenar som inte har n�gra 
										 // childs har en s�n bild.
		unsigned char ucSkinIndexClosed; // '+' tecknet, redo att �ppnas
		unsigned char ucSkinIndexOpen;	 // '-' tecknet, redo att st�ngas

		ZGuiCheckbox* pkButton;

		Node *pkPrev, *pkNext;

		bool bChildListIsOpen; // m�ste k�nna till detta f�r att kunna �ppna/st�nga tillr�ckligt m�nga "steg".
		vector<Node*> m_kChilds;
	};
	
	Node* AddItem(Node* pkParent, char* szText, unsigned char uiSkinNormal,
		unsigned char uiSkinClosed, unsigned char uiSkinOpen);

	ZGuiSkin* GetItemSkin(unsigned int uiIndex);

	bool InsertBranchSkin(unsigned int uiIndex, ZGuiSkin* pkSkin);
	unsigned int GetNumItemSkins();

protected:
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	
private:
	
	void ShowNode(vector<Node*> itList, bool bShow);
	void OpenNode(Node* pkNode, bool bNode);

	Node* CreateNode(Node* pkParent, char* szText, unsigned char uiSkinNormal,
		unsigned char uiSkinClosed, unsigned char uiSkinOpen);

	void CreateInternalControls();
	int m_iID;

	list<ZGuiSkin*> m_kItemSkinList;
	list<Node*> m_kNodeList;

	ZGuiScrollbar* m_pkVertScrollbar;
	ZGuiScrollbar* m_pkHorzScrollbar;
};

#endif // !defined(AFX_ZGUITREEBOX_H__BF9294AD_DC4A_4533_9A72_1D79F95BFE4F__INCLUDED_)
