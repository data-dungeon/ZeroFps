// skinbox.cpp: implementation of the SkinBox class.
//
///////////////////////////////////////////////////////////////////////////////

#include "skinbox.h"
#include "../zerofps/basic/zfbasicfs.h"
#include "../zerofps/render/texturemanager.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//
SkinBox::SkinBox(GuiBuilder* pkGuiBuilder, ZGuiWndProc oMainWndProc,
				 ZFBasicFS* pkBasicFS, TextureManager* pkTexMan) 
				 : DlgBox(pkGuiBuilder, oMainWndProc)
{
	m_pkBasicFS = pkBasicFS;
	m_pkTexMan = pkTexMan;
	m_unRealBorderSz = 0;
	m_eSelTextureType = BACKGROUND;
	m_usSkinTypeIndex = 0;
}

SkinBox::~SkinBox()
{
	
}

///////////////////////////////////////////////////////////////////////////////
// Name: Create
// Description: 
//
bool SkinBox::Create(int x,int y,int w,int h,ZGuiWndProc pkWndProc)
{
	m_pkDlgBox = m_pkGuiBuilder->CreateMainWindow(ID_SKINBOX_MAIN_WND,
		ID_SKINBOX_WND,"SkinBoxWnd",x,y,w,h,pkWndProc);
	m_pkDlgBox->SetMoveArea(m_pkGuiBuilder->m_rcScreen);
	m_pkGuiBuilder->CreateCloseButton(m_pkDlgBox,ID_SKINBOX_CLOSE_BN,NULL);
	m_pkGuiBuilder->CreateOKButton(m_pkDlgBox,ID_SKINBOX_OK_BN,NULL,true);
	m_pkGuiBuilder->CreateCancelButton(m_pkDlgBox,ID_SKINBOX_CANCEL_BN,NULL,true);

	ZGuiListbox* pkTextureList = m_pkGuiBuilder->CreateListbox(m_pkDlgBox,
		ID_SKINBOX_TEXTURELIST_LB,"TexturesLB",0,20,250,280); 
	pkTextureList->SetGUI(m_pkGui);

	FillTextureList();

	ZGuiLabel* pkPreview = m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,"PreviewLAB",-64,
		-64,64,64,"");
	
	typedef pair<string,string> name_label;
	typedef pair<int,name_label> controller;
	int id_start = ID_SKINTEXTURETYPE_RB;
	controller akCtrls[] = 
	{
		controller(id_start++,name_label("TexTypeBkRb","Background")),					// 0
		controller(id_start++,name_label("TexTypeHorzBorderRb","Horz.Border")),			// 1
		controller(id_start++,name_label("TexTypeVertBorderRb","Vert.Border")),			// 2
		controller(id_start++,name_label("TexTypeBorderCornerRb","Corner")),			// 3
		controller(id_start++,name_label("TexTypeBkARb","Background Alpha")),			// 0
		controller(id_start++,name_label("TexTypeHorzBorderARb","Horz.Border Alpha")),	// 1
		controller(id_start++,name_label("TexTypeVertBorderARb","Vert.Border Alpha")),	// 2
		controller(id_start++,name_label("TexTypeBorderCornerARb","Corner Alpha")),		// 3
	};

	ZGuiWnd* pkRadioBn;

	w = 16; h = 16; x = 255; y = 20; 
	for(int i=0; i<sizeof(akCtrls) / sizeof(akCtrls[1]); i++)
	{
		pkRadioBn = m_pkGuiBuilder->CreateRadioButton(m_pkDlgBox,
			akCtrls[i].first,(char*)akCtrls[i].second.first.c_str(),
			ID_SKINTEXTURETYPE_GROUP,false,x,y,w,h,
			(char*)akCtrls[i].second.second.c_str(), (i==0));
		y += 20;

		pkRadioBn->SetWindowFlag(WF_CANHAVEFOCUS);
	}

	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x,0,128,16,
		"Texture type","titlebar");
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,x,y,90,16,
		"Border size:","titlebar");
	m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,ID_SKINBORDERSIZE_EB,
		"SkinBorderSizeEB",345,y,38,16,false);

	m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox,ID_SKINTEXTURE_CHB,
		"SkinDlgTextureCHB",x,y+20,16,16,"Texture");

	m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox,ID_SKINTILEBORDER_CHB,
		"SkinDlgTileSkinCHB", x,y+40,16,16,"Tile skin");

	m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox,ID_TRANSPARENTBK_CHB,
		"SkinTransparentBkCHB", x,y+60,16,16,"Transparent");

	float fThumbSize = (float) 10 / (float) 255;

	x = 250;

	ZGuiScrollbar* pkScrollbar;
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox,
		ID_SKINBKREDCOLOR_SB,"SkinBkRedColorSB",x+145,0,20,300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,255);
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox,
		ID_SKINBKGREENCOLOR_SB,"SkinBkGreenColorSB",270+145,0,20,300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,255);
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox,
		ID_SKINBKBLUECOLOR_SB,"SkinBkBlueColorSB",290+145,0,20,300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,255);

	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox,
		ID_SKINBORDERREDCOLOR_SB,"SkinBdRedColorSB",320+145,0,20,300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,0);
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox,
		ID_SKINBORDERGREENCOLOR_SB,"SkinBdGreenColorSB",340+145,0,20,300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,0);
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox,
		ID_SKINBORDERBLUECOLOR_SB,"SkinBdBlueColorSB",360+145,0,20,300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,0);

	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,255,260,30,18,"Bk.",
		"titlebar");
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox,0,NULL,255,280,30,18,"Bd.",
		"titlebar");

	m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,ID_SKINBKREDCOLOR_EB,"BkRedEB",
		285,260,31,18,false);
	m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,ID_SKINBKGREENCOLOR_EB,"BkGreenEB",
		317,260,31,18,false);
	m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,ID_SKINBKBLUECOLOR_EB,"BkBlueEB",
		349,260,31,18,false);
	m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,ID_SKINBORDERREDCOLOR_EB,"BdRedEB",
		285,280,31,18,false);
	m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,ID_SKINBORDERGREENCOLOR_EB,"BdGreenEB",
		317,280,31,18,false);
	m_pkGuiBuilder->CreateTextbox(m_pkDlgBox,ID_SKINBORDERBLUECOLOR_EB,"BdBlueEB",
		349,280,31,18,false);

	m_pkGuiBuilder->CreateCombobox(m_pkDlgBox,ID_SKINDLGTYPES_CB,
		"SkinTypesCB",0,0,250,20,true);
	((ZGuiCombobox*)m_pkGuiBuilder->GetWnd("SkinTypesCB"))
		->SetLabelText("Skin type: 1");

	m_pkGui->ShowMainWindow(m_pkGuiBuilder->GetWnd("SkinBoxWnd"),false);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: OnOpen
// Description: 
//
bool SkinBox::OnOpen(int x,int y)
{
	if(!(x==-1 && y==-1))
		m_pkDlgBox->SetPos(x,y,true,true);

	// Clear child list.
	ZGuiCombobox* pkList = (ZGuiCombobox*) 
		m_pkGuiBuilder->GetWnd("SkinTypesCB");
	pkList->RemoveAllItems();

	if(SelectWnd::GetInstance()->m_pkWnd)
	{
		//
		// Add skin types to list
		//
		vector<ZGuiWnd::SKIN_DESC> pkSkinDesc;
		SelectWnd::GetInstance()->m_pkWnd->GetWndSkinsDesc(pkSkinDesc);
		for(unsigned int i=0; i<pkSkinDesc.size(); i++)
		{
			char szName[128];
			sprintf(szName, "%i.%s", i+1, pkSkinDesc[i].second.c_str());
			pkList->AddItem(szName,i);
		}
		
		//
		// Setup default skins parameters
		//
		m_vkPreviewSkin.clear();
		m_vkPreviewSkin.resize(pkSkinDesc.size()+1);

		for(i=0; i<pkSkinDesc.size(); i++)
			m_vkPreviewSkin[i] = *pkSkinDesc[i].first;

		ZGuiSkin* pkSkin = &m_vkPreviewSkin[m_usSkinTypeIndex];
		SetupDefaultSkin();

		m_pkGui->ShowMainWindow(m_pkGuiBuilder->GetWnd("SkinBoxWnd"),true);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: OnClose
// Description: 
//
bool SkinBox::OnClose(bool bSave)
{
	m_pkGui->ShowMainWindow(m_pkGuiBuilder->GetWnd("SkinBoxWnd"),false);
	
	if(SelectWnd::GetInstance()->m_pkWnd)
	{
		if(bSave)
		{
			// Copy preview skin to selected window skin
			vector<ZGuiWnd::SKIN_DESC> pkSkinDesc;
			SelectWnd::GetInstance()->m_pkWnd->GetWndSkinsDesc(pkSkinDesc);
			*pkSkinDesc[m_usSkinTypeIndex].first = 
				m_vkPreviewSkin[m_usSkinTypeIndex];
		}

		ZGuiWnd* pkParent = SelectWnd::GetInstance()->m_pkWnd->GetParent(true);

		if(pkParent == NULL)
		{
			pkParent = SelectWnd::GetInstance()->m_pkWnd;
		}

		m_pkGui->SetFocus(pkParent);
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Name: FillTextureList
// Description: 
//
void SkinBox::FillTextureList()
{
	ZGuiListbox* pkTextureList = (ZGuiListbox*) 
		m_pkGuiBuilder->GetWnd("TexturesLB");

	vector<string> vkFiles;
	vector<string> vkFilters;
	vkFilters.push_back(".bmp"); 
	string strDir = "../data/textures";
	m_pkBasicFS->ListDirFilter(&vkFiles,vkFilters,strDir.c_str(),true);

	sort(vkFiles.begin(),vkFiles.end());

	for( unsigned int i=0; i<vkFiles.size(); i++)	
		pkTextureList->AddItem((char*)vkFiles[i].c_str(),i,false); 
}

///////////////////////////////////////////////////////////////////////////////
// Name: UpdatePreviewImage
// Description: 
//
void SkinBox::UpdatePreviewImage(char* szTexture)
{
	long* pID;

	switch(m_eSelTextureType)
	{
	case BACKGROUND:
		pID = &m_vkPreviewSkin[m_usSkinTypeIndex].m_iBkTexID; 
		break;
	case BORDERHORZ:
		pID = &m_vkPreviewSkin[m_usSkinTypeIndex].m_iHorzBorderTexID;
		break;
	case BORDERVERT:
		pID = &m_vkPreviewSkin[m_usSkinTypeIndex].m_iVertBorderTexID;
		break;
	case BORDERCORNER:
		pID = &m_vkPreviewSkin[m_usSkinTypeIndex].m_iBorderCornerTexID;
		break;
	case BACKGROUNDALPHA:
		pID = &m_vkPreviewSkin[m_usSkinTypeIndex].m_iBkTexAlphaID; 
		break;
	case BORDERHORZALPHA:
		pID = &m_vkPreviewSkin[m_usSkinTypeIndex].m_iHorzBorderTexAlphaID;
		break;
	case BORDERVERTALPHA:
		pID = &m_vkPreviewSkin[m_usSkinTypeIndex].m_iVertBorderTexAlphaID;
		break;
	case BORDERCORNERALPHA:
		pID = &m_vkPreviewSkin[m_usSkinTypeIndex].m_iBorderCornerTexAlphaID;
		break;
	default:
		return;
	}

	char szFile[256];
	sprintf(szFile,"file:../data/textures/%s",szTexture);

	(*pID) = m_pkTexMan->Load(szFile,0);

}

///////////////////////////////////////////////////////////////////////////////
// Name: SetSkinBkColor
// Description: 
//
void SkinBox::SetSkinBkColor(int r,int g,int b)
{
	if(r != -1) { if(r < 0) r=0; if(r > 255) r=255; }
	if(g != -1) { if(g < 0) g=0; if(g > 255) g=255; }
	if(b != -1) { if(b < 0) b=0; if(b > 255) b=255; } 

	if(r != -1) 
		m_vkPreviewSkin[m_usSkinTypeIndex].m_afBkColor[0] = 
			(float) (1.0f/255) * r;
	if(g != -1) 
		m_vkPreviewSkin[m_usSkinTypeIndex].m_afBkColor[1] = 
			(float) (1.0f/255) * g;
	if(b != -1) 
		m_vkPreviewSkin[m_usSkinTypeIndex].m_afBkColor[2] = 
			(float) (1.0f/255) * b;

	if(r != -1) m_pkGuiBuilder->SetTextInt("BkRedEB",r);
	if(g != -1) m_pkGuiBuilder->SetTextInt("BkGreenEB",g);
	if(b != -1) m_pkGuiBuilder->SetTextInt("BkBlueEB",b);
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetSkinBorderColor
// Description: 
//
void SkinBox::SetSkinBorderColor(int r,int g,int b)
{
	if(r != -1) { if(r < 0) r=0; if(r > 255) r=255; }
	if(g != -1) { if(g < 0) g=0; if(g > 255) g=255; }
	if(b != -1) { if(b < 0) b=0; if(b > 255) b=255; }

	if(r != -1) 
		m_vkPreviewSkin[m_usSkinTypeIndex].m_afBorderColor[0] = (float) (1.0f/255) * r;
	if(g != -1) 
		m_vkPreviewSkin[m_usSkinTypeIndex].m_afBorderColor[1] = (float) (1.0f/255) * g;
	if(b != -1) 
		m_vkPreviewSkin[m_usSkinTypeIndex].m_afBorderColor[2] = (float) (1.0f/255) * b;
	
	if(r != -1) m_pkGuiBuilder->SetTextInt("BdRedEB",r);
	if(g != -1) m_pkGuiBuilder->SetTextInt("BdGreenEB",g);
	if(b != -1) m_pkGuiBuilder->SetTextInt("BdBlueEB",b);
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetSkinBorderSize
// Description: 
//
void SkinBox::SetSkinBorderSize(int iSize)
{
	m_vkPreviewSkin[m_usSkinTypeIndex].m_unBorderSize = iSize;
	m_pkGuiBuilder->SetTextInt("SkinBorderSizeEB",iSize);
}

///////////////////////////////////////////////////////////////////////////////
// Name: SetupDefaultSkin
// Description: 
//
void SkinBox::SetupDefaultSkin()
{
	ZGuiSkin* pkSkin = &m_vkPreviewSkin[m_usSkinTypeIndex];

	m_pkGuiBuilder->GetWnd("PreviewLAB")->SetSkin(pkSkin);

	SetSkinBkColor((int)(255.0f*pkSkin->m_afBkColor[0]),
		(int)(255.0f*pkSkin->m_afBkColor[1]),
		(int)(255.0f*pkSkin->m_afBkColor[2]));
	SetSkinBorderColor((int)(255*pkSkin->m_afBorderColor[0]),
		(int)(255.0f*pkSkin->m_afBorderColor[1]),
		(int)(255.0f*pkSkin->m_afBorderColor[2]));

	m_unRealBorderSz = pkSkin->m_unBorderSize;
	m_vkPreviewSkin[m_usSkinTypeIndex].m_unBorderSize = m_unRealBorderSz;
	if(m_vkPreviewSkin[m_usSkinTypeIndex].m_unBorderSize > 16)
		m_vkPreviewSkin[m_usSkinTypeIndex].m_unBorderSize = 16;

	m_pkGuiBuilder->SetTextInt("SkinBorderSizeEB",pkSkin->m_unBorderSize);

	if(pkSkin->m_bTileBkSkin)
		((ZGuiCheckbox*)m_pkGuiBuilder->
			GetWnd("SkinDlgTileSkinCHB"))->CheckButton();
	else
		((ZGuiCheckbox*)m_pkGuiBuilder->
			GetWnd("SkinDlgTileSkinCHB"))->UncheckButton();

	if(m_vkPreviewSkin[m_usSkinTypeIndex].m_bTransparent)
		((ZGuiCheckbox*)m_pkGuiBuilder->
			GetWnd("SkinTransparentBkCHB"))->CheckButton();
	else
		((ZGuiCheckbox*)m_pkGuiBuilder->
			GetWnd("SkinTransparentBkCHB"))->UncheckButton();
	//
	// Select listbox item
	//
	ZGuiListbox* pkListbox;
	pkListbox = ((ZGuiListbox*)m_pkGuiBuilder->GetWnd("TexturesLB"));

	int iListIndex=-1;
	long* pTexType;
	char szTexture[128];
	if(GetSelTexInfo(pkSkin, pTexType, szTexture))
		iListIndex = pkListbox->Find(szTexture);
	
	if(iListIndex > 0)
	{
		pkListbox->SelItem(iListIndex);
		((ZGuiCheckbox*)m_pkGuiBuilder->GetWnd(
			"SkinDlgTextureCHB"))->CheckButton();
	}
	else
	{
		pkListbox->SelNone();
		((ZGuiCheckbox*)m_pkGuiBuilder->GetWnd(
			"SkinDlgTextureCHB"))->UncheckButton();
	}

	// 
	// Set scrollbar pos.
	//
	m_pkGuiBuilder->SetScrollPos("SkinBkRedColorSB", 
		pkSkin->m_afBkColor[0]);
	m_pkGuiBuilder->SetScrollPos("SkinBkGreenColorSB", 
		pkSkin->m_afBkColor[1]);
	m_pkGuiBuilder->SetScrollPos("SkinBkBlueColorSB", 
		pkSkin->m_afBkColor[2]);

	m_pkGuiBuilder->SetScrollPos("SkinBdRedColorSB", 
		pkSkin->m_afBorderColor[0]);
	m_pkGuiBuilder->SetScrollPos("SkinBdGreenColorSB", 
		pkSkin->m_afBorderColor[1]);
	m_pkGuiBuilder->SetScrollPos("SkinBdBlueColorSB", 
		pkSkin->m_afBorderColor[2]);

	//
	// Select texture
	//
	int iTexID = -1;

	switch(m_eSelTextureType)
	{
	case BACKGROUND:
		iTexID = pkSkin->m_iBkTexID;
		pTexType = &pkSkin->m_iBkTexID;
		break;
	case BORDERHORZ:
		iTexID = pkSkin->m_iHorzBorderTexID;
		pTexType = &pkSkin->m_iHorzBorderTexID;
		break;
	case BORDERVERT:
		iTexID = pkSkin->m_iVertBorderTexID;
		pTexType = &pkSkin->m_iVertBorderTexID;
		break;
	case BORDERCORNER:
		iTexID = pkSkin->m_iBorderCornerTexID;
		pTexType = &pkSkin->m_iBorderCornerTexID;
		break;
	case BACKGROUNDALPHA:
		iTexID = pkSkin->m_iBkTexAlphaID;
		pTexType = &pkSkin->m_iBkTexAlphaID;
		break;
	case BORDERHORZALPHA:
		iTexID = pkSkin->m_iHorzBorderTexAlphaID;
		pTexType = &pkSkin->m_iHorzBorderTexAlphaID;
		break;
	case BORDERVERTALPHA:
		iTexID = pkSkin->m_iVertBorderTexAlphaID;
		pTexType = &pkSkin->m_iVertBorderTexAlphaID;
		break;
	case BORDERCORNERALPHA:
		iTexID = pkSkin->m_iBorderCornerTexAlphaID;
		pTexType = &pkSkin->m_iBorderCornerTexAlphaID;
		break;
	}

	if(iTexID > 0)
	{
		char szFile[256];
		sprintf(szFile,m_pkTexMan->GetFileName(iTexID));
		(*pTexType) = m_pkTexMan->Load(szFile,0);
	}
}

bool SkinBox::DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
					   int iNumberOfParams,void *pkParams ) 
{
	int color = 0;

	switch(uiMessage)
	{
	// Command Messages
	case ZGM_COMMAND:
		switch(((int*)pkParams)[0]) // control id
		{
		case ID_SKINTEXTURETYPE_RB:
		case ID_SKINTEXTURETYPE_RB+1:	case ID_SKINTEXTURETYPE_RB+2:
		case ID_SKINTEXTURETYPE_RB+3:	case ID_SKINTEXTURETYPE_RB+4:
		case ID_SKINTEXTURETYPE_RB+5:	case ID_SKINTEXTURETYPE_RB+6:
		case ID_SKINTEXTURETYPE_RB+7:
			{
				m_eSelTextureType = TextureType(
					((int*)pkParams)[0]-ID_SKINTEXTURETYPE_RB);

				ZGuiListbox* pkListbox;
				pkListbox=((ZGuiListbox*)m_pkGuiBuilder->GetWnd("TexturesLB"));

				int iListIndex=-1;
				long* pTexType;
				char szTexture[128];
				if(GetSelTexInfo(&m_vkPreviewSkin[m_usSkinTypeIndex], 
					pTexType, szTexture))
					iListIndex = pkListbox->Find(szTexture);
				
				if(iListIndex > 0)
				{
					pkListbox->SelItem(iListIndex);
					((ZGuiCheckbox*)m_pkGuiBuilder->GetWnd(
						"SkinDlgTextureCHB"))->CheckButton();
				}
				else
				{
					pkListbox->SelNone();
					((ZGuiCheckbox*)m_pkGuiBuilder->GetWnd(
						"SkinDlgTextureCHB"))->UncheckButton();
				}
			}
			break;
		case ID_SKINBOX_CANCEL_BN:
		case ID_SKINBOX_CLOSE_BN:
			OnClose(false);
			break;
		case ID_SKINBOX_OK_BN:
			OnClose(true);
			break;
		case ID_SKINTEXTURE_CHB:
			if(!m_pkGuiBuilder->IsButtonChecked("SkinDlgTextureCHB"))
			{
				((ZGuiListbox*)m_pkGuiBuilder->GetWnd("TexturesLB"))->SelNone();

				long* pTexType;
				char szTexture[128];
				if(GetSelTexInfo(&m_vkPreviewSkin[m_usSkinTypeIndex],pTexType,
					szTexture))
					(*pTexType) = -1;
			}
			break;
		case ID_SKINTILEBORDER_CHB:
			m_vkPreviewSkin[m_usSkinTypeIndex].m_bTileBkSkin = 
				m_pkGuiBuilder->IsButtonChecked(
				"SkinDlgTileSkinCHB");
			break;
		case ID_TRANSPARENTBK_CHB:
			m_vkPreviewSkin[m_usSkinTypeIndex].m_bTransparent = 
				m_pkGuiBuilder->IsButtonChecked(
				"SkinTransparentBkCHB");
			break;
		}
		break;

	// Select List Box Item Messages
	case ZGM_SELECTLISTITEM:
		int iListBoxID; iListBoxID = ((int*)pkParams)[0];
		int iSelItemIndex; iSelItemIndex = ((int*)pkParams)[1];

		switch(iListBoxID)
		{
		case ID_SKINBOX_TEXTURELIST_LB:
			ZGuiListbox* pkListbox;
			pkListbox = ((ZGuiListbox*)m_pkGuiBuilder->GetWnd("TexturesLB"));
			
			ZGuiListitem* pkItem;
			pkItem = pkListbox->GetItem(iSelItemIndex);
			UpdatePreviewImage(pkItem->GetText());

			((ZGuiCheckbox*)m_pkGuiBuilder->GetWnd("SkinDlgTextureCHB"))
				->CheckButton();
			break;
		}
		break;

	// Scrollbar Messages
	case ZGM_SCROLL:
		int iScrollBarID; iScrollBarID = ((int*)pkParams)[0];
		int iScrollBarPos; iScrollBarPos = ((int*)pkParams)[2];

		switch(iScrollBarID)
		{
		case ID_SKINBKREDCOLOR_SB:
			SetSkinBkColor(iScrollBarPos,-1,-1);
			break;
		case ID_SKINBKGREENCOLOR_SB:
			SetSkinBkColor(-1,iScrollBarPos,-1);
			break;
		case ID_SKINBKBLUECOLOR_SB:
			SetSkinBkColor(-1,-1,iScrollBarPos);
			break;
		case ID_SKINBORDERREDCOLOR_SB:
			SetSkinBorderColor(iScrollBarPos,-1,-1);
			break;
		case ID_SKINBORDERGREENCOLOR_SB:
			SetSkinBorderColor(-1,iScrollBarPos,-1);
			break;
		case ID_SKINBORDERBLUECOLOR_SB:
			SetSkinBorderColor(-1,-1,iScrollBarPos);
			break;
		}
		break;

	// Editbox Typing Message
	case ZGM_EN_CHANGE:

		switch(((int*)pkParams)[0]) // id of the textbox
		{
		case ID_SKINBORDERSIZE_EB:
			m_unRealBorderSz = m_pkGuiBuilder->GetTextInt("SkinBorderSizeEB");
			m_vkPreviewSkin[m_usSkinTypeIndex].m_unBorderSize = m_unRealBorderSz;
			if(m_vkPreviewSkin[m_usSkinTypeIndex].m_unBorderSize > 16)
				m_vkPreviewSkin[m_usSkinTypeIndex].m_unBorderSize = 16;
			break;

		case ID_SKINBKREDCOLOR_EB:
		case ID_SKINBORDERREDCOLOR_EB:
		case ID_SKINBKGREENCOLOR_EB:
		case ID_SKINBORDERGREENCOLOR_EB:
		case ID_SKINBKBLUECOLOR_EB:
		case ID_SKINBORDERBLUECOLOR_EB:
			{
				string strTextbox, strScrollbar;
				int iBdIndex=-1, iBkIndex=-1;

				switch(((int*)pkParams)[0])
				{
				case ID_SKINBKREDCOLOR_EB:
					strTextbox = "BkRedEB";
					strScrollbar = "SkinBkRedColorSB";
					iBkIndex = 0;
					break;
				case ID_SKINBORDERREDCOLOR_EB:
					strTextbox = "BdRedEB";
					strScrollbar = "SkinBdRedColorSB";
					iBdIndex = 0;
					break;
				case ID_SKINBKGREENCOLOR_EB:
					strTextbox = "BkGreenEB";
					strScrollbar = "SkinBkGreenColorSB";
					iBkIndex = 1;
					break;
				case ID_SKINBORDERGREENCOLOR_EB:
					strTextbox = "BdGreenEB";
					strScrollbar = "SkinBdGreenColorSB";
					iBdIndex = 1;
					break;
				case ID_SKINBKBLUECOLOR_EB:
					strTextbox = "BkBlueEB";
					strScrollbar = "SkinBkBlueColorSB";
					iBkIndex = 2;
					break;
				case ID_SKINBORDERBLUECOLOR_EB:
					strTextbox = "BdBlueEB";
					strScrollbar = "SkinBdBlueColorSB";
					iBdIndex = 2;
					break;
				}

				if((color = m_pkGuiBuilder->GetTextInt(strTextbox.c_str())) > 255)
				{
					color = 255;
					m_pkGuiBuilder->SetTextInt((char*)strTextbox.c_str(), color);
				}
				
				if(iBkIndex != -1)
				{
					m_vkPreviewSkin[m_usSkinTypeIndex].m_afBkColor[iBkIndex]=
						1.0f/255*color;
					m_pkGuiBuilder->SetScrollPos((char*)strScrollbar.c_str(), 
						m_vkPreviewSkin[m_usSkinTypeIndex].m_afBkColor[iBkIndex]);
				}
				if(iBdIndex != -1)
				{
					m_vkPreviewSkin[m_usSkinTypeIndex].m_afBorderColor[iBdIndex]=
						1.0f/255*color;
					m_pkGuiBuilder->SetScrollPos((char*)strScrollbar.c_str(), 
						m_vkPreviewSkin[m_usSkinTypeIndex].m_afBorderColor[iBdIndex]);
				}

				break;
			}
		}
		break;

		case ZGM_CBN_SELENDOK:
			
			int iID = ((int*)pkParams)[0];
			ZGuiListitem* pkSelItem;

			switch(iID)
			{
				case ID_SKINDLGTYPES_CB:
				{
					ZGuiCombobox* pkCbox = (ZGuiCombobox*)
						m_pkGuiBuilder->GetWnd("SkinTypesCB");

					pkSelItem = pkCbox->GetListbox()->GetSelItem();
					m_usSkinTypeIndex = pkSelItem->GetIndex();

					char szText[64];
					sprintf(szText, "Skin type: %i",m_usSkinTypeIndex+1);
					pkCbox->SetLabelText(szText);

					vector<ZGuiWnd::SKIN_DESC> pkSkinDesc;
					SelectWnd::GetInstance()->m_pkWnd->GetWndSkinsDesc(pkSkinDesc);
					SetupDefaultSkin();
				}
				break;
			}

			break;
	}
	return true;
}

bool SkinBox::GetSelTexInfo(ZGuiSkin* pkSkin, long* &pTexType, char* szTexture)
{
	if(!pkSkin)
		return false;

	int iTexID;

	switch(m_eSelTextureType)
	{
	case BACKGROUND:
		iTexID = pkSkin->m_iBkTexID;
		pTexType = &pkSkin->m_iBkTexID;
		break;
	case BORDERHORZ:
		iTexID = pkSkin->m_iHorzBorderTexID;
		pTexType = &pkSkin->m_iHorzBorderTexID;
		break;
	case BORDERVERT:
		iTexID = pkSkin->m_iVertBorderTexID;
		pTexType = &pkSkin->m_iVertBorderTexID;
		break;
	case BORDERCORNER:
		iTexID = pkSkin->m_iBorderCornerTexID;
		pTexType = &pkSkin->m_iBorderCornerTexID;
		break;
	case BACKGROUNDALPHA:
		iTexID = pkSkin->m_iBkTexAlphaID;
		pTexType = &pkSkin->m_iBkTexAlphaID;
		break;
	case BORDERHORZALPHA:
		iTexID = pkSkin->m_iHorzBorderTexAlphaID;
		pTexType = &pkSkin->m_iHorzBorderTexAlphaID;
		break;
	case BORDERVERTALPHA:
		iTexID = pkSkin->m_iVertBorderTexAlphaID;
		pTexType = &pkSkin->m_iVertBorderTexAlphaID;
		break;
	case BORDERCORNERALPHA:
		iTexID = pkSkin->m_iBorderCornerTexAlphaID;
		pTexType = &pkSkin->m_iBorderCornerTexAlphaID;
		break;
	default:
		return false;
	}

	if(iTexID < 0)
		return false;

	strcpy(szTexture, m_pkTexMan->GetFileName(iTexID));

	string strFormat = szTexture;
	int offset = strFormat.find_last_of('/');
	if(offset == string::npos || offset < 0)
		return false;

	char temp[512];
	strcpy(temp, szTexture+(offset+1));
	strcpy(szTexture, temp);

	return true;
}
