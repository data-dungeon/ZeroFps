#include "zgr_app.h"

bool ZGuiResEd::CreateBackup(const char* szFileName)
{
	string strBackupName = szFileName;

	char *ext = strrchr( strBackupName.c_str(), '.');
	if(ext != NULL && strcmp(ext, ".lua")==0)
	{			
		strBackupName.erase(strBackupName.find(".lua"), 4);
		strBackupName = strBackupName + ".bak";

		int bk_number = 1;

		while(1)
		{
			char number[10];
			sprintf(number, "%i", bk_number++);
			string test = strBackupName + number;					
			if(!m_pkBasicFS->FileExist(test.c_str()))
			{
				strBackupName = test;
				break;
			}
		}

#ifdef WIN
		if(!CopyFile(szFileName, strBackupName.c_str(), TRUE))
			return false;
#endif

	}
	else
	{
		return false;
	}

	return true;
}

bool ZGuiResEd::SaveScript(const char* szFileName, bool bConfirmOverwrite, bool bCreateBackUp)
{
	if(bConfirmOverwrite && m_pkBasicFS->FileExist(szFileName))
	{
		MsgBox("This file exist, overwrite?", "Confirm Save Script", MSGBOX_YESNO);
		return false;
	}

	if(bCreateBackUp && m_pkBasicFS->FileExist(szFileName))
	{
		if(!CreateBackup(szFileName))
			printf("Failed to create backup file!\n");
	}

	m_pkSaveFile = fopen(szFileName, "wt");
	if(m_pkSaveFile == NULL)
	{
		printf("Failed to save script =\n%s\n", szFileName);
		return false;
	}

	m_kSkinTable.clear();

	fprintf(m_pkSaveFile, "function GUICreate()\n\n");

	fprintf(m_pkSaveFile, "\tSetDesignResolution(%i, %i)\n", 
		m_iDesignResX, m_iDesignResY);

	printf("writing skins\n");
	if(!WriteSkins())
		return false;

	printf("writing windows\n");
	if(!WriteWindows())
		return false;

	printf("writing special properties\n");
	if(!WriteSpecialProperties())
		return false;
	
	fprintf(m_pkSaveFile, "end\n");

	fclose(m_pkSaveFile);

	return true;
}

bool ZGuiResEd::WriteSkins()
{
	fprintf(m_pkSaveFile, "\n\t-- Skinlist ---------------------------------------------------------" \
		"--------------------------------------------------------------\n\n");

	ZGuiWnd* pkMainWnd = GetWnd("GuiMainWnd");

	vector<ZGuiSkin*> kAddVector;
	int iCounter = 1;
	map<string, ZGuiWnd*> kWindows;
	m_pkGuiMan->GetWindows(kWindows);
	for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
	{
		ZGuiWnd* pkWnd = it->second;

		if(pkWnd == pkMainWnd || IsEditorWnd(pkWnd) )
			continue;

		vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
		pkWnd->GetWndSkinsDesc(vkSkinDesc);

		for(int i=0; i<vkSkinDesc.size(); i++)
		{
			bool bExist = false;
			ZGuiSkin* pkSkin = (*vkSkinDesc[i].first);		
						
			for(int j=0; j<kAddVector.size(); j++)
			{			
				if((*kAddVector[j]) == (*pkSkin))
				{
					bExist = true;
					break;
				}
			}

			if(!bExist)
			{
				bool bWriteColon = false;
				fprintf(m_pkSaveFile, "\tSkin%i\t= { ", iCounter);
				if(pkSkin->m_iBkTexID != -1)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "tex1=\"%s\"", GetTexNameFromID(pkSkin->m_iBkTexID, false).c_str());
				}
				if(pkSkin->m_iHorzBorderTexID != -1)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "tex2=\"%s\"", GetTexNameFromID(pkSkin->m_iHorzBorderTexID, false).c_str());
				}
				if(pkSkin->m_iVertBorderTexID != -1)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "tex3=\"%s\"", GetTexNameFromID(pkSkin->m_iVertBorderTexID, false).c_str());
				}
				if(pkSkin->m_iBorderCornerTexID != -1)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "tex4=\"%s\"", GetTexNameFromID(pkSkin->m_iBorderCornerTexID, false).c_str());
				}
				if(pkSkin->m_iBkTexAlphaID != -1)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "tex1a=\"%s\"", GetTexNameFromID(pkSkin->m_iBkTexAlphaID, false).c_str());
				}
				if(pkSkin->m_iHorzBorderTexAlphaID != -1)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "tex2a=\"%s\"", GetTexNameFromID(pkSkin->m_iHorzBorderTexAlphaID, false).c_str());
				}
				if(pkSkin->m_iVertBorderTexAlphaID != -1)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "tex3a=\"%s\"", GetTexNameFromID(pkSkin->m_iVertBorderTexAlphaID, false).c_str());
				}
				if(pkSkin->m_iBorderCornerTexAlphaID != -1)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "tex4a=\"%s\"", GetTexNameFromID(pkSkin->m_iBorderCornerTexAlphaID, false).c_str());
				}
				if(pkSkin->m_afBkColor[0] != 1.0f || pkSkin->m_afBkColor[1] != 1.0f || pkSkin->m_afBkColor[2] != 1.0f )
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "bkR=%i", (int)(pkSkin->m_afBkColor[0] * 255.0f));					

					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "bkG=%i", (int)(pkSkin->m_afBkColor[1] * 255.0f));					

					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "bkB=%i", (int)(pkSkin->m_afBkColor[2] * 255.0f));					
				}
				if( pkSkin->m_afBorderColor[0] != 1.0f || pkSkin->m_afBorderColor[1] != 1.0f || pkSkin->m_afBorderColor[2] != 1.0f )
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "borderR=%i", (int)(pkSkin->m_afBorderColor[0] * 255.0f));					

					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "borderG=%i", (int)(pkSkin->m_afBorderColor[1] * 255.0f));					

					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "borderB=%i", (int)(pkSkin->m_afBorderColor[2] * 255.0f));					
				}
				if( pkSkin->m_unBorderSize > 0)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "bd_size=%i", (int)(pkSkin->m_unBorderSize));					
				}
				if( pkSkin->m_bTileBkSkin == true)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "tile=%i", 1);					
				}
				if( pkSkin->m_bTransparent == true)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					fprintf(m_pkSaveFile, "trans=%i", 1);					
				}
				if( pkSkin->m_fRotDegree > 0)
				{
					if(bWriteColon) fprintf(m_pkSaveFile, ", "); bWriteColon = true;
					float procent_av_max = pkSkin->m_fRotDegree / Math::TWO_PI;	//(PI+PI)
					procent_av_max *= 360.0f;
					fprintf(m_pkSaveFile, "degree=%i", (int) procent_av_max);					
				}

				if(bWriteColon == false) // should never happen... but just in case.
				{					
					fprintf(m_pkSaveFile, "bkR=255,bkG=255,bkB=255");					
				}

				fprintf(m_pkSaveFile, " }\n");

				kAddVector.push_back(pkSkin);

				char name[50];
				sprintf(name, "Skin%i", iCounter++);
				m_kSkinTable.push_back( pair<ZGuiSkin, string>( *pkSkin, string(name) ));
			}
		}		
	}	

	return true;
}

bool ZGuiResEd::WriteWindows()
{
	fprintf(m_pkSaveFile, "\n\t-- Windowlist ---------------------------------------------------------" \
		"--------------------------------------------------------------\n\n");

	fprintf(m_pkSaveFile, "\t--Window Types\n\n");
	fprintf(m_pkSaveFile, "\tlocal Wnd         = 0\n");	
	fprintf(m_pkSaveFile, "\tlocal Button      = 1\n");	
	fprintf(m_pkSaveFile, "\tlocal Checkbox    = 2\n");
	fprintf(m_pkSaveFile, "\tlocal Combobox    = 3\n");
	fprintf(m_pkSaveFile, "\tlocal Label       = 4\n");
	fprintf(m_pkSaveFile, "\tlocal Listbox     = 5\n");
	fprintf(m_pkSaveFile, "\tlocal Radiobutton = 6\n");
	fprintf(m_pkSaveFile, "\tlocal Scrollbar   = 7\n");
	fprintf(m_pkSaveFile, "\tlocal Slider      = 8\n");
	fprintf(m_pkSaveFile, "\tlocal TabControl  = 9\n");
	fprintf(m_pkSaveFile, "\tlocal Textbox     = 10\n");
	fprintf(m_pkSaveFile, "\tlocal Treebox     = 11\n");
	fprintf(m_pkSaveFile, "\tlocal Menu        = 12\n");
	fprintf(m_pkSaveFile, "\tlocal Progressbar = 13\n\n");

	fprintf(m_pkSaveFile, "\t--Window Alignent Types\n\n");
	fprintf(m_pkSaveFile, "\tlocal TopLeft      = 0\n");
	fprintf(m_pkSaveFile, "\tlocal TopRight     = 1\n");
	fprintf(m_pkSaveFile, "\tlocal BottomLeft   = 2\n");
	fprintf(m_pkSaveFile, "\tlocal BottomRight  = 3\n");
	fprintf(m_pkSaveFile, "\tlocal CenterHorz   = 4\n");
	fprintf(m_pkSaveFile, "\tlocal CenterVert   = 5\n");
	fprintf(m_pkSaveFile, "\tlocal Center       = 6\n\n");

	fprintf(m_pkSaveFile, "\t--Window Resize Type\n\n");
	fprintf(m_pkSaveFile, "\tlocal eNone        = 0\n");
	fprintf(m_pkSaveFile, "\tlocal ResizeWidth  = 1\n");
	fprintf(m_pkSaveFile, "\tlocal ResizeHeight = 2\n");
	fprintf(m_pkSaveFile, "\tlocal Resize       = 3\n\n");
	
	ZGuiWnd* pkMainWnd = GetWnd("GuiMainWnd");

	vector<ZGuiWnd*> vkCandidates;
	vector<ZGuiWnd*> sortlist;

	map<string, ZGuiWnd*> kWindows;
	m_pkGuiMan->GetWindows(kWindows);
	for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
	{
		if(IsEditorWnd(it->second) == false || it->second == pkMainWnd)
			vkCandidates.push_back(it->second);
	}
	
	while(1)
	{
		for(int i=0; i<vkCandidates.size(); i++)
		{
			GuiType eType = GetWndType(vkCandidates[i]);
			ZGuiWnd* pkParent = vkCandidates[i]->GetParent();

			if(pkParent && GetWndType(pkParent) == TabControl)
				if(!AlreadyInList(sortlist, pkParent))
					continue;

			if(eType == TabControl)
			{
				if(pkParent == NULL || (pkParent && AlreadyInList(sortlist, pkParent)) )
				{
					if(!AlreadyInList(sortlist, vkCandidates[i]))
						sortlist.push_back(vkCandidates[i]);

					ZGuiTabCtrl* pkTabCtrl = (ZGuiTabCtrl*) vkCandidates[i];
					for(int i=0; i<pkTabCtrl->GetNumPages(); i++)
					{
						ZGuiWnd* pkPage = pkTabCtrl->GetPage(i);
						if(!AlreadyInList(sortlist, pkPage))
							sortlist.push_back(pkPage);						
					}

					continue;
				}
			}

			if(pkParent == NULL)
			{
				bool already_in_list = false;
				for(int k=0; k<sortlist.size(); k++)
					if(sortlist[k] == vkCandidates[i])
					{
						already_in_list = true;
						break;
					}

					if(!already_in_list)
						sortlist.push_back(vkCandidates[i]);
			}
			else
			{
				for(int j=0; j<sortlist.size(); j++)
					if(pkParent == sortlist[j]) 
					{
						bool already_in_list = false;
						for(int k=0; k<sortlist.size(); k++)
							if(sortlist[k] == vkCandidates[i])
							{
								already_in_list = true;
								break;
							}

							if(!already_in_list)
								sortlist.push_back(vkCandidates[i]);

							break;
					}
			}
		}

		if(sortlist.size() == vkCandidates.size())
			break;
	}

	int iType = 0, iAlignment = 0, iResizeType = 0;
	char szName[150],szParent[150],szLabel[500];
	Rect rc;

	for(int k=0; k<sortlist.size(); k++)
	{
		if(sortlist[k] != pkMainWnd)
		{
			ZGuiWnd* pkParent = sortlist[k]->GetParent();

			strcpy(szName, sortlist[k]->GetName());
			if(pkParent)
				strcpy(szParent, pkParent->GetName());
			else
				strcpy(szParent, "");

			if(pkParent)
				rc = sortlist[k]->GetWndRect();
			else
				rc = sortlist[k]->GetScreenRect();

			strcpy(szLabel, sortlist[k]->GetText());

			iType = (int) GetWndType(sortlist[k]);

			iAlignment = sortlist[k]->m_iWndAlignment;
			iResizeType = sortlist[k]->m_iResizeType;

			fprintf(m_pkSaveFile, "\tCreateWnd(%s, \"%s\", \"%s\", \"%s\", %i, %i, %i, %i, 0, %s, %s)\n", 
				FormatWndType(GetWndType(sortlist[k])).c_str(), szName, szParent, szLabel, rc.Left, rc.Top, 
				rc.Width(), rc.Height(), FormatWndAlignent(sortlist[k]).c_str(), 
				FormatWndResizeType(sortlist[k]).c_str());			

			PrintSkins(sortlist[k]);

			fprintf(m_pkSaveFile, "\n");
		}
	}

	return true;
}

bool ZGuiResEd::WriteSpecialProperties()
{
	fprintf(m_pkSaveFile, "\n\t-- Special properties -----------------------------------------------" \
		"--------------------------------------------------------------\n\n");

	map<string, ZGuiWnd*> kWindows;
	m_pkGuiMan->GetWindows(kWindows);
	for( map<string, ZGuiWnd*>::iterator it = kWindows.begin(); it != kWindows.end(); it++)
	{
		ZGuiWnd* pkWnd = it->second;

		if(IsEditorWnd(pkWnd))
			continue;

		ZGuiFont* pkFont = pkWnd->GetFont();
		if(pkFont != NULL)
		{
			string strFontName = pkFont->m_szNameID;
			unsigned char r,g,b;
			pkWnd->GetTextColor(r,g,b);

			if(!(strFontName == "defguifont" && r == 0 && g == 0 && b == 0))
			{			
				fprintf(m_pkSaveFile, "\tSetFont(\"%s\",\"%s\",%i,%i,%i)\n", 
					pkWnd->GetName(), strFontName.c_str(), (int)r, (int)g, (int)b);
			}
		}

		//Rect rc;
		//if((rc=pkWnd->GetMoveArea()) != pkWnd->GetScreenRect())
		//{
		//	fprintf(m_pkSaveFile, "\tSetMoveArea(\"%s\",%i,%i,%i,%i)\n", 
		//		pkWnd->GetName(), 0, 0, 1600, 1200);
		//}

		GuiType eType = GetWndType(pkWnd);

		if(eType == Textbox)
		{
			ZGuiTextbox* pkTextbox = (ZGuiTextbox*) pkWnd;

			if(pkTextbox->IsMultiLine())
				fprintf(m_pkSaveFile, "\tChangeWndParameter(\"%s\",\"TOGGLE_MULTILINE\")\n", 
					pkWnd->GetName());			
		}
		else
		if(eType == Progressbar)
		{
			char szParameter[50];
			ZGuiProgressbar* pkProgressbar = (ZGuiProgressbar*) pkWnd;
			
			if(pkProgressbar->GetDir() != PBDIR_LEFT_TO_RIGHT)
			{				
				if(pkProgressbar->GetDir() == PBDIR_RIGHT_TO_LEFT)
					strcpy(szParameter, "PBDIR_RIGHT_TO_LEFT");
				if(pkProgressbar->GetDir() == PBDIR_TOP_TO_BOTTOM)
					strcpy(szParameter, "PBDIR_TOP_TO_BOTTOM");
				if(pkProgressbar->GetDir() == PBDIR_BOTTOM_TO_TOP)
					strcpy(szParameter, "PBDIR_BOTTOM_TO_TOP");
				
				fprintf(m_pkSaveFile, "\tChangeWndParameter(\"%s\",\"%s\")\n", 
						pkWnd->GetName(), szParameter);	
			}

			if(pkProgressbar->GetTextOrientation() != PBTEXTORIENT_CENTER)
			{
				if(pkProgressbar->GetTextOrientation() == PBTEXTORIENT_LEFT)
					strcpy(szParameter, "PBTEXTORIENT_LEFT");
				if(pkProgressbar->GetTextOrientation() == PBTEXTORIENT_OVER)
					strcpy(szParameter, "PBTEXTORIENT_OVER");
				if(pkProgressbar->GetTextOrientation() == PBTEXTORIENT_RIGHT)
					strcpy(szParameter, "PBTEXTORIENT_RIGHT");
				if(pkProgressbar->GetTextOrientation() == PBTEXTORIENT_UNDER)
					strcpy(szParameter, "PBTEXTORIENT_UNDER");
				
				fprintf(m_pkSaveFile, "\tChangeWndParameter(\"%s\",\"%s\")\n", 
						pkWnd->GetName(), szParameter);	
			}

			if(pkProgressbar->m_bShowText == false)
				fprintf(m_pkSaveFile, "\tChangeWndParameter(\"%s\",\"%s\")\n", 
						pkWnd->GetName(), "PB_HIDETEXT");
		}
		else
		if(eType == Slider)
		{
			if(((ZGuiSlider*)pkWnd)->GetCtrlStyle(SCF_VERT))
			{
				fprintf(m_pkSaveFile, "\tChangeWndParameter(\"%s\",\"SCF_VERT\")\n", 
						pkWnd->GetName());
			}
		}
	}	

	map<string,SPECIAL_WND_INFO>::iterator it;
	for( map<string, SPECIAL_WND_INFO>::iterator it = 
		 m_kSpecialWndInfo.begin(); it != m_kSpecialWndInfo.end(); it++)
	{
		ZGuiWnd* pkWnd = GetWnd(it->first);

		if(pkWnd)
		{
			if(it->second.bHiddenFromStart)
				fprintf(m_pkSaveFile, "\tShowWnd(\"%s\",0,0)\n", pkWnd->GetName());
			if(it->second.bFreemovement)
			{
				Rect rc;
				if((rc=pkWnd->GetMoveArea()) != pkWnd->GetScreenRect())
				{
					fprintf(m_pkSaveFile, "\tSetMoveArea(\"%s\",%i,%i,%i,%i)\n", 
						pkWnd->GetName(), 0, 0, 1600, 1200);
				}
			}

		}
	}

	return true;
}

void ZGuiResEd::PrintSkins(ZGuiWnd* pkWnd)
{
	const char* szName = pkWnd->GetName();

	vector< pair<ZGuiSkin**, string> >kSkins;
	pkWnd->GetWndSkinsDesc(kSkins);

	vector< pair<ZGuiSkin**, string> >::iterator itSkin = kSkins.begin(); 
	for( ; itSkin != kSkins.end(); itSkin++)
	{
		if(*itSkin->first)
		{
			ZGuiSkin kSkin = *(*itSkin->first);

			// leta rätt på namnet på skin'en och printa
			for(unsigned int k=0; k<m_kSkinTable.size(); k++)
				if(kSkin == m_kSkinTable[k].first)
				{
					fprintf(m_pkSaveFile, "\tChangeSkin(\"%s\",\"%s\",\"%s\")\n", 
						szName, m_kSkinTable[k].second.c_str(), itSkin->second.c_str());
					break;
				}
		}
	}

	kSkins.clear(); 
}

bool ZGuiResEd::AlreadyInList(vector<ZGuiWnd*>& kList, ZGuiWnd* kWindow)
{
	for(int i=0; i<kList.size(); i++)
		if(kList[i] == kWindow)
			return true;

	return false;
}

/*
bool ZGuiResEd::SearchFiles(vector<string>& vkPathList, const char* szRootPath, 
								 char* szExtension, bool bSearchForFolders)
{
	char searchPath[512];
	WIN32_FIND_DATA finddata;
	HANDLE hFind;
	BOOL bMore;
	set<string> kSearchedFiles;
	list<string> dir_list;
	list<string> vkFileNames;

	dir_list.push_back(string(szRootPath));

	while(1)
	{
		vkFileNames.clear();
		string currentFolder = dir_list.back();
		strcpy(searchPath, currentFolder.c_str());

		if(strnicmp("\\*",searchPath+strlen(searchPath)-2,2))
			strcat(searchPath, "\\*");

		hFind = FindFirstFile(searchPath, &finddata);         
		bMore = (hFind != (HANDLE) -1);   
		
		while (bMore) 
		{                  
			if (strcmp (finddata.cFileName, ".") && strcmp (finddata.cFileName, ".."))
			{
				vkFileNames.push_back(string(finddata.cFileName));
			}
             
			bMore = FindNextFile(hFind, &finddata);         
		} 

		// Lägg till alla filer
		for(list<string>::iterator it = vkFileNames.begin(); it != vkFileNames.end(); it++)  
		{
			string strLabel = (*it);
			string id = currentFolder + string("\\") + strLabel;

			// formatera filnamnet till lowercase.
			int length = id.length();
			for(int i=0; i<length; i++)
				if(id[i] > 64 && id[i] < 91)
					id[i] += 32;

			if(kSearchedFiles.find(id) == kSearchedFiles.end())
			{			
				if(strLabel.find(".") == string::npos) // är katalog
				{
					dir_list.push_back(currentFolder + string("\\") + strLabel);

					if(bSearchForFolders)
						vkPathList.push_back(currentFolder + string("\\") + strLabel);
				}

				const char* s = id.c_str();

				if(!bSearchForFolders)
				{
					if( strstr(s, szExtension) ) //if( strstr(s, ".lua") )
						vkPathList.push_back(id);
				}

				kSearchedFiles.insert(id);
			}
		}

		// ej klivit in i ett nytt dir? gå tillbaks
		if(dir_list.back() == currentFolder)
		{
			// sista?
			if(currentFolder == szRootPath)
				break;

			dir_list.pop_back();
		}
	}

	FindClose(hFind);

	return true;
}

*/






