// zguiradiobutton.h: interface for the ZGuiRadiobutton class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GUI_ZGUIRADIOBUTTON_H_
#define _GUI_ZGUIRADIOBUTTON_H_

#include "gui_x.h"
#include "zguicheckbox.h"

class GUI_API ZGuiRadiobutton : public ZGuiControl
{
public:
	void ChangeGroupName(char* szNewName);
	ZGuiRadiobutton(Rect kRectangle, ZGuiWnd* pkParent, int iID=0, 
		int iGroupID=-1, char* szGroupName=NULL, 
		ZGuiRadiobutton* pkPrev=NULL, bool bVisible=true);
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
	char* GetGroupName() {return m_szGroupName;}
	void CopyNonUniqueData(const ZGuiWnd* pkSrc);

private:

	void ConnectToGroup(int iGroupID, ZGuiRadiobutton* pbNeigbour);
	ZGuiRadiobutton* GetPrev() { return m_pkPrev; }
	void SetNext(ZGuiRadiobutton* pkNext) { m_pkNext = pkNext; }
	void SetPrev(ZGuiRadiobutton* pkPrev) { m_pkPrev = pkPrev; }

	ZGuiCheckbox* m_pkCheckbox;
	int m_iGroupID;
	char m_szGroupName[50]; // Same name as the resource manager use to find the window.

	ZGuiRadiobutton* m_pkPrev;
	ZGuiRadiobutton* m_pkNext;
	static ZGuiRadiobutton* m_pkLastbutton;
};

#endif 






