// propertybox.cpp: implementation of the PropertyBox class.
//
//////////////////////////////////////////////////////////////////////

#include "propertybox.h"
#include "../zerofps/engine/input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PropertyBox::PropertyBox(GuiBuilder* pkGuiBuilder) : DlgBox(pkGuiBuilder)
{

}

PropertyBox::~PropertyBox()
{

}

bool PropertyBox::Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc)
{
	m_pkDlgBox = m_pkGuiBuilder->CreateMainWindow(
		ID_CTRLPROPERTYBOX_MAIN_WND,ID_CTRLPROPERTYBOX_WND,x,y,w,h,
		pkWndProc,"red");
	m_pkGuiBuilder->Register(m_pkDlgBox, "CtrlPropBoxWnd");

	int x2, y2, w2, h2;

	// ID Number
	x2 = 10; y2 = 10; w2 = 100; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, x2, y2, w2, h2, "ID:");
	x2 = 100; y2 = 10; w2 = 150; h2 = 20;
	ZGuiWnd* pkIDField = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, 
		ID_CTRLID_EB, x2, y2, w2, h2, false);
	m_pkGuiBuilder->Register(pkIDField, "CtrlPropIDEB");

	// Caption
	x2 = 10; y2 = 40; w2 = 100; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, x2, y2, w2, h2, "Caption:");
	x2 = 100; y2 = 40; w2 = 150; h2 = 20;
	ZGuiWnd* pkCaptionField = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, 
		ID_CTRLCAPTION_EB, x2, y2, w2, h2, false);
	m_pkGuiBuilder->Register(pkCaptionField, "CtrlPropCaptionEB");

	// OK button
	ZGuiWnd* pkOKButton = m_pkGuiBuilder->CreateOKButton(m_pkDlgBox, 
		ID_CTRLOK_BN);

	// Close button
	m_pkGuiBuilder->CreateCloseButton(m_pkDlgBox, ID_CTRLCANCEL_BN);

	// Bind return key to textboxes-ok button
	m_pkGui->AddKeyCommand(KEY_RETURN, pkIDField, pkOKButton);
	m_pkGui->AddKeyCommand(KEY_RETURN, pkCaptionField, pkOKButton);

	// Skins
	x2 = 10; y2 = 70; w2 = 100; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, x2, y2, w2, h2, "Skin:");
	x2 = 100; y2 = 70; w2 = 150; h2 = 20;
	ZGuiCombobox* pkSkinbox = m_pkGuiBuilder->CreateCombobox(m_pkDlgBox,
		ID_CTRLSKIN_CB, x2, y2, w2, h2, false);
	pkSkinbox->SetGUI(m_pkGui);
	m_pkGuiBuilder->Register(pkSkinbox, "CtrlPropSkinCB");

	pkSkinbox->AddItem("Apa 1",331,true);
	pkSkinbox->AddItem("Apa 2",332);
	pkSkinbox->AddItem("Apa 3",333);

	m_pkGui->ShowMainWindow(ID_CTRLPROPERTYBOX_MAIN_WND, false);

	return true;
}

bool PropertyBox::OnOpen(int x, int y)
{
	if(m_pkWndEdit == NULL)
		return false;

	if(m_pkDlgBox->IsVisible() == false)
	{
		if(!(x==-1 && y==-1))
			m_pkDlgBox->SetPos(x, y, true, true);

		m_pkDlgBox->SetMoveArea(m_pkGuiBuilder->m_rcScreen);
	}

	m_pkGui->ShowMainWindow(ID_CTRLPROPERTYBOX_MAIN_WND, true);

	char szID[50];
	sprintf(szID, "%i", m_pkWndEdit->GetID());
	m_pkGuiBuilder->GetWnd("CtrlPropIDEB")->SetText(szID);
	m_pkGuiBuilder->GetWnd("CtrlPropCaptionEB")->SetText(m_pkWndEdit->GetText());
	return true;
}

bool PropertyBox::OnClose(bool bSave)
{
	if(m_pkWndEdit && bSave)
	{
		m_pkWndEdit->SetText(m_pkGuiBuilder->GetWnd("CtrlPropCaptionEB")->GetText());
		m_pkWndEdit->SetID(atoi(m_pkGuiBuilder->GetWnd("CtrlPropIDEB")->GetText()));
	}

	m_pkGui->ShowMainWindow(ID_CTRLPROPERTYBOX_MAIN_WND, false);

	return true;
}

void PropertyBox::SetEditWnd(ZGuiWnd *pkWnd)
{
	m_pkWndEdit = pkWnd;
}
