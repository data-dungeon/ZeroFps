// zguiradiobutton.h: interface for the ZGuiRadiobutton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUIRADIOBUTTON_H__34742515_ACF4_4DF6_AB10_9A11E2DC881F__INCLUDED_)
#define AFX_ZGUIRADIOBUTTON_H__34742515_ACF4_4DF6_AB10_9A11E2DC881F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_x.h"
#include "zguicheckbox.h"

class ENGINE_API ZGuiRadiobutton : public ZGuiControl
{
public:
	ZGuiRadiobutton(Rect kRectangle, ZGuiWnd* pkParent, int iID=0, 
		int iGroupID=-1, ZGuiRadiobutton* pkPrev=NULL, bool bVisible=true);
	virtual ~ZGuiRadiobutton();
	bool Render( ZGuiRender* pkRenderer );
	void GetWndSkinsDesc(vector<SKIN_DESC>& pkSkinDesc) const; // overloaded
	void SetButtonSelectedSkin(ZGuiSkin* pkSkin);
	void SetButtonUnselectedSkin(ZGuiSkin* pkSkin);
	ZGuiCheckbox* GetButton() { return m_pkCheckbox; }
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	void SetText(char* strText, bool bResizeWnd=false); // överlagrad
	ZGuiRadiobutton* GetNext() { return m_pkNext; }
	char* GetText();// overloaded
	void Resize(int Width, int Height, bool bChangeMoveArea=true); // overloaded
	int GetGroupID() {return m_iGroupID;}

private:

	void ConnectToGroup(int iGroupID, ZGuiRadiobutton* pbNeigbour);
	ZGuiRadiobutton* GetPrev() { return m_pkPrev; }
	void SetNext(ZGuiRadiobutton* pkNext) { m_pkNext = pkNext; }
	void SetPrev(ZGuiRadiobutton* pkPrev) { m_pkPrev = pkPrev; }

	ZGuiCheckbox* m_pkCheckbox;
	int m_iGroupID;

	ZGuiRadiobutton* m_pkPrev;
	ZGuiRadiobutton* m_pkNext;
	static ZGuiRadiobutton* m_pkLastbutton;
};

#endif // !defined(AFX_ZGUIRADIOBUTTON_H__34742515_ACF4_4DF6_AB10_9A11E2DC881F__INCLUDED_)






