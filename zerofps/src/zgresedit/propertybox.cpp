// propertybox.cpp: implementation of the PropertyBox class.
//
///////////////////////////////////////////////////////////////////////////////

#include "propertybox.h"
#include "../zerofps/engine/input.h"
#include "selectwnd.h"
#include <typeinfo>

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//
PropertyBox::PropertyBox(GuiBuilder* pkGuiBuilder, ZGuiWndProc oMainWndProc) 
	: DlgBox(pkGuiBuilder, oMainWndProc)
{

}

PropertyBox::~PropertyBox()
{

}

///////////////////////////////////////////////////////////////////////////////
// Name: Create
// Description: 
//
bool PropertyBox::Create(int x,int y,int w,int h,ZGuiWndProc pkWndProc)
{
	m_pkDlgBox = m_pkGuiBuilder->CreateMainWindow(
		ID_CTRLPROPERTYBOX_MAIN_WND,ID_CTRLPROPERTYBOX_WND,
		"CtrlPropBoxWnd", x,y,w,h, pkWndProc);

	int x2,y2,w2,h2;

	char szText[100];

	// Label
	sprintf(szText,"%s","Control"); 
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,"PropertyBoxLabel",10,10,300,20,szText);

	// ID Number
	x2 = 10; y2 = 30; w2 = 100; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x2,y2,w2,h2,"ID:");
	x2 = 100; y2 = 30; w2 = 150; h2 = 20;
	ZGuiWnd* pkIDField = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_CTRLID_EB,"CtrlPropIDEB",x2,y2,w2,h2,false);
	pkIDField->SetWindowFlag(WF_CANHAVEFOCUS);
	pkIDField->Disable();

	// ID Name
	x2 = 10; y2 = 60; w2 = 100; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x2,y2,w2,h2,"Res. Name:");
	x2 = 100; y2 = 60; w2 = 150; h2 = 20;
	ZGuiWnd* pkIDNameField = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_CTRLNAME_EB,"CtrlPropIDName",x2,y2,w2,h2,false);
	pkIDNameField->SetWindowFlag(WF_CANHAVEFOCUS);
	//pkIDNameField->Disable();

	// Caption
	x2 = 10; y2 = 90; w2 = 100; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x2,y2,w2,h2,"Caption:");
	x2 = 100; y2 = 90; w2 = 150; h2 = 20;
	ZGuiWnd* pkCaptionField = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_CTRLCAPTION_EB,"CtrlPropCaptionEB",x2,y2,w2,h2,false);
	pkCaptionField->SetWindowFlag(WF_CANHAVEFOCUS);

	// Tab Order Number
	x2 = 10; y2 = 120; w2 = 100; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x2,y2,w2,h2,"Tab order:");
	x2 = 100; y2 = 120; w2 = 150; h2 = 20;
	ZGuiWnd* pkTabOrderField = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_CTRLTABORDERNR_EB,"CtrlTabOrderEB",x2,y2,w2,h2,false);
	pkTabOrderField->SetWindowFlag(WF_CANHAVEFOCUS);

	// Pos x
	x2 = 10; y2 = 150; w2 = 10; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x2,y2,w2,h2,"x:");
	x2 = 30; y2 = 150; w2 = 32; h2 = 20;
	ZGuiWnd* pkPosX = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_CTRLPOSX_EB,"CtrlPosXEB",x2,y2,w2,h2,false);
	pkPosX->SetWindowFlag(WF_CANHAVEFOCUS);

	// Pos y
	x2 = 70; y2 = 150; w2 = 10; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x2,y2,w2,h2,"y:");
	x2 = 90; y2 = 150; w2 = 32; h2 = 20;
	ZGuiWnd* pkPosY = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_CTRLPOSY_EB,"CtrlPosYEB",x2,y2,w2,h2,false);
	pkPosX->SetWindowFlag(WF_CANHAVEFOCUS);

	// Width
	x2 = 132; y2 = 150; w2 = 10; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x2,y2,w2,h2,"w:");
	x2 = 152; y2 = 150; w2 = 32; h2 = 20;
	ZGuiWnd* pkWidth = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_CTRLWIDTH_EB,"CtrlWidthEB",x2,y2,w2,h2,false);
	pkPosX->SetWindowFlag(WF_CANHAVEFOCUS);

	// Height
	x2 = 192; y2 = 150; w2 = 10; h2 = 20;
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x2,y2,w2,h2,"h:");
	x2 = 218; y2 = 150; w2 = 32; h2 = 20;
	ZGuiWnd* pkHeight = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_CTRLHEIGHT_EB,"CtrlHeightEB",x2,y2,w2,h2,false);
	pkPosX->SetWindowFlag(WF_CANHAVEFOCUS);

	// Disabled
	x2 = 10; y2 = 180; w2 = 16; h2 = 16;
	ZGuiWnd* pkEnabled = m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox, 
		ID_CRTLDISABLED_CB, "CtrlDisabledCH", x2, y2, w2, h2, "Disabled", false);
	pkPosX->SetWindowFlag(WF_CANHAVEFOCUS);

	// Create special state controls
	CreateUniqueProperites();

	// OK button
	ZGuiWnd* pkOKButton = m_pkGuiBuilder->CreateOKButton(m_pkDlgBox,
		ID_CTRLOK_BN,NULL,true);
	pkOKButton->SetWindowFlag(WF_CANHAVEFOCUS);
	// Cancel button
	ZGuiWnd* pkCancelButton = m_pkGuiBuilder->CreateCancelButton(m_pkDlgBox,
		ID_CTRLCANCEL_BN,NULL,true);
	pkOKButton->SetWindowFlag(WF_CANHAVEFOCUS);
	// Close button
	ZGuiWnd* pkCloseButton = m_pkGuiBuilder->CreateCloseButton(m_pkDlgBox,
		ID_CTRLCLOSE_BN,NULL);

	// Bind return key to textboxes-ok button
	m_pkGui->AddKeyCommand(KEY_RETURN,pkIDField,pkOKButton);
	m_pkGui->AddKeyCommand(KEY_RETURN,pkCaptionField,pkOKButton);

	m_pkGui->ShowMainWindow(m_pkGuiBuilder->GetWnd("CtrlPropBoxWnd"),false);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: OnOpen
// Description: 
//
bool PropertyBox::OnOpen(int x,int y)
{
	if(SelectWnd::GetInstance()->m_pkWnd == NULL)
		return false;

	if(m_pkDlgBox->IsVisible() == false)
	{
		if(!(x==-1 && y==-1))
			m_pkDlgBox->SetPos(x,y,true,true);

		m_pkDlgBox->SetMoveArea(m_pkGuiBuilder->m_rcScreen);
	}

	Update(SelectWnd::GetInstance()->m_pkWnd);

	m_pkGui->ShowMainWindow(m_pkGuiBuilder->GetWnd("CtrlPropBoxWnd"),true);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: OnClose
// Description: 
//
bool PropertyBox::OnClose(bool bSave)
{
	if(SelectWnd::GetInstance()->m_pkWnd && bSave)
	{
		SelectWnd::GetInstance()->m_pkWnd->SetText(
			m_pkGuiBuilder->GetWnd("CtrlPropCaptionEB")->GetText());
		SelectWnd::GetInstance()->m_pkWnd->SetID(atoi(
			m_pkGuiBuilder->GetWnd("CtrlPropIDEB")->GetText()));
		SelectWnd::GetInstance()->m_pkWnd->SetTabOrderNr(atoi(
			m_pkGuiBuilder->GetWnd("CtrlTabOrderEB")->GetText()));

		switch(m_pkGuiBuilder->GetWndType(
			SelectWnd::GetInstance()->m_pkWnd))
		{
			case COMBOBOX:
				((ZGuiCombobox*)SelectWnd::GetInstance()->m_pkWnd)->
					SetNumVisibleRows(atoi(m_pkGuiBuilder->
						GetWnd("NumVisibleRowsEB")->GetText()));
				break;
		}
	}

	m_pkGui->ShowMainWindow(m_pkGuiBuilder->GetWnd("CtrlPropBoxWnd"),false);
	m_pkGui->SetFocus(SelectWnd::GetInstance()->m_pkWnd);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: Update
// Description: 
//
void PropertyBox::Update(ZGuiWnd* pkWnd)
{
	if(SelectWnd::GetInstance()->m_pkWnd)
	{
		Rect rc = pkWnd->GetScreenRect();

		m_pkGuiBuilder->SetTextString("PropertyBoxLabel",
			m_pkGuiBuilder->GetTypeName(pkWnd));

		m_pkGuiBuilder->SetTextInt("CtrlPosXEB",rc.Left);
		m_pkGuiBuilder->SetTextInt("CtrlPosYEB",rc.Top);
		m_pkGuiBuilder->SetTextInt("CtrlWidthEB",rc.Right-rc.Left);
		m_pkGuiBuilder->SetTextInt("CtrlHeightEB",rc.Bottom-rc.Top);

		m_pkGuiBuilder->SetTextInt("CtrlTabOrderEB",pkWnd->GetTabOrderNr());
		m_pkGuiBuilder->SetTextInt("CtrlPropIDEB",pkWnd->GetID());
		m_pkGuiBuilder->SetTextString("CtrlPropCaptionEB",pkWnd->GetText());
		m_pkGuiBuilder->SetTextString("CtrlPropIDName",(char*)pkWnd->GetName());

		if(SelectWnd::GetInstance()->m_pkWnd->Enabled())
			((ZGuiCheckbox*)m_pkGuiBuilder->GetWnd("CtrlDisabledCH"))->UncheckButton();
		else
			((ZGuiCheckbox*)m_pkGuiBuilder->GetWnd("CtrlDisabledCH"))->CheckButton();

		// Show/Hide and update special property values

		const CtrlType c_iSelWndType = m_pkGuiBuilder->GetWndType(
			SelectWnd::GetInstance()->m_pkWnd);

		vector<pair<CtrlType, ZGuiWnd*> >::iterator it;
		for(it = m_kSpecCtrls.begin(); it != m_kSpecCtrls.end(); it++)
		{
			if(it->first == c_iSelWndType)
			{
				UpdateUniquePropertyText(it->second, c_iSelWndType);
				it->second->Show();
			}
			else
				it->second->Hide();
		}		
	}
}

bool PropertyBox::IsOpen()
{
	return m_pkDlgBox->IsVisible();
}

bool PropertyBox::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
						   int iNumberOfParams,void *pkParams ) 
{
	switch(uiMessage)
	{
	// Command Messages
	case ZGM_COMMAND:
		switch(((int*)pkParams)[0]) // control id
		{
		case ID_CTRLOK_BN:
			if(SelectWnd::GetInstance()->m_pkWnd)
			{
				char* szNewRegName;
				szNewRegName = m_pkGuiBuilder->GetWnd("CtrlPropIDName")->GetText();

				if(m_pkGuiBuilder->RenameWnd(
					SelectWnd::GetInstance()->m_pkWnd,
					szNewRegName))
				{
					OnClose(true);
				}
				else
				{
					m_pkGuiBuilder->SetTextString("CtrlPropIDName",
						"bad name!");
					return false;
				}
			}
			break;
		case ID_CTRLCANCEL_BN:
		case ID_CTRLCLOSE_BN:
			OnClose(false);
			break;
		case ID_CRTLVERTHORZSCROLLBAR_CB:
			((ZGuiScrollbar*)SelectWnd::GetInstance()->m_pkWnd)->ToogleHorizontal();
			break;
		case ID_CRTLMOVEABLEWND_CB:
			printf("hhheee\n");
			if(m_pkGuiBuilder->IsButtonChecked("ToogleMoveableWndCB"))
				SelectWnd::GetInstance()->m_pkWnd->SetMoveArea(Rect(0,0,1024,768));
			else
				SelectWnd::GetInstance()->m_pkWnd->SetMoveArea(
					SelectWnd::GetInstance()->m_pkWnd->GetScreenRect());
			break;
		case ID_CRTLDISABLED_CB:
			if(m_pkGuiBuilder->IsButtonChecked("CtrlDisabledCH"))
				SelectWnd::GetInstance()->m_pkWnd->Disable();
			else
				SelectWnd::GetInstance()->m_pkWnd->Enable();
			break;
		}
		break;

	// Editbox Typing Message
	case ZGM_EN_CHANGE:
		
		switch(((int*)pkParams)[0]) // id of the textbox
		{
		case ID_CTRLPOSX_EB:
			if(SelectWnd::GetInstance()->m_pkWnd)
			{
				Rect rc = SelectWnd::GetInstance()->m_pkWnd->GetScreenRect();
				rc.Left = m_pkGuiBuilder->GetTextInt("CtrlPosXEB");
				SelectWnd::GetInstance()->m_pkWnd->SetPos(rc.Left, rc.Top, 1, 1);
			}
			break;
		case ID_CTRLPOSY_EB:
			if(SelectWnd::GetInstance()->m_pkWnd)
			{
				Rect rc = SelectWnd::GetInstance()->m_pkWnd->GetScreenRect();
				rc.Top = m_pkGuiBuilder->GetTextInt("CtrlPosYEB");
				SelectWnd::GetInstance()->m_pkWnd->SetPos(rc.Left, rc.Top, 1, 1);
			}
			break;
		case ID_CTRLWIDTH_EB:
			if(SelectWnd::GetInstance()->m_pkWnd)
			{
				int w = m_pkGuiBuilder->GetTextInt("CtrlWidthEB");
				int h = SelectWnd::GetInstance()->m_pkWnd->GetScreenRect().Bottom-
					SelectWnd::GetInstance()->m_pkWnd->GetScreenRect().Top;
				SelectWnd::GetInstance()->m_pkWnd->Resize(w, h);
			}
			break;
		case ID_CTRLHEIGHT_EB:
			if(SelectWnd::GetInstance()->m_pkWnd)
			{
				int w = SelectWnd::GetInstance()->m_pkWnd->GetScreenRect().Right-
					SelectWnd::GetInstance()->m_pkWnd->GetScreenRect().Left;
				int h = m_pkGuiBuilder->GetTextInt("CtrlHeightEB");
				SelectWnd::GetInstance()->m_pkWnd->Resize(w, h);
			}
			break;
		}
		break;

	default:
		return m_oMainWndProc(pkWnd,uiMessage,iNumberOfParams,pkParams);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateUniqueProperites
// Description: Create controls that are unique for each control and that are
//				hidden from the beginning and showed when the Dialog are opened.
//
void PropertyBox::CreateUniqueProperites()
{
	ZGuiTextbox* pkTextbox;
	ZGuiCheckbox* pkCheckbox;
	ZGuiLabel* pkLabel;

	int x = 260, y = 30, w = 16, h = 16;

	// Main windows: Moveable on/off
	pkCheckbox = m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox, 
		ID_CRTLMOVEABLEWND_CB, "ToogleMoveableWndCB", x, y, w, h,
		"Moveable", false);
	pkCheckbox->Hide();
	m_kSpecCtrls.push_back(pair<CtrlType, ZGuiWnd*>(WINDOW,pkCheckbox));

	// Scrollbar: Vertical/Horizontal scrollbar checkbox
	pkCheckbox = m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox, 
		ID_CRTLVERTHORZSCROLLBAR_CB, "ToogleScrollbarTypeCB", x, y, w, h,
		"Horizontal", false);
	pkCheckbox->Hide();
	m_kSpecCtrls.push_back(pair<CtrlType, ZGuiWnd*>(SCROLLBAR,pkCheckbox));

	// Combox: Menu/Normal
	pkCheckbox = m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox, 
		ID_CRTLMENUNORMAL_CB, "ToogleComboboxTypeCB", x, y, w, h,
		"Menu", false);
	pkCheckbox->Hide();
	m_kSpecCtrls.push_back(pair<CtrlType, ZGuiWnd*>(COMBOBOX,pkCheckbox));

	// Textbox: Multiline/Singelline
	pkCheckbox = m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox, 
		ID_CRTLMULTILINETEXTBOX_CB, "ToogleTextboxMultilineCB", x, y, w, h,
		"Multi-line", false);
	pkCheckbox->Hide();
	m_kSpecCtrls.push_back(pair<CtrlType, ZGuiWnd*>(TEXTBOX,pkCheckbox));

	// Radiobutton: GroupID
	w = 40; h = 20;
	pkLabel = m_pkGuiBuilder->CreateLabel(m_pkDlgBox,ID_CTRLGROUPIDRADIOBN_LAB,
		"RadioBnGroupIDLB",x,y,80,h,"Group ID:");
	pkLabel->Hide();
	m_kSpecCtrls.push_back(pair<CtrlType, ZGuiWnd*>(RADIOBUTTON,pkLabel));
	pkTextbox = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, 
		ID_CTRLGROUPIDRADIOBN_EB,"RadioBnGroupIDEB",x+90,y,w,h,false);
	pkTextbox->SetWindowFlag(WF_CANHAVEFOCUS);
	pkTextbox->Hide();
	m_kSpecCtrls.push_back(pair<CtrlType, ZGuiWnd*>(RADIOBUTTON,pkTextbox));

	// Combobox: Number of visible rows
	y += 30; w = 40; h = 20;
	pkLabel = m_pkGuiBuilder->CreateLabel(m_pkDlgBox,ID_CTRLNUMVISROWSCOMBOBOX_LAB,
		"NumVisibleRowsLB",x,y,80,h,"Visible rows:");
	pkLabel->Hide();
	m_kSpecCtrls.push_back(pair<CtrlType, ZGuiWnd*>(COMBOBOX,pkLabel));
	pkTextbox = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_CTRLNUMVISROWSCOMBOBOX_EB,"NumVisibleRowsEB",x+90,y,w,h,false);
	pkTextbox->SetWindowFlag(WF_CANHAVEFOCUS);
	pkTextbox->SetText("4");
	pkTextbox->Hide();
	m_kSpecCtrls.push_back(pair<CtrlType, ZGuiWnd*>(COMBOBOX,pkTextbox));
}

void PropertyBox::UpdateUniquePropertyText(ZGuiWnd *pkControl, CtrlType c_iSelWndType)
{
	switch(c_iSelWndType)
	{
	case WINDOW:
		if(pkControl == m_pkGuiBuilder->GetWnd("ToogleMoveableWndCB"))
		{
			if( SelectWnd::GetInstance()->m_pkWnd->GetMoveArea() ==
				SelectWnd::GetInstance()->m_pkWnd->GetScreenRect() )
			{
				((ZGuiCheckbox*) pkControl)->UncheckButton();
			}
			else
			{
				((ZGuiCheckbox*) pkControl)->CheckButton();
			}
		}
		break;

	case SCROLLBAR:
		if(pkControl == m_pkGuiBuilder->GetWnd("ToogleScrollbarTypeCB"))
		{
			bool bHorzintal = SelectWnd::GetInstance()->m_pkWnd->
					GetWndRect().Width() > 
				SelectWnd::GetInstance()->m_pkWnd->
					GetWndRect().Height() ? true : false;

			if( bHorzintal )
			{
				((ZGuiCheckbox*) pkControl)->UncheckButton();
			}
			else
			{
				((ZGuiCheckbox*) pkControl)->CheckButton();
			}
		}
		break;
	}
}

