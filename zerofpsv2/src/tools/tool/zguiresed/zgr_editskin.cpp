#include "zgr_app.h"

void ZGuiResEd::UpdatePreviewImage(const char* szFileName)
{	
	static ZGuiWnd* pkPreviewLabel = GetWnd("GuiEd_PreviewImageLabel");

	pkPreviewLabel->Hide();

	static string strPrevFile = "";

	if(szFileName == NULL || strPrevFile == szFileName)
		return;

	if(szFileName != NULL && strlen(szFileName) > 1)
	{
		if(pkPreviewLabel->GetSkin())
		{
			int id = m_pkTexMan->Load(szFileName,0);
			pkPreviewLabel->GetSkin()->m_iBkTexID = id;
			strPrevFile = szFileName;

			if(id > -1)
			{
				m_pkTexMan->BindTexture( id );
				m_pkTexMan->EditStart( id );

				Image* pkSurface = m_pkTexMan->EditGetImage( id );

				if(pkSurface)
				{
					float width = (float) pkSurface->m_iWidth;
					float height = (float) pkSurface->m_iHeight;

					char szInfo[50];
					sprintf(szInfo, "%ix%i", (int) width, (int) height);
					SetText("GuiEd_TextureSizeLabel", szInfo);

					if(width > height)
					{
						float mod = width / 92.0f;
						height = height / mod;
						width = 92.0f;
					}
					else
					{
						float mod = height / 92.0f;
						width = width / mod;
						height = 92.0f;
					}

					m_pkTexMan->EditEnd( id );

					pkPreviewLabel->Resize((int)width, (int)height);
					pkPreviewLabel->Show();					
				}
			}
		}
	}

}

void ZGuiResEd::AddFilesInFolderToListbox(const char* szFolderName)
{
	ZSSVFileSystem* m_pkFileSys = reinterpret_cast<ZSSVFileSystem*>(
		g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	

	vector<string> t;
	m_pkFileSys->ListDir(&t, szFolderName);

	for(int i=0; i<t.size(); i++) 
	{
		char *ext = strrchr( t[i].c_str(), '.');
		if(ext == NULL || strcmp(ext,".") == 0 || strcmp(ext,".bmp") == 0 || strcmp(ext,".tga") == 0) 
		{
			AddListItem("GuiEd_TextureList", (char*) t[i].c_str());
		}
	}
}

bool ZGuiResEd::UpdateTextureList()
{
	ClearListbox("GuiEd_TextureList");
	AddFilesInFolderToListbox(m_strCurrTexDir.c_str());
	return true;
}

bool ZGuiResEd::UpdateSkinList()
{
	ClearListbox("GuiEd_SkinElementsList");

	if(m_pkFocusWnd == NULL)
		return false;

	vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
	m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

	for(int i=0; i<vkSkinDesc.size(); i++)
		AddListItem("GuiEd_SkinElementsList", (char*) vkSkinDesc[i].second.c_str());

	return true;
}

void ZGuiResEd::SelNewSkin(int iIndex)
{
	string strRealName = "";

	int sel = 0;

	if(iIndex == -1)
	{
		if(((ZGuiListbox*)GetWnd("GuiEd_SkinElementsList"))->GetSelItem())
			sel = ((ZGuiListbox*)GetWnd("GuiEd_SkinElementsList"))->GetSelItem()->GetIndex();  
	}
	else
	{
		sel = iIndex;
		((ZGuiListbox*)GetWnd("GuiEd_SkinElementsList"))->SelItem(sel);
	}

	if(!m_pkFocusWnd)
	{
		SetText("GuiEd_BorderSize", "");
		SetText("GuiEd_ColorR", "");
		SetText("GuiEd_ColorG", "");
		SetText("GuiEd_ColorB", "");
		SetText("GuiEd_RotValue", "");
		CheckButton("GuiEd_Trans", false);
		CheckButton("GuiEd_Tile", false);
		SelListItem("GuiEd_TextureList", NULL);
	}

	ZGuiSkin** ppkSkin;
	if(GetSelSkin(ppkSkin))
	{
		int id;

		if(m_bAlphaTextureMode == false)
		{
			id = (*ppkSkin)->m_iBkTexID;

			if(IsButtonChecked("GuiEd_SelBorderHorz"))
				id = (*ppkSkin)->m_iHorzBorderTexID;
			if(IsButtonChecked("GuiEd_SelBorderVert"))
				id = (*ppkSkin)->m_iVertBorderTexID;
			if(IsButtonChecked("GuiEd_SelBorderCorner"))
				id = (*ppkSkin)->m_iBorderCornerTexID;
		}
		else
		{
			id = (*ppkSkin)->m_iBkTexAlphaID;

			if(IsButtonChecked("GuiEd_SelBorderHorz"))
				id = (*ppkSkin)->m_iHorzBorderTexAlphaID;
			if(IsButtonChecked("GuiEd_SelBorderVert"))
				id = (*ppkSkin)->m_iVertBorderTexAlphaID;
			if(IsButtonChecked("GuiEd_SelBorderCorner"))
				id = (*ppkSkin)->m_iBorderCornerTexAlphaID;
		}

		if(id > 0)
		{
			//string strName = m_pkTexMan->GetTextureNameFromOpenGlIndex(m_pkTexMan->GetTextureID(id));
			string strName = GetTexNameFromID(id);

			ZSSVFileSystem* m_pkFileSys = reinterpret_cast<ZSSVFileSystem*>(
					g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	

			// set name
			strRealName = m_pkFileSys->GetFullPath(strName.c_str());		

			// select in texture list	
			char *name = strrchr( strRealName.c_str(), '/');
			if(name)
			{
				name++; // m_strCurrTexDir

				string strPrevDir = m_strCurrTexDir;

				string strNewDirs = strRealName;
				int p = strNewDirs.find(name);
				if(p != string::npos)
				{
					strNewDirs.erase(p);
					int p2 = strNewDirs.find("/textures/gui/");
					if(p2 != string::npos)
					{
						p2 += strlen("/textures/gui/");
						strNewDirs.erase(0,p2);

						m_strCurrTexDir = string("data/textures/gui/") + strNewDirs;
						//SetText(g_kDlgBoxRight, IDC_CURRENT_PATH_EB, m_strCurrTexDir.c_str());
						
						if(strPrevDir != m_strCurrTexDir)
							UpdateTextureList();
					}
				}

				//if(LB_ERR == SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST,  LB_SELECTSTRING, -1, 
				//	(LPARAM) (LPCSTR) name))
				//{
				//	SendDlgItemMessage(g_kDlgBoxRight, IDC_TEXTURE_LIST, LB_SETCURSEL, -1, 0);
				//}

				SelListItem("GuiEd_TextureList", name);
			}
		}
		else
		{			
			SelListItem("GuiEd_TextureList", NULL);
		}

		char szText[50];

		sprintf(szText, "%i", (*ppkSkin)->m_unBorderSize);
		SetText("GuiEd_BorderSize", szText);

		if(IsButtonChecked("GuiEd_SelBackgroundTexture"))
		{
			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBkColor[0]*255));
			SetText("GuiEd_ColorR", szText);

			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBkColor[1]*255));
			SetText("GuiEd_ColorG", szText);

			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBkColor[2]*255));
			SetText("GuiEd_ColorB", szText);
		}
		else
		{
			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBorderColor[0]*255));
			SetText("GuiEd_ColorR", szText);

			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBorderColor[1]*255));
			SetText("GuiEd_ColorG", szText);

			sprintf(szText, "%i", (int)((*ppkSkin)->m_afBorderColor[2]*255));
			SetText("GuiEd_ColorB", szText);
		}

		CheckButton("GuiEd_Trans", (*ppkSkin)->m_bTransparent);
		CheckButton("GuiEd_Tile", (*ppkSkin)->m_bTileBkSkin);

		float procent_av_max = (*ppkSkin)->m_fRotDegree / (PI+PI);
		procent_av_max *= 360.0f;

		sprintf(szText, "%i", (int)procent_av_max);
		SetText("GuiEd_RotValue", szText);
		
		RedrawCtrlSkin();
	}

	UpdatePreviewImage(strRealName.c_str());

	
}

void ZGuiResEd::SetTexture(bool bSet)
{
	if(char* szTexName = GetSelItem("GuiEd_TextureList"))
	{
		ZGuiSkin** ppkSkin;
		if(GetSelSkin(ppkSkin))
		{
			string strFileName;

			if(bSet)
				strFileName = m_strCurrTexDir + string("/") + string(szTexName);

			ZGuiSkin* pSkin = *ppkSkin;

			if(m_bAlphaTextureMode == false)
			{
				if(IsButtonChecked("GuiEd_SelBackgroundTexture"))
					pSkin->m_iBkTexID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				else
				if(IsButtonChecked("GuiEd_SelBorderHorz"))
					pSkin->m_iHorzBorderTexID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				else
				if(IsButtonChecked("GuiEd_SelBorderVert"))
					pSkin->m_iVertBorderTexID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				else
				if(IsButtonChecked("GuiEd_SelBorderCorner"))
					pSkin->m_iBorderCornerTexID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;				
			}
			else
			{
				if(IsButtonChecked("GuiEd_SelBackgroundTexture"))
					pSkin->m_iBkTexAlphaID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				else
				if(IsButtonChecked("GuiEd_SelBorderHorz"))
					pSkin->m_iHorzBorderTexAlphaID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				else
				if(IsButtonChecked("GuiEd_SelBorderVert"))
					pSkin->m_iVertBorderTexAlphaID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
				else
				if(IsButtonChecked("GuiEd_SelBorderCorner"))
					pSkin->m_iBorderCornerTexAlphaID = bSet ? m_pkTexMan->Load(strFileName.c_str()) : -1;
			}
		}
	}

	UpdateWndSkins();
}

void ZGuiResEd::UpdateWndSkins()
{	
	//if(m_pkFocusWnd == NULL)
	//	return;

	//GuiType eType = GetWndType(m_pkFocusWnd);

	//if(eType == Button)
	//	m_pkFocusWnd->m_pkSkin = ((ZGuiButton*) m_pkFocusWnd)->();
}


bool ZGuiResEd::GetSelSkin(ZGuiSkin**& ppkSkin)
{
	if(char* item = GetSelItem("GuiEd_SkinElementsList"))
	{
		if(m_pkFocusWnd)
		{
			vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
			m_pkFocusWnd->GetWndSkinsDesc(vkSkinDesc);

			for(int i=0; i<vkSkinDesc.size(); i++)
			{
				if(vkSkinDesc[i].second == string(item))
				{
					ppkSkin = vkSkinDesc[i].first;
					return true;
				}
			}
		}
	}


	return false;
}

void ZGuiResEd::ResizeWndToImage()
{
	if(char* szTexName = GetSelItem("GuiEd_TextureList"))
	{
		string strName = m_strCurrTexDir + string("/") + string(szTexName);

		ZSSVFileSystem* m_pkFileSys = reinterpret_cast<ZSSVFileSystem*>(
				g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));	

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

string ZGuiResEd::GetTexNameFromID(int id, bool bFullName) 
{ 
	if(bFullName) 
		return m_pkTexMan->GetTextureNameFromOpenGlIndex(m_pkTexMan->GetTextureID(id)); 
	else
	{
		string strRemoveString = "gui/"; // "data/textures/gui"
		string strFullName = m_pkTexMan->GetTextureNameFromOpenGlIndex(m_pkTexMan->GetTextureID(id));
		int pos = strFullName.find(strRemoveString);
		if(pos != string::npos)
			strFullName.erase(pos, strlen(strRemoveString.c_str()));
		else
		if((pos = strFullName.find(strRemoveString)) != string::npos)
			strFullName.erase(pos, strlen(strRemoveString.c_str()));
		else
		if((pos = strFullName.find(strRemoveString)) != string::npos)
			strFullName.erase(pos, strlen(strRemoveString.c_str()));
		else
		if((pos = strFullName.find(strRemoveString)) != string::npos)
			strFullName.erase(pos, strlen(strRemoveString.c_str()));

		return strFullName;
	}
}

void ZGuiResEd::RedrawCtrlSkin()
{
	char* szItem;
	if((szItem = GetSelItem("GuiEd_SkinElementsList")) == NULL)
		return;

	string strText = szItem;

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

bool ZGuiResEd::CloneSkins(ZGuiWnd* pkDst, ZGuiWnd* pkSrc)
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

void ZGuiResEd::OnChangeSkinColor()
{
	float fColor;
	ZGuiSkin** ppkSkin;
	if(GetSelSkin(ppkSkin))
	{
		char text[64];

		strcpy(text,GetText("GuiEd_ColorR"));
		fColor = (float) atoi(text) / 255.0f;
		if(IsButtonChecked("GuiEd_SelBackgroundTexture"))
			(*ppkSkin)->m_afBkColor[0] = fColor;
		else
			(*ppkSkin)->m_afBorderColor[0] = fColor;
	
		strcpy(text,GetText("GuiEd_ColorG"));
		fColor = (float) atoi(text) / 255.0f;
		if(IsButtonChecked("GuiEd_SelBackgroundTexture"))
			(*ppkSkin)->m_afBkColor[1] = fColor;
		else
			(*ppkSkin)->m_afBorderColor[1] = fColor;

		strcpy(text,GetText("GuiEd_ColorB"));
		fColor = (float) atoi(text) / 255.0f;
		if(IsButtonChecked("GuiEd_SelBackgroundTexture"))
			(*ppkSkin)->m_afBkColor[2] = fColor;
		else
			(*ppkSkin)->m_afBorderColor[2] = fColor;
	}
}




