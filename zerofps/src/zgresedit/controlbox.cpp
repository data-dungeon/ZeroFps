// Controlbox.cpp: implementation of the ControlBox class.
//
//////////////////////////////////////////////////////////////////////

#include "controlbox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ControlBox::ControlBox(GuiBuilder* pkGuiBuilder) : DlgBox(pkGuiBuilder)
{

}

ControlBox::~ControlBox()
{

}

bool ControlBox::Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc)
{
	m_pkDlgBox = m_pkGuiBuilder->CreateMainWindow(ID_CTRLBOX_MAIN_WND,
		ID_CTRLBOX_WND,x,y,w,h,pkWndProc,"blue");
	m_pkGuiBuilder->Register(m_pkDlgBox, "CtrlBoxWnd");
		
	ZGuiWnd* pkResMenuBn;

	// Create controller type radio butons.
	typedef pair<string, string> name_label;
	typedef pair<int, name_label> controller;
	int id_start = ID_CTRL_WINDOW_BN;
	controller akCtrls[] = 
	{
		controller(id_start++, name_label("SelWndCtrlRb","Window")),				// 0
		controller(id_start++, name_label("SelLabelCtrlRb","Label")),				// 1
		controller(id_start++, name_label("SelTextboxCtrlRb","Textbox")),			// 2
		controller(id_start++, name_label("SelButtonCtrlRb","Button")),				// 3
		controller(id_start++, name_label("SelCheckboxCtrlRb","Check Box")),		// 4
		controller(id_start++, name_label("SelRadioButtonCtrlRb","Radio Button")),	// 5
		controller(id_start++, name_label("SelComboboxCtrlRb","Combo Box")),		// 6
		controller(id_start++, name_label("SelListboxCtrlRb","List Box")),			// 7
		controller(id_start++, name_label("SelScrollbarCtrlRb","Scroll Bar")),		// 8
	};

	w = 16; h = 16; x = 10; y = 10; 
	for(int i=0; i<sizeof(akCtrls) / sizeof(akCtrls[1]); i++)
	{
		pkResMenuBn = m_pkGuiBuilder->CreateRadioButton(m_pkDlgBox,akCtrls[i].first,
			CTRLMENU_RADIO_GROUP,x,y,w,h,(char*)akCtrls[i].second.second.c_str());
		m_pkGuiBuilder->Register(pkResMenuBn, (char*)akCtrls[i].second.first.c_str());
		y += 20;
	}

	// Create open skins button.
	w = 60; h = 20; x = 10; 
	ZGuiWnd* pkOpenSkinsBn = m_pkGuiBuilder->CreateButton(m_pkDlgBox, 
		ID_CTRLBOX_OPENSKINDLG_BN, x, y, w, h, "Skins...");
	pkOpenSkinsBn->SetWindowFlag(WF_CENTER_TEXT);

	return true;
}

bool ControlBox::OnOpen(int x, int y)
{
	m_pkDlgBox->Show();
	return true;
}

bool ControlBox::OnClose(bool bSave)
{
	m_pkDlgBox->Hide();
	return true;
}
