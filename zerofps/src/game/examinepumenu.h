// examinepumenu.h: interface for the ExaminePUMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXAMINEPUMENU_H__491DA0D1_C503_4E21_B9FC_80EF1F935D11__INCLUDED_)
#define AFX_EXAMINEPUMENU_H__491DA0D1_C503_4E21_B9FC_80EF1F935D11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../common/dialogbox.h"

class TextureManager;	
class PlayerControlProperty;
#include "../common/itemproperty.h"

class ExaminePUMenu : public DlgBox
{
public:
	char* GetUseString(int index);
	void SetPlayerControlProperty(PlayerControlProperty *pkPlayerProp);
	void SetItemProperty(ItemProperty* pkItemProp);
	ExaminePUMenu(ZGui* pkGui, Input* pkInput, ZGuiWndProc oMainWndProc, TextureManager* pkTexMan);
	virtual ~ExaminePUMenu();

	bool Create(int x, int y, char* szResourceFile, char* szDlgName);
	bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams );

	void SetUseState(Action_Type eNewActionType);

private:

	bool OnOpen(int x=-1, int y=-1);
	bool OnClose(bool bSave);

	char* GenMainWndName();
	TextureManager* m_pkTexMan;
	static ZGuiSkin* s_pkMainWndSkin;
	static ZGuiSkin* s_pkSkinItem;
	static ZGuiSkin* s_pkSkinItemSelected;
	static ZGuiSkin* s_pkSkinItemHighLight;

	static int s_iExamineMainWndID, s_iExamineListboxID;
	ZGuiListbox* m_kListbox;
	int m_iWidth, m_iHeight;
	int m_iPickedListIndex;

	ItemProperty* m_pkItemProperty;
	PlayerControlProperty* m_pkPlayerProp;

	Action_Type m_eActionType;

};

#endif // !defined(AFX_EXAMINEPUMENU_H__491DA0D1_C503_4E21_B9FC_80EF1F935D11__INCLUDED_)
