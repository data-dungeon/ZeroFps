// examinepumenu.cpp: implementation of the ExaminePUMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "examinepumenu.h"
#include "../common/playercontrolproperty.h"

int ExaminePUMenu::s_iExamineMainWndID = 5000;
int ExaminePUMenu::s_iExamineListboxID = 6000;
ZGuiSkin* ExaminePUMenu::s_pkMainWndSkin = NULL;
ZGuiSkin* ExaminePUMenu::s_pkSkinItem = NULL;
ZGuiSkin* ExaminePUMenu::s_pkSkinItemSelected = NULL;
ZGuiSkin* ExaminePUMenu::s_pkSkinItemHighLight = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ExaminePUMenu::ExaminePUMenu(ZGui* pkGui, Input* pkInput, 
							 ZGuiWndProc oMainWndProc, 
							 TextureManager* pkTexMan) 
	:	DlgBox(pkGui, pkInput, oMainWndProc)
{
	m_kListbox = NULL;
	m_pkItemProperty = NULL;
	m_pkTexMan = pkTexMan;
	m_iWidth = 150;
	m_iHeight = 300;
	m_iPickedListIndex = -1;
	m_eActionType = NORMALUSE;
}

ExaminePUMenu::~ExaminePUMenu()
{

}

bool ExaminePUMenu::Create(int x, int y, char* szResourceFile, char* szDlgName)
{
	// Create transparent transparent
	if(s_pkMainWndSkin == NULL)
		s_pkMainWndSkin = new ZGuiSkin(0,0,0, 0,0,0, 0, true);
	if(s_pkSkinItem == NULL)
		s_pkSkinItem = new ZGuiSkin(0,192,50, 0,0,0, 0, false);
	if(s_pkSkinItemSelected == NULL)
		s_pkSkinItemSelected = new ZGuiSkin(0,192,50, 0,0,0, 1, false);
	if(s_pkSkinItemHighLight == NULL)
		s_pkSkinItemHighLight = new ZGuiSkin(128,128,255, 0,0,0, 1, false);

	m_pkDlgBox = new ZGuiWnd(Rect(x,y,x+m_iWidth,y+m_iHeight),NULL,false,s_iExamineMainWndID++);
	m_pkDlgBox->SetSkin(s_pkMainWndSkin);

	m_kListbox = new ZGuiListbox(Rect(0,0,m_iWidth,m_iHeight), m_pkDlgBox, true, 
		s_iExamineListboxID++, 20, s_pkSkinItem, s_pkSkinItemSelected,
		s_pkSkinItemHighLight);
	m_kListbox->IsMenu(true);

	char* name = GenMainWndName();
	m_pkGui->AddMainWindow(s_iExamineMainWndID++, m_pkDlgBox, name, 
		m_oMainWndProc, false);
	delete[] name;

	return true;
}

bool ExaminePUMenu::DlgProc( ZGuiWnd* pkWnd, unsigned int uiMsg, int iNumParams,
							 void *pkParams )
{
	switch(uiMsg)
	{
	// Command Messages
	case ZGM_SELECTLISTITEM:
		m_iPickedListIndex = ((int*)pkParams)[1]; // list item index
		OnClose(true);
		break;
	}
	return true;
}

bool ExaminePUMenu::OnOpen(int x, int y)
{
	if(m_pkItemProperty == NULL)
		return false;

	vector<string> akNames;
	m_pkItemProperty->GetUses(m_eActionType, &akNames);

	bool bPickable = m_pkItemProperty->m_bPickable && (m_eActionType == NORMALUSE);
	int iNumItems = akNames.size();

	if(bPickable)
		iNumItems++;

	if(iNumItems == 0)
		return false;

	// Resize windows to fit all items.
	m_iHeight = iNumItems*20;

	m_pkDlgBox->Resize(m_iWidth, m_iHeight);
	m_kListbox->Resize(m_iWidth, m_iHeight);
	
	// Add new items to listbox.
	m_kListbox->RemoveAllItems();

	int counter = 0, i=0;

	if(bPickable)
	{
		m_kListbox->AddItem("Pick up", counter++, false);
		iNumItems--;
	}

	for(i=0; i<iNumItems; i++)
		m_kListbox->AddItem((char*)akNames[i].c_str(), counter++, false);

	m_pkGui->ShowMainWindow(m_pkDlgBox, true);

	if(!(x==-1 && y==-1))
	{
		x -= m_pkDlgBox->GetScreenRect().Width()/2;
		y -= m_pkDlgBox->GetScreenRect().Height()/2;

		SetPos(x,y);
	}

	CenterCursor();

	return true;
}

bool ExaminePUMenu::OnClose(bool bSave)
{
	if(bSave == true)
	{
		if(m_iPickedListIndex != -1 && m_pkItemProperty)
		{
			m_pkItemProperty->Use( m_eActionType, 
				m_kListbox->GetItem(m_iPickedListIndex)->GetText());
		}

		if(m_pkPlayerProp && m_pkItemProperty->m_bPickable &&
			m_iPickedListIndex == 0)
		{
			m_pkPlayerProp->PickUp( m_pkItemProperty->GetObject() );
		}
	}

	m_iPickedListIndex = -1;
	m_pkItemProperty = NULL;
	m_pkPlayerProp = NULL;

	m_pkGui->ShowMainWindow(m_pkDlgBox, false);

//	m_pkGui->ShowCursor(false);

	KillFocus();

	return true;
}

void ExaminePUMenu::SetItemProperty(ItemProperty *pkItemProp)
{
	m_pkItemProperty = pkItemProp;
}

void ExaminePUMenu::SetPlayerControlProperty(PlayerControlProperty *pkPlayerProp)
{
	m_pkPlayerProp = pkPlayerProp;
}

char* ExaminePUMenu::GenMainWndName()
{
	char* szApa = new char[50];
	sprintf(szApa, "ExaminePUMainWnd%i", s_iExamineMainWndID);
	return szApa;
}

char* ExaminePUMenu::GetUseString(int index)
{
	if(m_kListbox == NULL)
		return "null";

	if(index >= 0 && index < m_kListbox->GetItemCount())
		return m_kListbox->GetItem(index)->GetText();

	return "null";
}

void ExaminePUMenu::SetUseState(Action_Type eNewActionType)
{
	m_eActionType = eNewActionType;
}
