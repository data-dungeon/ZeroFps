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

	struct Branch
	{
		bool bOpen;
		Branch* pkParent;
		ZGuiCheckbox* pkOpenCloseButton;
		vector<ZGuiCheckbox*> akLeafs; 
	};

	ZGuiSkin* m_pkCloseBnSkin;
	ZGuiSkin* m_pkOpenBnSkin;

	vector<ZGuiSkin*> m_pkItemSkins;
	vector<Branch*> m_akBranshes;

	ZGuiScrollbar* m_pkVertScrollbar;
	ZGuiScrollbar* m_pkHorzScrollbar;

	Branch* AddItem(Branch* pkParent, char* szText, int iSkin);
	bool Render(ZGuiRender* pkRenderer );

	int m_iID;
};

#endif // !defined(AFX_ZGUITREEBOX_H__BF9294AD_DC4A_4533_9A72_1D79F95BFE4F__INCLUDED_)
