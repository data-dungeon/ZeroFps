// skinbox.cpp: implementation of the SkinBox class.
//
//////////////////////////////////////////////////////////////////////

#include "skinbox.h"
#include "../zerofps/basic/zfbasicfs.h"
#include "../zerofps/render/texturemanager.h"
#include <algorithm>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SkinBox::SkinBox(GuiBuilder* pkGuiBuilder, ZFBasicFS* pkBasicFS, TextureManager* pkTexMan) 
	: DlgBox(pkGuiBuilder)
{
	m_pkBasicFS = pkBasicFS;
	m_pkTexMan = pkTexMan;
}

SkinBox::~SkinBox()
{

}

bool SkinBox::Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc)
{
	m_pkDlgBox = m_pkGuiBuilder->CreateMainWindow(ID_SKINBOX_MAIN_WND,
		ID_SKINBOX_WND,x,y,w,h,pkWndProc,"lblue");
	m_pkDlgBox->SetMoveArea(m_pkGuiBuilder->m_rcScreen);
	m_pkGuiBuilder->Register(m_pkDlgBox, "SkinBoxWnd");
	m_pkGuiBuilder->CreateCloseButton(m_pkDlgBox, ID_SKINBOX_CLOSE_BN);
	m_pkGuiBuilder->CreateOKButton(m_pkDlgBox, ID_SKINBOX_OK_BN);
	m_pkGuiBuilder->CreateCancelButton(m_pkDlgBox, ID_SKINBOX_CANCEL_BN, true);

	ZGuiListbox* pkTextureList = m_pkGuiBuilder->CreateListbox(m_pkDlgBox, 
		ID_SKINBOX_TEXTURELIST_LB, 0, 0, 250, 300); 
	pkTextureList->SetGUI(m_pkGui);
	m_pkGuiBuilder->Register(pkTextureList, "TexturesLB");

	FillTextureList();

	m_pkPreviewSkin = m_pkGuiBuilder->GetSkin("PreviewSkin");
	ZGuiLabel* pkPreview = m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, 290, 
		160, 64, 64, "", "PreviewSkin");

	typedef pair<string, string> name_label;
	typedef pair<int, name_label> controller;
	int id_start = ID_SKINTEXTURETYPE_RB;
	controller akCtrls[] = 
	{
		controller(id_start++, name_label("TexTypeBkRb","Background")),					// 0
		controller(id_start++, name_label("TexTypeHorzBorderRb","Horz.Border")),		// 1
		controller(id_start++, name_label("TexTypeVertBorderRb","Vert.Border")),		// 2
		controller(id_start++, name_label("TexTypeBorderCornerRb","Corner.Border")),	// 3
	};

	ZGuiWnd* pkRadioBn;

	w = 16; h = 16; x = 250; y = 20; 
	for(int i=0; i<sizeof(akCtrls) / sizeof(akCtrls[1]); i++)
	{
		pkRadioBn = m_pkGuiBuilder->CreateRadioButton(m_pkDlgBox,akCtrls[i].first,
			ID_SKINTEXTURETYPE_GROUP,x,y,w,h,(char*)akCtrls[i].second.second.c_str());
		m_pkGuiBuilder->Register(pkRadioBn, (char*)akCtrls[i].second.first.c_str());
		y += 20;
	}

	m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, 250, 0, 128, 20, "Texture type", "titlebar");
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, 250, 0, 128, 100, "", "BlackFrame");

	m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, 250, 100, 90, 20, "Border size:", "titlebar");
	m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, ID_SKINBORDERSIZE_EB, 340, 102, 38, 18, false);

	m_pkGuiBuilder->CreateCheckbox(m_pkDlgBox, ID_SKINTILEBORDER_CHB, 250, 125,
		16, 16, "Tile skin");

	float fThumbSize = (float) 10 / (float) 255;

	ZGuiScrollbar* pkScrollbar;
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox, ID_SKINBKREDCOLOR_SB, 
		250+140, 0, 20, 300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,255);
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox, ID_SKINBKGREENCOLOR_SB, 
		270+140, 0, 20, 300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,255);
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox, ID_SKINBKBLUECOLOR_SB, 
		290+140, 0, 20, 300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,255);

	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox, ID_SKINBORDERREDCOLOR_SB, 
		320+140, 0, 20, 300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,0);
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox, ID_SKINBORDERGREENCOLOR_SB, 
		340+140, 0, 20, 300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,0);
	pkScrollbar = m_pkGuiBuilder->CreateScrollbar(m_pkDlgBox, ID_SKINBORDERBLUECOLOR_SB, 
		360+140, 0, 20, 300); 
	pkScrollbar->SetGUI(m_pkGui);
	pkScrollbar->SetScrollInfo(0,255,fThumbSize,0);

	m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, 250, 257, 35, 18, "Bk.", "titlebar");
	m_pkGuiBuilder->CreateLabel(m_pkDlgBox, 0, 250, 280, 35, 18, "Bd.", "titlebar");

	m_pkGuiBuilder->Register(m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, 0, 285, 257, 
		31, 18, false), "BkRed");
	m_pkGuiBuilder->Register(m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, 0, 317, 257, 
		31, 18, false), "BkGreen");
	m_pkGuiBuilder->Register(m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, 0, 349, 257, 
		31, 18, false), "BkBlue");

	m_pkGuiBuilder->Register(m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, 0, 285, 280, 
		31, 18, false), "BdRed");
	m_pkGuiBuilder->Register(m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, 0, 317, 280, 
		31, 18, false), "BdGreen");
	m_pkGuiBuilder->Register(m_pkGuiBuilder->CreateTextbox(m_pkDlgBox, 0, 349, 280, 
		31, 18, false), "BdBlue");

	m_pkGui->ShowMainWindow(ID_SKINBOX_MAIN_WND, false);
	return true;
}

bool SkinBox::OnOpen(int x, int y)
{
	if(!(x==-1 && y==-1))
		m_pkDlgBox->SetPos(x, y, true, true);

	m_pkGui->ShowMainWindow(ID_SKINBOX_MAIN_WND, true);
	return true;
}

bool SkinBox::OnClose(bool bSave)
{
	m_pkGui->ShowMainWindow(ID_SKINBOX_MAIN_WND, false);
	return true;
}

void SkinBox::FillTextureList()
{
	ZGuiListbox* pkTextureList = (ZGuiListbox*) m_pkGuiBuilder->GetWnd("TexturesLB");

	vector<string> vkFiles;
	vector<string> vkFilters;
	vkFilters.push_back(".bmp"); 
	string strDir = "../data/textures";
	m_pkBasicFS->ListDirFilter(&vkFiles, vkFilters, strDir.c_str(), true);

	sort(vkFiles.begin(), vkFiles.end());

	for( unsigned int i=0; i<vkFiles.size(); i++)	
		pkTextureList->AddItem((char*)vkFiles[i].c_str(), i, false); 
}

void SkinBox::UpdatePreviewImage(char* strFileName)
{
	char szFile[256];
	sprintf(szFile, "file:../data/textures/%s", strFileName);

	m_pkPreviewSkin->m_iBkTexID = m_pkTexMan->Load(szFile, 0);
}

void SkinBox::SetSkinBkColor(int r, int g, int b)
{
	if(r > 0) m_pkPreviewSkin->m_afBkColor[0] = (float) (1.0f/255) * r;
	if(g > 0) m_pkPreviewSkin->m_afBkColor[1] = (float) (1.0f/255) * g;
	if(b > 0) m_pkPreviewSkin->m_afBkColor[2] = (float) (1.0f/255) * b;

	char szRed[5], szGreen[5], szBlue[5];
	
	sprintf(szRed, "%i", r);
	sprintf(szGreen, "%i", g);
	sprintf(szBlue, "%i", b);
	
	if(r > 0) m_pkGuiBuilder->GetWnd("BkRed")->SetText(szRed);
	if(g > 0) m_pkGuiBuilder->GetWnd("BkGreen")->SetText(szGreen);
	if(b > 0) m_pkGuiBuilder->GetWnd("BkBlue")->SetText(szBlue);
}

void SkinBox::SetSkinBorderColor(int r, int g, int b)
{
	if(r > 0) m_pkPreviewSkin->m_afBorderColor[0] = (float) (1.0f/255) * r;
	if(g > 0) m_pkPreviewSkin->m_afBorderColor[1] = (float) (1.0f/255) * g;
	if(b > 0) m_pkPreviewSkin->m_afBorderColor[2] = (float) (1.0f/255) * b;

	char szRed[5], szGreen[5], szBlue[5];
	
	sprintf(szRed, "%i", r);
	sprintf(szGreen, "%i", g);
	sprintf(szBlue, "%i", b);
	
	if(r > 0) m_pkGuiBuilder->GetWnd("BdRed")->SetText(szRed);
	if(g > 0) m_pkGuiBuilder->GetWnd("BdGreen")->SetText(szGreen);
	if(b > 0) m_pkGuiBuilder->GetWnd("BdBlue")->SetText(szBlue);
}



