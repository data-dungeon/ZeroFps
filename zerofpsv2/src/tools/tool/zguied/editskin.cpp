#include "zguied.h"
#include "resource.h"

void ZGuiEd::UpdatePreviewImage(const char* szFileName)
{		
	static string strPrevFile = "";

	if(strPrevFile == szFileName)
		return;

	strPrevFile = szFileName;

	DeleteObject(preview_bitmap);

	Image kImage;
	if(kImage.load(szFileName))
	{
		char *ext = strrchr( szFileName, '.');
		if(strcmp(ext,".bmp") == 0) 
			kImage.Flip(false,true);
		
		unsigned long num_bytes = 3*kImage.m_iWidth*kImage.m_iHeight;
		unsigned char* bytes = new unsigned char[num_bytes];

		BITMAPINFOHEADER	bmih;									
		bmih.biSize = sizeof (BITMAPINFOHEADER);			
		bmih.biPlanes = 1;										
		bmih.biBitCount = 24;									
		bmih.biWidth = kImage.m_iWidth;									
		bmih.biHeight = kImage.m_iHeight;									
		bmih.biCompression = BI_RGB;		

		HDC hScreenDC = GetDC(g_kDlgBoxRight);

		preview_bitmap = CreateDIBSection(hScreenDC, (BITMAPINFO*)(&bmih), 
			DIB_RGB_COLORS, (void**)(&bytes), NULL, NULL );
	
		int offset = 0, pixel = 0;

		for(int y=0; y<kImage.m_iHeight; y++)
			for(int x=0; x<kImage.m_iWidth; x++)
			{
				bytes[offset++]	= kImage.m_pkPixels[pixel].b;		// blue
				bytes[offset++]	= kImage.m_pkPixels[pixel].g;		// green
				bytes[offset++]	= kImage.m_pkPixels[pixel++].r;		// red
			}

		SetBitmapDimensionEx(preview_bitmap, kImage.m_iWidth, kImage.m_iHeight, NULL);
	}

	RECT rc = {0,768-128-8,128+8,768};	
	InvalidateRect(g_kDlgBoxRight, &rc, true);
	UpdateWindow(g_kDlgBoxRight);
}

void ZGuiEd::AddFilesInFolderToListbox(const char* szFolderName)
{
	ZFVFileSystem* m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

	vector<string> t;
	m_pkFileSys->ListDir(&t, szFolderName);

	for(int i=0; i<t.size(); i++) 
	{
		char *ext = strrchr( t[i].c_str(), '.');
		if(ext == NULL || strcmp(ext,".") == 0 || strcmp(ext,".bmp") == 0 || strcmp(ext,".tga") == 0) 
		{
			SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_ADDSTRING,  
				0, (LPARAM) (LPCSTR) t[i].c_str() );
		}
	}
}


bool ZGuiEd::UpdateTextureList()
{
	if(!GetDlgItem(g_kDlgBoxRight, IDC_TEXTURE_LIST))
		return false;

	SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_RESETCONTENT, 0, 0);

	AddFilesInFolderToListbox(m_strCurrTexDir.c_str());

	return true;
}

bool ZGuiEd::UpdateSkinList()
{
	if(!GetDlgItem(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST))
		return false;

	SendDlgItemMessage(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST, LB_RESETCONTENT, 0, 0);

	if(m_pkFocusWnd == NULL)
		return false;

	vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
	m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

	for(int i=0; i<vkSkinDesc.size(); i++)
		SendDlgItemMessage(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST, LB_ADDSTRING, 0, 
			(LPARAM) (LPCSTR) vkSkinDesc[i].second.c_str() );

	return true;
}

void ZGuiEd::SelNewSkin(int iIndex)
{
	string strRealName = "";

	int sel = 0;

	if(iIndex == -1)
		sel = SendDlgItemMessage(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST, LB_GETCURSEL, 0, 0);
	else
	{
		sel = iIndex;
		sel = SendDlgItemMessage(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST, LB_SETCURSEL, sel, 0);
	}

	if(!m_pkFocusWnd)
	{
		SetDlgItemText(g_kDlgBoxBottom, IDC_BORDER_EB, "");
		SetDlgItemText(g_kDlgBoxBottom, IDC_BORDER_EB, "");
		SetDlgItemText(g_kDlgBoxBottom, IDC_RGB_COLOR_R_EB, "");
		SetDlgItemText(g_kDlgBoxBottom, IDC_RGB_COLOR_G_EB, "");
		SetDlgItemText(g_kDlgBoxBottom, IDC_RGB_COLOR_B_EB, "");
		SetDlgItemText(g_kDlgBoxBottom, IDC_ROTATION_EB, "");
		CheckDlgButton(g_kDlgBoxBottom, IDC_TRANSPARENT_CB, BST_UNCHECKED);
		CheckDlgButton(g_kDlgBoxBottom, IDC_TILE_CB, BST_UNCHECKED);
	}

	ZGuiSkin** ppkSkin;
	if(GetSelSkin(ppkSkin))
	{
		int id = (*ppkSkin)->m_iBkTexID;

		if(IsDlgButtonChecked(g_kDlgBoxBottom, IDC_SKINTYPE_HORZBORDER_RB))
			id = (*ppkSkin)->m_iHorzBorderTexID;
		if(IsDlgButtonChecked(g_kDlgBoxBottom, IDC_SKINTYPE_VERTBORDER_RB))
			id = (*ppkSkin)->m_iVertBorderTexID;
		if(IsDlgButtonChecked(g_kDlgBoxBottom, IDC_SKINTYPE_CORNERBORDER_RB))
			id = (*ppkSkin)->m_iBorderCornerTexID;

		if(id > 0)
		{
			//string strName = m_pkTexMan->GetTextureNameFromOpenGlIndex(m_pkTexMan->GetTextureID(id));
			string strName = GetTexNameFromID(id);

			ZFVFileSystem* m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
					g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

			// set name
			strRealName = m_pkFileSys->GetFullPath(strName.c_str());		

			// select in texture list
			char *name = strrchr( strRealName.c_str(), '/');
			if(name)
			{
				name++;
				SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST,  LB_SELECTSTRING, -1, 
					(LPARAM) (LPCSTR) name);
			}
		}

		char szText[50];

		sprintf(szText, "%i", (*ppkSkin)->m_unBorderSize);
		SetDlgItemText(g_kDlgBoxBottom, IDC_BORDER_EB, szText);

		if(IsDlgButtonChecked(g_kDlgBoxBottom, IDC_SKINTYPE_BACKGROUND_RB))
		{
			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBkColor[0]*255));
			SetDlgItemText(g_kDlgBoxBottom, IDC_RGB_COLOR_R_EB, szText);

			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBkColor[1]*255));
			SetDlgItemText(g_kDlgBoxBottom, IDC_RGB_COLOR_G_EB, szText);

			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBkColor[2]*255));
			SetDlgItemText(g_kDlgBoxBottom, IDC_RGB_COLOR_B_EB, szText);
		}
		else
		{
			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBorderColor[0]*255));
			SetDlgItemText(g_kDlgBoxBottom, IDC_RGB_COLOR_R_EB, szText);

			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBorderColor[1]*255));
			SetDlgItemText(g_kDlgBoxBottom, IDC_RGB_COLOR_G_EB, szText);

			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBorderColor[2]*255));
			SetDlgItemText(g_kDlgBoxBottom, IDC_RGB_COLOR_B_EB, szText);
		}


		CheckDlgButton(g_kDlgBoxBottom, IDC_TRANSPARENT_CB, 
			(*ppkSkin)->m_bTransparent ? BST_CHECKED : BST_UNCHECKED);

		CheckDlgButton(g_kDlgBoxBottom, IDC_TILE_CB, 
			(*ppkSkin)->m_bTileBkSkin ? BST_CHECKED : BST_UNCHECKED);

		float procent_av_max = (*ppkSkin)->m_fRotDegree / (PI+PI);
		procent_av_max *= 360.0f;

		sprintf(szText, "%i", (int)procent_av_max);
		SetDlgItemText(g_kDlgBoxBottom, IDC_ROTATION_EB, szText);
		
		RedrawCtrlSkin();
	}

	UpdatePreviewImage(strRealName.c_str());

	
}

void ZGuiEd::SetTexture(bool bSet)
{
	int sel = SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_GETCURSEL, 0, 0);
	if(sel != LB_ERR || bSet == false)
	{	
		char szTexName[200];

		if(SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_GETTEXT, sel, 
			(LPARAM) (LPCSTR) szTexName) != LB_ERR || bSet == false)
		{
			ZGuiSkin** ppkSkin;
			if(GetSelSkin(ppkSkin))
			{
				string strFileName;

				if(bSet)
					strFileName = m_strCurrTexDir + string("/") + string(szTexName);

				ZGuiSkin* pSkin = *ppkSkin;
				if(IsDlgButtonChecked(g_kDlgBoxBottom, IDC_SKINTYPE_BACKGROUND_RB))
					pSkin->m_iBkTexID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				else
				if(IsDlgButtonChecked(g_kDlgBoxBottom, IDC_SKINTYPE_HORZBORDER_RB))
					pSkin->m_iHorzBorderTexID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				else
				if(IsDlgButtonChecked(g_kDlgBoxBottom, IDC_SKINTYPE_VERTBORDER_RB))
					pSkin->m_iVertBorderTexID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				else
				if(IsDlgButtonChecked(g_kDlgBoxBottom, IDC_SKINTYPE_CORNERBORDER_RB))
					pSkin->m_iBorderCornerTexID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				
			}
		}
	}

	UpdateWndSkins();
}

void ZGuiEd::UpdateWndSkins()
{	
	//if(m_pkFocusWnd == NULL)
	//	return;

	//GuiType eType = GetWndType(m_pkFocusWnd);

	//if(eType == Button)
	//	m_pkFocusWnd->m_pkSkin = ((ZGuiButton*) m_pkFocusWnd)->();
}


bool ZGuiEd::GetSelSkin(ZGuiSkin**& ppkSkin)
{
	int sel = SendDlgItemMessage(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST, LB_GETCURSEL, 0, 0);
	if(sel != LB_ERR)
	{
		char szWndName[200];
		if(SendDlgItemMessage(g_kDlgBoxBottom, IDC_SKINELEMENTS_LIST, LB_GETTEXT, sel, 
			(LPARAM) (LPCSTR) szWndName) != LB_ERR)
		{
			if(m_pkFocusWnd)
			{
				vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
				m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

				for(int i=0; i<vkSkinDesc.size(); i++)
				{
					if(vkSkinDesc[i].second == string(szWndName))
					{
						ppkSkin = vkSkinDesc[i].first;
						return true;
					}
				}
			}
		}
	}

	return false;
}

void ZGuiEd::ResizeWndToImage()
{
	int sel = SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_GETCURSEL, 0, 0);
	if(sel != LB_ERR)
	{
		char szTexName[200];
		if(SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_GETTEXT, sel, 
			(LPARAM) (LPCSTR) szTexName) != LB_ERR)
		{
			string strName = m_strCurrTexDir + string("/") + string(szTexName);

			ZFVFileSystem* m_pkFileSys = reinterpret_cast<ZFVFileSystem*>(
					g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));	

			string strRealName = m_pkFileSys->GetFullPath(strName.c_str());

			Image kImage;
			if(kImage.load(strRealName.c_str()))
			{
				if(m_pkFocusWnd)
				{
					m_pkFocusWnd->Resize(kImage.m_iWidth, kImage.m_iHeight);
					CheckMovement();
				}
			}
		}
	}
}

string ZGuiEd::GetTexNameFromID(int id, bool bFullName) 
{ 
	if(bFullName) 
		return m_pkTexMan->GetTextureNameFromOpenGlIndex(m_pkTexMan->GetTextureID(id)); 
	else
	{
		string strFullName = m_pkTexMan->GetTextureNameFromOpenGlIndex(m_pkTexMan->GetTextureID(id));
		int pos = strFullName.find("/data/textures/gui/");
		if(pos != string::npos)
			strFullName.erase(pos, strlen("/data/textures/gui/"));
		else
		if((pos = strFullName.find("/data/textures/gui")) != string::npos)
			strFullName.erase(pos, strlen("/data/textures/gui"));
		else
		if((pos = strFullName.find("data/textures/gui/")) != string::npos)
			strFullName.erase(pos, strlen("data/textures/gui/"));
		else
		if((pos = strFullName.find("data/textures/gui")) != string::npos)
			strFullName.erase(pos, strlen("data/textures/gui"));

		return strFullName;
	}
}

void ZGuiEd::RedrawCtrlSkin()
{
	string strText = GetSelItemText(IDC_SKINELEMENTS_LIST, false);
	if(strText == "")
		return;

	// Button
	if(strText == "Button up") 
		m_pkFocusWnd->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Button down") 
		m_pkFocusWnd->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Button focus") 
		m_pkFocusWnd->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);

	// Checkbox
	if(strText == "Checkbox: Button up") 
		((ZGuiCheckbox*)m_pkFocusWnd)->UncheckButton();
	if(strText == "Checkbox: Button down") 
		((ZGuiCheckbox*)m_pkFocusWnd)->CheckButton();

	// Radiobutton
	if(strText == "Radiobutton: Checkbox: Button up") 
		((ZGuiRadiobutton*)m_pkFocusWnd)->GetButton()->UncheckButton();
	if(strText == "Radiobutton: Checkbox: Button down") 
		((ZGuiRadiobutton*)m_pkFocusWnd)->Check();

	// Scrollbar
	if(strText == "Scrollbar: Button up") 
		((ZGuiWnd*)((ZGuiScrollbar*) m_pkFocusWnd)->GetButton())->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Scrollbar: Button down") 
		((ZGuiWnd*)((ZGuiScrollbar*) m_pkFocusWnd)->GetButton())->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Scrollbar: Button focus") 
		((ZGuiWnd*)((ZGuiScrollbar*) m_pkFocusWnd)->GetButton())->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);
	if(strText == "Scrollbar: Top: Button up") 
		((ZGuiWnd*)((ZGuiScrollbar*) m_pkFocusWnd)->GetArrowButton(true))->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Scrollbar: Top: Button down") 
		((ZGuiWnd*)((ZGuiScrollbar*) m_pkFocusWnd)->GetArrowButton(true))->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Scrollbar: Top: Button focus") 
		((ZGuiWnd*)((ZGuiScrollbar*) m_pkFocusWnd)->GetArrowButton(true))->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);
	if(strText == "Scrollbar: Bottom: Button up") 
		((ZGuiWnd*)((ZGuiScrollbar*) m_pkFocusWnd)->GetArrowButton(false))->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Scrollbar: Bottom: Button down") 
		((ZGuiWnd*)((ZGuiScrollbar*) m_pkFocusWnd)->GetArrowButton(false))->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Scrollbar: Bottom: Button focus") 
		((ZGuiWnd*)((ZGuiScrollbar*) m_pkFocusWnd)->GetArrowButton(false))->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);

	// Listbox
	if(strText == "Listbox: Scrollbar: Button up") 
		((ZGuiWnd*)((ZGuiListbox*) m_pkFocusWnd)->GetScrollbar()->GetButton())->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Listbox: Scrollbar: Button down") 
		((ZGuiWnd*)((ZGuiListbox*) m_pkFocusWnd)->GetScrollbar()->GetButton())->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Listbox: Scrollbar: Button focus") 
		((ZGuiWnd*)((ZGuiListbox*) m_pkFocusWnd)->GetScrollbar()->GetButton())->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);
	if(strText == "Listbox: Scrollbar: Top: Button up") 
		((ZGuiWnd*)((ZGuiListbox*) m_pkFocusWnd)->GetScrollbar()->GetArrowButton(true))->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Listbox: Scrollbar: Top: Button down") 
		((ZGuiWnd*)((ZGuiListbox*) m_pkFocusWnd)->GetScrollbar()->GetArrowButton(true))->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Listbox: Scrollbar: Top: Button focus") 
		((ZGuiWnd*)((ZGuiListbox*) m_pkFocusWnd)->GetScrollbar()->GetArrowButton(true))->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);
	if(strText == "Listbox: Scrollbar: Bottom: Button up") 
		((ZGuiWnd*)((ZGuiListbox*) m_pkFocusWnd)->GetScrollbar()->GetArrowButton(false))->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Listbox: Scrollbar: Bottom: Button down") 
		((ZGuiWnd*)((ZGuiListbox*) m_pkFocusWnd)->GetScrollbar()->GetArrowButton(false))->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Listbox: Scrollbar: Bottom: Button focus") 
		((ZGuiWnd*)((ZGuiListbox*) m_pkFocusWnd)->GetScrollbar()->GetArrowButton(false))->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);
	//
	//// Combobox


	// Slider
	if(strText == "Slider: Label") 
		((ZGuiSlider*) m_pkFocusWnd)->SetBkSkin( ((ZGuiSlider*) m_pkFocusWnd)->GetBkSkin() );
	if(strText == "Slider: Button up") 
		((ZGuiWnd*)((ZGuiSlider*) m_pkFocusWnd)->GetButton())->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Slider: Button down") 
		((ZGuiWnd*)((ZGuiSlider*) m_pkFocusWnd)->GetButton())->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Slider: Button focus") 
		((ZGuiWnd*)((ZGuiSlider*) m_pkFocusWnd)->GetButton())->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);

	// Progressbar
	if(strText == "Progressbar: Back")
	{
		vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
		m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

		ZGuiSkin* pkFront=NULL, *pkBack=NULL;

		for(int i=0; i<vkSkinDesc.size(); i++) 
		{
			if(vkSkinDesc[i].second == "Progressbar: Back")
				pkBack = (*vkSkinDesc[i].first);
			if(vkSkinDesc[i].second == "Progressbar: Front")
				pkFront = (*vkSkinDesc[i].first);
		}
		
		((ZGuiProgressbar*)m_pkFocusWnd)->SetSkin(pkFront, pkBack);
	}

	// Tabcontrol
	if(strText == "Tabctrl: prevtab: Button up") 
		((ZGuiWnd*)((ZGuiTabCtrl*) m_pkFocusWnd)->GetTabButton(false))->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Tabctrl: prevtab: Button down") 
		((ZGuiWnd*)((ZGuiTabCtrl*) m_pkFocusWnd)->GetTabButton(false))->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Tabctrl: prevtab: Button down") 
		((ZGuiWnd*)((ZGuiTabCtrl*) m_pkFocusWnd)->GetTabButton(false))->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);
	if(strText == "Tabctrl: nexttab: Button up") 
		((ZGuiWnd*)((ZGuiTabCtrl*) m_pkFocusWnd)->GetTabButton(true))->Notify(m_pkFocusWnd, NCODE_CLICK_UP);
	if(strText == "Tabctrl: nexttab: Button down") 
		((ZGuiWnd*)((ZGuiTabCtrl*) m_pkFocusWnd)->GetTabButton(true))->Notify(m_pkFocusWnd, NCODE_CLICK_DOWN);
	if(strText == "Tabctrl: nexttab: Button down") 
		((ZGuiWnd*)((ZGuiTabCtrl*) m_pkFocusWnd)->GetTabButton(true))->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);


	//if(strText == "Tabctrl: UnSeltab") 
	//	((ZGuiWnd*)((ZGuiTabCtrl*) m_pkFocusWnd)->GetTabButton(true))->Notify(m_pkFocusWnd, NCODE_OVER_CTRL);
		

}

bool ZGuiEd::CloneSkins(ZGuiWnd* pkDst, ZGuiWnd* pkSrc)
{
	if(pkSrc == NULL || pkDst == NULL)
		return false;

	if(GetWndType(pkSrc) != GetWndType(pkDst))
		return false;

	vector<ZGuiWnd::SKIN_DESC> vkSrcSkinDesc, vkDstSkinDesc;
	pkSrc->GetWndSkinsDesc(vkSrcSkinDesc);
	pkDst->GetWndSkinsDesc(vkDstSkinDesc);

	for(int i=0; i<vkSrcSkinDesc.size(); i++)
	{
		*(*vkDstSkinDesc[i].first) = *(*vkSrcSkinDesc[i].first);
	}

	return true;
}