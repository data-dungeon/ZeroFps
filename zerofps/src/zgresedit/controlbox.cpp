// Controlbox.cpp: implementation of the ControlBox class.
//
///////////////////////////////////////////////////////////////////////////////

#include "controlbox.h"
#include "selectwnd.h"
#include "serialization.h"
#include "../zerofps/render/texturemanager.h"
#include <typeinfo>
#include "../zerofps/basic/zfini.h"
#include "../zerofps/basic/zfassert.h"
#include <set>

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//
ControlBox::ControlBox(GuiBuilder* pkGuiBuilder, ZGuiWndProc oMainWndProc) 
	: DlgBox(pkGuiBuilder, oMainWndProc)
{

}

ControlBox::~ControlBox()
{

}

///////////////////////////////////////////////////////////////////////////////
// Name: Create
// Description: 
//
bool ControlBox::Create(int x,int y,int w,int h,ZGuiWndProc pkWndProc)
{
	m_pkDlgBox = m_pkGuiBuilder->CreateMainWindow(ID_CTRLBOX_MAIN_WND, 
		ID_CTRLBOX_WND, "CtrlBoxWnd", x,y,w,h,pkWndProc);
		
	ZGuiWnd* pkResMenuBn;

	// Create controller type radio butons.
	typedef pair<string,string> name_label;
	typedef pair<int,name_label> controller;
	int id_start = ID_CTRL_WINDOW_BN;
	controller akCtrls[] = 
	{
		controller(id_start++,name_label("SelWndCtrlRb","Window")),					// 0
		controller(id_start++,name_label("SelLabelCtrlRb","Label")),				// 1
		controller(id_start++,name_label("SelTextboxCtrlRb","Textbox")),			// 2
		controller(id_start++,name_label("SelButtonCtrlRb","Button")),				// 3
		controller(id_start++,name_label("SelCheckboxCtrlRb","Check Box")),			// 4
		controller(id_start++,name_label("SelRadioButtonCtrlRb","Radio Button")),	// 5
		controller(id_start++,name_label("SelRadioButtonNewGroupCtrlRb",
			"Radio Button (g)")),// 5
		controller(id_start++,name_label("SelComboboxCtrlRb","Combo Box")),			// 6
		controller(id_start++,name_label("SelListboxCtrlRb","List Box")),			// 7
		controller(id_start++,name_label("SelScrollbarCtrlRb","Scroll Bar")),		// 8
	};

	w = 16; h = 16; x = 10; y = 10; 
	for(int i=0; i<sizeof(akCtrls) / sizeof(akCtrls[1]); i++)
	{
		pkResMenuBn = m_pkGuiBuilder->CreateRadioButton(m_pkDlgBox,
			akCtrls[i].first,(char*) akCtrls[i].second.first.c_str(),
			CTRLMENU_RADIO_GROUP,"CTRLMENU_RADIO_GROUP",false,
			x,y,w,h,(char*)akCtrls[i].second.second.c_str());

		((ZGuiRadiobutton*)pkResMenuBn)->SetButtonUnselectedSkin(
			m_pkGuiBuilder->GetSkin("RbSqrSqrUp"));
		((ZGuiRadiobutton*)pkResMenuBn)->SetButtonSelectedSkin(
			m_pkGuiBuilder->GetSkin("RbSqrSqrDown"));

		pkResMenuBn->SetWindowFlag(WF_CANHAVEFOCUS);
		y += 20;
	}

	// Create open skins button.
	w = 60; h = 20; x = 10; y += 5;
	ZGuiWnd* pkOpenSkinsBn = m_pkGuiBuilder->CreateButton(m_pkDlgBox,
		ID_CTRLBOX_OPENSKINDLG_BN,NULL,x,y,w,h," Skins...");
	pkOpenSkinsBn->SetWindowFlag(WF_CENTER_TEXT | WF_CANHAVEFOCUS);

	// Create clone button.
	w = 60; h = 20; x = 80;
	ZGuiWnd* pkCloneBn = m_pkGuiBuilder->CreateButton(m_pkDlgBox,
		ID_CTRLBOX_COPY_BN,NULL,x,y,w,h," Copy");
	pkOpenSkinsBn->SetWindowFlag(WF_CENTER_TEXT | WF_CANHAVEFOCUS);

	// Create grid checkbox
	w = 16; h = 16; x = 10; y += 25;
	ZGuiCheckbox* pkGridCB = m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox, 
		ID_GRID_CB, "GridCB", x, y, w, h, "Use grid");

	// Create grid precision editbox
	w = 32; h = 16; x = 110;
	ZGuiTextbox* pkGridPrec = m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,
		ID_GRID_PRECISION, "GridPrecEB", x, y, w, h, false);
	pkGridPrec->SetText("5");

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: OnOpen
// Description: 
//
bool ControlBox::OnOpen(int x,int y)
{
	m_pkDlgBox->Show();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: OnClose
// Description: 
//
bool ControlBox::OnClose(bool bSave)
{
	m_pkDlgBox->Hide();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: CreateNewType
// Description: 
//
bool ControlBox::CreateNewType(CtrlType eType, ZGuiWndProc oWndProc)
{
	int x = 10, y = 10, w, h;
	ZGuiWnd* pkParent = SelectWnd::GetInstance()->m_pkWnd;

	// Ändra så att vi markerar ett main window fönster...
	if(eType != WINDOW && pkParent)
	{
		ZGuiWnd* pkMain = SelectWnd::GetInstance()->m_pkWnd->GetParent(true);
		if(pkMain != NULL)
		{
			pkParent = pkMain;

			// Kolla positionen på den markerade kontrollen...
			if(SelectWnd::GetInstance()->m_pkWnd)
			{
				x = SelectWnd::GetInstance()->m_pkWnd->GetWndRect().Left;
				y = SelectWnd::GetInstance()->m_pkWnd->GetWndRect().Bottom;
			}
		}
	}
	else
	if(eType != WINDOW)
	{
		return false;
	}

	ZGuiWnd* pkWin;

	// Skapa en nytt skin till fönstret.
	m_pkGuiBuilder->m_bAllocateNewSkins = true;

	switch(eType)
	{
	case WINDOW:
		{
			int w = 500, h = 500;
			int x = m_pkGuiBuilder->m_rcScreen.Right/2-w/2;
			int y = m_pkGuiBuilder->m_rcScreen.Bottom/2-h/2;
			pkWin=m_pkGuiBuilder->CreateMainWindow(-1,-1,NULL,x,y,w,h,oWndProc);
		}
		break;
	case LABEL:
		pkWin=m_pkGuiBuilder->CreateLabel(pkParent,-1,NULL,x,y,100,20,"Label");
		break;
	case TEXTBOX:
		pkWin=m_pkGuiBuilder->CreateTextbox(pkParent,-1,NULL,x,y,w=100,h=100,false);	
		break;
	case BUTTON:
		pkWin=m_pkGuiBuilder->CreateButton(pkParent,-1,NULL,x,y,w=50,h=20,"Button");	
		break;
	case CHECKBOX:
		pkWin=m_pkGuiBuilder->CreateCheckbox(pkParent,-1,NULL,x,y,w=16,h=16,
			"Check Box");	
		break;
	case RADIOBUTTON:
		pkWin=m_pkGuiBuilder->CreateRadioButton(pkParent,-1,NULL,-1,NULL,false,
			x,y,w=16,h=16,"Radio Button");	
		break;
	case RADIOBUTTON_NEW_GROUP:
		pkWin=m_pkGuiBuilder->CreateRadioButton(pkParent,-1,NULL,-1,NULL,true,
			x,y,w=16,h=16,"Radio Button");	
		break;
	case LISTBOX:
		pkWin=m_pkGuiBuilder->CreateListbox(pkParent,-1,NULL,x,y,w=100,h=100);
		break;
	case COMBOBOX:
		pkWin=m_pkGuiBuilder->CreateCombobox(pkParent,-1,NULL,x,y,w=100,h=20,false);
		break;
	case SCROLLBAR:
		pkWin=m_pkGuiBuilder->CreateScrollbar(pkParent,-1,NULL,x,y,w=20,h=100);
		break;
	}

	m_pkGuiBuilder->m_bAllocateNewSkins = false;

	if(pkParent)
	{
		if(y+h > pkParent->GetScreenRect().Height())
		{
			int y_offset = pkParent->GetScreenRect().Height()-h;
			pkWin->Move(w, -y_offset);
		}

		if(x+w > pkParent->GetScreenRect().Width())
		{
			int x_offset = pkParent->GetScreenRect().Width()-w;
			pkWin->Move(-x_offset, 0);
		}
	}

	// Markera det nya fönstret.
	pkWin->SetWindowFlag(WF_CANHAVEFOCUS); 
	m_pkGui->SetFocus(pkWin);
	SelectWnd::GetInstance()->m_pkWnd = pkWin;

	m_pkCreatedWindows.push_back(CreateWnd(pkWin));

	return true;
}

void ControlBox::CreateCopy()
{
	if(SelectWnd::GetInstance()->m_pkWnd)
	{
		// Must register selected window because CreateNewType change it
		// and we need to look at it later.
		ZGuiWnd* pkPrevSelWnd = SelectWnd::GetInstance()->m_pkWnd;

		CreateNewType(m_pkGuiBuilder->GetWndType(
			SelectWnd::GetInstance()->m_pkWnd),
			(ZGuiWnd::callbackfunc)
			m_pkGui->GetSpecialWndData(
			SelectWnd::GetInstance()->m_pkWnd, 
			ZGui::ZndInfo::WNDPROC));

		ZGuiWnd* pkNewWnd = m_pkCreatedWindows.back().m_pkWnd; // id = last-1

		int id = pkNewWnd->GetID();
		char name[50]; 

		// copy data
		pkNewWnd->CopyNonUniqueData(pkPrevSelWnd);

		// change some values
		pkNewWnd->SetID(id);

		sprintf(name, "%s%i", m_pkGuiBuilder->GetTypeName(pkNewWnd), id);
		m_pkGui->ChangeWndRegName(pkNewWnd, name);

		pkNewWnd->Move(0,pkPrevSelWnd->GetScreenRect().Height());
	}
}

bool ControlBox::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
					      int iNumberOfParams,void *pkParams ) 
{
	switch(uiMessage)
	{
	// Command Messages
	case ZGM_COMMAND:
		switch(((int*)pkParams)[0]) // control id
		{
		case ID_CTRL_WINDOW_BN:
		case ID_CTRL_WINDOW_BN+1:	case ID_CTRL_WINDOW_BN+2:
		case ID_CTRL_WINDOW_BN+3:	case ID_CTRL_WINDOW_BN+4:
		case ID_CTRL_WINDOW_BN+5:	case ID_CTRL_WINDOW_BN+6:
		case ID_CTRL_WINDOW_BN+7:	case ID_CTRL_WINDOW_BN+8:
		case ID_CTRL_WINDOW_BN+9:
			CreateNewType(CtrlType(
				((int*)pkParams)[0]-ID_CTRL_WINDOW_BN),m_oMainWndProc);
			break;

		// let the main proc process this messages
		case ID_GRID_CB:
		case ID_CTRLBOX_OPENSKINDLG_BN: 
			return m_oMainWndProc(pkWnd,uiMessage,iNumberOfParams,pkParams);
			break;

		case ID_CTRLBOX_COPY_BN: 
			CreateCopy();
			// let the main proc update the property box
			return m_oMainWndProc(pkWnd,uiMessage,iNumberOfParams,pkParams);
			break;
		}
		break;
	default:
		return m_oMainWndProc(pkWnd,uiMessage,iNumberOfParams,pkParams);
	}

	return true;
}

void ControlBox::UnregisterNewType(ZGuiWnd *pkWnd)
{
	for(vector<CreateWnd>::iterator it = m_pkCreatedWindows.begin(); 
		it != m_pkCreatedWindows.end(); it++)
		{
			if(pkWnd == (*it).m_pkWnd)
			{
				m_pkCreatedWindows.erase(it);
				break;
			}
		}
}

void ControlBox::PrintWindowIDs(Serialization *pkFile)
{
	unsigned int i;

	for( i=0; i<m_pkCreatedWindows.size(); i++)
		pkFile->Outputa( "\t%s = %i,\n", m_pkCreatedWindows[i].m_pkWnd->GetName(), 
			m_pkCreatedWindows[i].m_pkWnd->GetID() );

	// Printa också alla radio groups
	set<int> kPrintedIDs;

	// Lämna en tomrad mellan
	pkFile->Outputa("\n");

	for( i=0; i<m_pkCreatedWindows.size(); i++)
	{
		if(m_pkGuiBuilder->GetWndType(m_pkCreatedWindows[i].m_pkWnd) == 
			RADIOBUTTON)
		{
			char* group_id_name = 
				((ZGuiRadiobutton*)m_pkCreatedWindows[i].m_pkWnd)->GetGroupName();
			int group_id = 
				((ZGuiRadiobutton*)m_pkCreatedWindows[i].m_pkWnd)->GetGroupID();

			// Finns redan?
			if( kPrintedIDs.find(group_id) != kPrintedIDs.end() )
				continue;

			kPrintedIDs.insert(group_id);
			pkFile->Outputa( "\t%s = %i,\n", group_id_name, group_id );
		}
	}
}

static bool SortPrintWnd(const ControlBox::CreateWnd& w1, const ControlBox::CreateWnd& w2)
{
	const type_info& t1 = typeid(*w1.m_pkWnd);
	const type_info& t2 = typeid(*w2.m_pkWnd);

	if(t1 == typeid(ZGuiWnd) && t2 != typeid(ZGuiWnd))
		return true;
	else
		return false;
}

void ControlBox::PrintWindowRC(Serialization *pkFile, TextureManager* pkTexMan)
{
	ZGuiWnd* pkWnd, *pkParent;

	printf("apa fore\n");

	// Print all skins.
	PrintSkins(pkFile, pkTexMan);

	printf("apa efter\n");

	unsigned int number = m_pkCreatedWindows.size();
	unsigned int aiCounter[11] = {1,1,1,1,1,1,1,1,1,1,1};

	sort(m_pkCreatedWindows.begin(), m_pkCreatedWindows.end(), SortPrintWnd);

	for(unsigned int i=0; i<number; i++)
	{
		pkWnd = m_pkCreatedWindows[i].m_pkWnd;

		// Section
		pkFile->Outputa("[%s%i]\n", m_pkGuiBuilder->GetTypeName(pkWnd), 
			aiCounter[m_pkGuiBuilder->GetWndType(pkWnd)]++); 

		// Type
		pkFile->Outputa("type=%s\n", m_pkGuiBuilder->GetTypeName(pkWnd));

		// ID
		pkFile->Outputa("id=%i\n", pkWnd->GetID());

		// Name
		pkFile->Outputa("name=%s\n", pkWnd->GetName());

		// Parent ID, Parent name
		pkParent = pkWnd->GetParent();
		
		if(pkParent)
		{
			pkFile->Outputa("parent_id=%i\nparent_name=%s\n",
				pkParent->GetID(), pkParent->GetName());
		}
		else
		{
			pkFile->Outputa("parent_id=-1\nparent_name=NULL\n");
		}

		// Text
		pkFile->Outputa("text=%s\n", pkWnd->GetText()); 

		// Visible, Enabled
		pkFile->Outputa("visible=%i\nenabled=%i\n", 
			(int) pkWnd->IsVisible(), (int) pkWnd->Enabled());

		// Area, MoveArea
		Rect rc = pkWnd->GetWndRect(), rc_m = pkWnd->GetMoveArea();
		pkFile->Outputa("area_left=%i\narea_top=%i\narea_right=%i\narea_bottom=%i\n",
			rc.Left, rc.Top, rc.Right, rc.Bottom);

		pkFile->Outputa("move_area_left=%i\nmove_area_top=%i\n" \
			"move_area_right=%i\nmove_area_bottom=%i\n",
			rc_m.Left, rc_m.Top, rc_m.Right, rc_m.Bottom); 

		// Tab order
		pkFile->Outputa("tab_order=%i\n", pkWnd->GetTabOrderNr() ); 

		// Unique properties
		PrintSpecialProperites(pkFile, pkWnd);

		if(i < number-1)
			pkFile->Outputa("\n");
	}
}

void ControlBox::PrintSkins(Serialization *pkFile, TextureManager* pkTexMan)
{
	unsigned int uiSkinCounter = 1;

	for(unsigned int i=0; i<m_pkCreatedWindows.size(); i++)
	{
		ZGuiWnd* pkWnd = m_pkCreatedWindows[i].m_pkWnd;

		// Get all skins used by this control.
		vector<ZGuiWnd::SKIN_DESC> kWndSkins;
		pkWnd->GetWndSkinsDesc(kWndSkins);

		for(unsigned int j=0; j<kWndSkins.size(); j++)
		{
			ZGuiSkin* pkSkin = kWndSkins[j].first;

			if(pkSkin == NULL)
				continue;

			
			ZFAssert(pkSkin, "No skin!");
				


			// Section
			pkFile->Outputa("[Skin%i]\n", uiSkinCounter++);

			// Name
			pkFile->Outputa("window_name=%s\n", pkWnd->GetName());

			// Window description
			pkFile->Outputa("wnd_desc=%s\n", kWndSkins[j].second);

			char* aszSkinNames[] =
			{
				"bk_tex",
				"horz_bd_tex",
				"vert_bd_tex",
				"corner_bd_tex",
				"bk_a_tex",
				"horz_bd_a_tex",
				"vert_bd_a_tex",
				"corner_bd_a_tex",
			};

			int aTexIDs[8] = 
			{
				pkSkin->m_iBkTexID,
				pkSkin->m_iHorzBorderTexID,
				pkSkin->m_iVertBorderTexID,
				pkSkin->m_iBorderCornerTexID,
				pkSkin->m_iBkTexAlphaID,
				pkSkin->m_iHorzBorderTexAlphaID,
				pkSkin->m_iVertBorderTexAlphaID,
				pkSkin->m_iBorderCornerTexAlphaID,
			};

			// Textures
			for(int k=0; k<8; k++)
				pkFile->Outputa("%s=%s\n",aszSkinNames[k],pkTexMan->GetFileName(aTexIDs[k]));

			// Background color
			pkFile->Outputa("bk_r=%i\nbk_g=%i\nbk_b=%i\n", (int)(pkSkin->m_afBkColor[0]*255),
				(int)(pkSkin->m_afBkColor[1]*255),
				(int)(pkSkin->m_afBkColor[2]*255));
			
			// Border color
			pkFile->Outputa("bd_r=%i\nbd_g=%i\nbd_b=%i\n", (int)(pkSkin->m_afBorderColor[0]*255),
				(int)(pkSkin->m_afBorderColor[1]*255),
				(int)(pkSkin->m_afBorderColor[2]*255));

			// Bordersize, tile skin, transparency
			pkFile->Outputa("bd_size=%i\ntile_skin=%i\ntransparent=%i\n", (int)(pkSkin->m_unBorderSize),
				(int)(pkSkin->m_bTileBkSkin),
				(int)(pkSkin->m_bTransparent));

			pkFile->Outputa("\n");
		}
	}
}

void ControlBox::PrintSpecialProperites(Serialization *pkFile, ZGuiWnd *pkWnd)
{
	switch(m_pkGuiBuilder->GetWndType(pkWnd))
	{
	case WINDOW:
		pkFile->Outputa("mainwnd_id=%i\n", 
			m_pkGui->GetMainWindowID((char*)pkWnd->GetName()));
		break;
	case LABEL:
		break;
	case TEXTBOX:
		break;
	case BUTTON:
		break;
	case CHECKBOX:
		pkFile->Outputa("checked=%i\n", (int)((ZGuiCheckbox*)(pkWnd))->IsChecked());
		break;
	case RADIOBUTTON:
		pkFile->Outputa("radio_group=%i\n", 
			((ZGuiRadiobutton*)(pkWnd))->GetGroupID());
		pkFile->Outputa("radio_group_name=%s\n", 
			((ZGuiRadiobutton*)(pkWnd))->GetGroupName());
		break;
	case RADIOBUTTON_NEW_GROUP:
		break;
	case COMBOBOX:
		pkFile->Outputa("is_menu=%i\n", (int)((ZGuiCombobox*)(pkWnd))->IsMenu());
		pkFile->Outputa("num_visible_rows=%i\n",
			(int)((ZGuiCombobox*)(pkWnd))->GetNumVisibleRows());
		break;
	case LISTBOX:
		break;
	case SCROLLBAR:
		break;
	}
}

bool ControlBox::LoadGUI(ZFIni *pkINI, TextureManager* pkTexMan)
{
	ClearAll();

	vector<string> vkSections;
	pkINI->GetSectionNames(vkSections);

	vector<tSkinInf> kAllSkinsTempArray;

	// Load all skins to a temp array.
	for(unsigned int i=0; i<vkSections.size(); i++)
	{
		tSkinInf kNewSkinInfo;

		char* szWindowName = pkINI->GetValue(
			(char*)vkSections[i].c_str(),"window_name");

		// We have found the last skin. 
		if(szWindowName == NULL)
			break;

		kNewSkinInfo.first.first=szWindowName;
		kNewSkinInfo.first.second=
			pkINI->GetValue((char*)vkSections[i].c_str(),"wnd_desc");
		
		ZGuiSkin* pkSkin = new ZGuiSkin;

		char* tex[8] =
		{
			pkINI->GetValue((char*)vkSections[i].c_str(), "bk_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "horz_bd_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "vert_bd_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "corner_bd_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "bk_a_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "horz_bd_a_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "vert_bd_a_tex"),
			pkINI->GetValue((char*)vkSections[i].c_str(), "corner_bd_a_tex"),
		};

		pkSkin->m_iBkTexID = strcmp(tex[0], "(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[0],0);
		pkSkin->m_iHorzBorderTexID = strcmp(tex[1], "(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[1],0);
		pkSkin->m_iVertBorderTexID = strcmp(tex[2], "(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[2],0);
		pkSkin->m_iBorderCornerTexID = strcmp(tex[3], "(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[3],0);
		pkSkin->m_iBkTexAlphaID = strcmp(tex[4], "(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[4],0);
		pkSkin->m_iHorzBorderTexAlphaID = strcmp(tex[5], "(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[5],0);
		pkSkin->m_iVertBorderTexAlphaID = strcmp(tex[6], "(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[6],0);
		pkSkin->m_iBorderCornerTexAlphaID = strcmp(tex[7], "(null)")==0 ? - 1 : 
			pkTexMan->Load(tex[7],0);

		int bk_r  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bk_r"));
		int bk_g  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bk_g"));
		int bk_b  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bk_b"));
		int bd_r  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bd_r"));
		int bd_g  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bd_g"));
		int bd_b  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bd_b"));
		int bd_sz = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "bd_size"));
		int tile  = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "tile_skin"));
		int trans = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "transparent"));

		pkSkin->m_afBkColor[0] = (1.0f/255)*bk_r;
		pkSkin->m_afBkColor[1] = (1.0f/255)*bk_g;
		pkSkin->m_afBkColor[2] = (1.0f/255)*bk_b;

		pkSkin->m_afBorderColor[0] = (1.0f/255)*bd_r;
		pkSkin->m_afBorderColor[1] = (1.0f/255)*bd_g;
		pkSkin->m_afBorderColor[2] = (1.0f/255)*bd_b;

		pkSkin->m_unBorderSize = (unsigned short) bd_sz;
		pkSkin->m_bTileBkSkin = (tile == 0) ? false : true;
		pkSkin->m_bTransparent = (trans == 0) ? false : true;

		kNewSkinInfo.second = pkSkin;

		kAllSkinsTempArray.push_back( kNewSkinInfo );
	}

	// Skapa en nytt skin till fönstret.
	m_pkGuiBuilder->m_bAllocateNewSkins = true;

	// Forsätt med att ladda in alla fönster.
	// De är sorterade så att main windows laddas in först.
	for( ; i<vkSections.size(); i++)
	{
		char* szType = pkINI->GetValue((char*)vkSections[i].c_str(), "type");
		CtrlType eNewType = m_pkGuiBuilder->GetType(szType);

		if(eNewType == NONE)
		{
			printf("Error loading new type!\n");
			continue;
		}

		int wnd_id = atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"id"));
		char* name = pkINI->GetValue((char*)vkSections[i].c_str(), "name");
		int parentwnd_id = atoi(pkINI->GetValue(
			(char*)vkSections[i].c_str(), "parent_id"));
		char* parent_name = pkINI->GetValue((char*)vkSections[i].c_str(),
			"parent_name");
		bool bVisible = atoi(pkINI->GetValue((char*)vkSections[i].c_str(), 
			"visible")) == 0 ? false : true;
		bool bEnabled = true;//atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"enabled")) == 0 ? false : true;
		int x=atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"area_left"));
		int y=atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"area_top"));
		int w=atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"area_right"))-x;
		int h=atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"area_bottom"))-y;
		Rect rc_m( atoi(pkINI->GetValue((char*)vkSections[i].c_str(),
			"move_area_left")),
			atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "move_area_top")),
			atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "move_area_right")),
			atoi(pkINI->GetValue((char*)vkSections[i].c_str(), "move_area_bottom")));
		int iTabOrder=atoi(pkINI->GetValue((char*)vkSections[i].c_str(),"tab_order"));
		char* text = pkINI->GetValue((char*)vkSections[i].c_str(), "text");
		
		ZGuiWnd* pkWnd;
		ZGuiWnd* pkParent;

		if(eNewType != WINDOW) // Om det är en control
		{
			pkParent = m_pkGuiBuilder->GetWnd(parent_name);

			if(pkParent == NULL)
			{
				printf("GUIRESEDIT ERROR: Can't create window named %s%s",
					"because no parent was found.\n", name);
			}
		}

		switch(eNewType)
		{
		case WINDOW: // alla parents har printats först
			{
				int mainwnd_id = atoi(pkINI->GetValue(
					(char*)vkSections[i].c_str(), "mainwnd_id"));
				pkWnd=m_pkGuiBuilder->CreateMainWindow(mainwnd_id,wnd_id,name,
					x,y,w,h,m_oMainWndProc);
			}
			break;
		case LABEL:
			pkWnd=m_pkGuiBuilder->CreateLabel(pkParent,wnd_id,name,x,y,w,h,text);
			break;
		case TEXTBOX:
			pkWnd=m_pkGuiBuilder->CreateTextbox(pkParent,wnd_id,name,x,y,w,h,false);	
			break;
		case BUTTON:
			pkWnd=m_pkGuiBuilder->CreateButton(pkParent,wnd_id,name,x,y,w,h,text);	
			break;
		case CHECKBOX:
			pkWnd=m_pkGuiBuilder->CreateCheckbox(pkParent,wnd_id,name,x,y,w,h,text);	
			break;
		case RADIOBUTTON:
			{
				int radio_group = atoi(pkINI->GetValue(
					(char*)vkSections[i].c_str(), "radio_group"));
				char* radio_group_name = pkINI->GetValue(vkSections[i].c_str(),
					"radio_group_name");
				pkWnd=m_pkGuiBuilder->CreateRadioButton(pkParent,wnd_id,name,
					radio_group,radio_group_name,false,x,y,w,h,text);	
			}
			break;
		case RADIOBUTTON_NEW_GROUP:
			{
				int radio_group = atoi(pkINI->GetValue(
					(char*)vkSections[i].c_str(), "radio_group"));
				char* radio_group_name = pkINI->GetValue(vkSections[i].c_str(),
					"radio_group_name");
				pkWnd=m_pkGuiBuilder->CreateRadioButton(pkParent,wnd_id,name,
					radio_group,radio_group_name,true,x,y,w,h,text);	
			}
			break;
		case LISTBOX:
			pkWnd=m_pkGuiBuilder->CreateListbox(pkParent,wnd_id,name,x,y,w,h);
			break;
		case COMBOBOX:
			{
				int num_visible_rows = atoi(pkINI->GetValue(
					(char*)vkSections[i].c_str(),"num_visible_rows"));
				pkWnd=m_pkGuiBuilder->CreateCombobox(pkParent,wnd_id,name,x,y,
					w,h,false);
				((ZGuiCombobox*)pkWnd)->SetNumVisibleRows(num_visible_rows);
			}
			break;
		case SCROLLBAR:
			pkWnd=m_pkGuiBuilder->CreateScrollbar(pkParent,wnd_id,name,x,y,w,h);
			break;
		}

		pkWnd->SetWindowFlag(WF_CANHAVEFOCUS); // to allow movement.

		SelectSkins(kAllSkinsTempArray, pkWnd);

		// Set last ID
		if(m_pkGuiBuilder->m_iLastID < (int) pkWnd->GetID())
			m_pkGuiBuilder->m_iLastID = pkWnd->GetID();

		m_pkCreatedWindows.push_back(CreateWnd(pkWnd));
	}

	// Skapa en nytt skin till fönstret.
	m_pkGuiBuilder->m_bAllocateNewSkins = false;

	// Raise last ID by one
	m_pkGuiBuilder->m_iLastID++;

	// Clear temp skin vektor.
	for( i=0; i<kAllSkinsTempArray.size(); i++)
	{
		delete kAllSkinsTempArray[i].second;
	}
	
	return true;
}

void ControlBox::ClearAll()
{
	unsigned int i;
	unsigned int uiNumWindows = m_pkCreatedWindows.size();

	vector<ZGuiWnd::SKIN_DESC> kSkinList;

	// First, delete all skin.
	for( i=0; i<uiNumWindows; i++)
	{
		m_pkCreatedWindows[i].m_pkWnd->GetWndSkinsDesc(kSkinList);

		for(unsigned int j=0; j<kSkinList.size(); j++)
		{
			delete kSkinList[j].first;
			kSkinList[j].first = NULL;
		}

		kSkinList.clear(); 
	}

	// Create a list with windows that are Main windows
	// and destroy only them.
	vector<int> kMainWindows;
	for( i=0; i<m_pkCreatedWindows.size(); i++)
		if( m_pkGuiBuilder->GetWndType(m_pkCreatedWindows[i].m_pkWnd) == WINDOW )
			kMainWindows.push_back(i); 

	// Then destroy all main windows ( the childrens are destroyed to )
	for( i=0; i<kMainWindows.size(); i++)
		if(!m_pkGui->UnregisterWindow(m_pkCreatedWindows[kMainWindows[i]].m_pkWnd))
			cout << "Failed to unregister window!" << endl;
	
	SelectWnd::GetInstance()->m_pkWnd = NULL;

	m_pkCreatedWindows.clear();
}

void ControlBox::SelectSkins(vector<tSkinInf>& kAllSkinsTempArray, ZGuiWnd* pkWnd)
{
	vector<ZGuiWnd::SKIN_DESC> kSkinList;
	pkWnd->GetWndSkinsDesc(kSkinList);

	string strWndName = pkWnd->GetName();

	// Loopa igenom alla skins som detta fönster har
	for(unsigned int i=0; i<kSkinList.size(); i++)
	{
		ZGuiSkin* pkSkin = kSkinList[i].first;
		string strDesc = kSkinList[i].second;

		// Loopa igenom alla skins som finns och välj ut ett som passar.
		for(unsigned int j=0; j<kAllSkinsTempArray.size(); j++)
		{
			if( kAllSkinsTempArray[j].first.first == strWndName &&
				kAllSkinsTempArray[j].first.second == strDesc )
			{
				if(pkSkin != NULL)
					*pkSkin = *kAllSkinsTempArray[j].second;
				break;
			}
		}
	}
}
