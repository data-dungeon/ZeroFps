#include "zeroed.h"

void ZeroEd::SetupGuiEnviroment()
{
	// Create from script.
	LoadGuiFromScript("data/script/gui/zeroed2.lua");
	GetWnd("worktab")->Hide();

	m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("data/textures/gui/blue_cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/blue_cursor_a.bmp", 0), 32, 32);
	
	// Fill zone- and object treebox.
	BuildFileTree("ZoneModelTree", "data/mad/zones", ".mad");
	BuildFileTree("ObjectTree", "data/script/objects", ".lua");

	// Fill enviroment listbox.
	vector<string> vkFileNames;
	m_pkZFVFileSystem->ListDir(&vkFileNames, "/data/enviroments", false);
	for(unsigned int i=0; i<vkFileNames.size(); i++)
		AddListItem("EnviromentPresetList", (char*) vkFileNames[i].c_str());

	m_pkGui->GetToolTip()->AddToolTip(GetWnd("ToggleLight"),"Light");
	m_pkGui->GetToolTip()->AddToolTip(GetWnd("OpenWorkTabButton"),"Worktab");
	m_pkGui->GetToolTip()->AddToolTip(GetWnd("RotateZoneModellButton"),"Rotate Zone");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("DeleteZoneButton"),"Delete Zone");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("AddToScriptBn"),"Create Script From Current");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("PlaceongroundButton"),"Place object on ground");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("DeleteObjectButton"),"Delete object");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("RotateObjectButton"),"Rotate object");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("DisableFreeZoneBuildBn"),"Free Zone Build");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("RemovePropertyBn"),"Remove Selected Property");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("OpenAllPropertyWnd"),"Add New Property");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("OpenEditPropertyWnd"),"Edit Selected Property");
   m_pkGui->GetToolTip()->AddToolTip(GetWnd("AddPropertyBn"),"Add Selected Property");

	CheckButton("PlaceongroundButton", m_bPlaceObjectsOnGround);
	CheckButton("DisableFreeZoneBuildBn", m_bDisableFreeZonePlacement);

	ShowWnd("SelectFileWnd", false);

	CreateWnd(Label, "vp1Label", "Perspective", GetWnd("vp1"), 2, 0, 100, 20, 0);
	CreateWnd(Label, "vp2Label", "Top", GetWnd("vp2"), 2, 0, 100, 20, 0);
	CreateWnd(Label, "vp3Label", "Left", GetWnd("vp3"), 2, 0, 100, 20, 0);
	CreateWnd(Label, "vp4Label", "Front", GetWnd("vp4"), 2, 0, 100, 20, 0);
}

void ZeroEd::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
{
	ZGuiWnd* pkWndClicked = NULL;

	if(GetWndType(pkMainWnd) == Menu)
	{
		list<ZGuiWnd*> kChilds;
		pkMainWnd->GetChildrens(kChilds); 

		list<ZGuiWnd*>::iterator it = kChilds.begin();
		for( ; it!=kChilds.end(); it++)
		{
			if((*it)->GetID() == iID)
			{
				pkWndClicked = (*it);
				break;
			}
		}
	}
	else
	{
		pkWndClicked = GetWnd(iID);
	}

	if(pkWndClicked)
	{
		string strMainWnd = pkMainWnd->GetName();

		string strWndClicked = pkWndClicked->GetName();

		string strParent = "null";

		if(pkWndClicked->GetParent())
		{
			strParent = pkWndClicked->GetParent()->GetName();
			strMainWnd = strParent;
		}

		if(strMainWnd == "GuiMainWnd")
		{
			if(strWndClicked == "OpenWorkTabButton")
			{
				static bool state[2] = {0,0};
				if( IsWndVisible("worktab") )
				{
					m_pkAudioSys->StartSound("/data/sound/close_window.wav",m_pkAudioSys->GetListnerPos());
					GetWnd("worktab")->Hide(); 

					state[0] = GetWnd("AddNewProperyWnd")->IsVisible();
					state[1] = GetWnd("EditPropertyWnd")->IsVisible();

					GetWnd("AddNewProperyWnd")->Hide();
					GetWnd("EditPropertyWnd")->Hide();
					ShowWnd("SelectFileWnd", false);
				}
				else 
				{
					m_pkAudioSys->StartSound("/data/sound/open_window.wav",m_pkAudioSys->GetListnerPos());
					GetWnd("worktab")->Show(); 
					m_pkGui->PlaceWndFrontBack(GetWnd("worktab"), true);

					if(state[0] == true)
						GetWnd("AddNewProperyWnd")->Show();
					else
						GetWnd("AddNewProperyWnd")->Hide();

					if(state[1] == true)
						GetWnd("EditPropertyWnd")->Show();
					else
						GetWnd("EditPropertyWnd")->Hide();
				}
			}
			else
			if(strWndClicked == "ToggleLight")
			{
				ToogleLight( static_cast<ZGuiCheckbox*>(pkWndClicked)->IsChecked() );
			}
		}
		else
		if(strMainWnd == "ZonePage")
		{
			if(strWndClicked == "RotateZoneModellButton")
			{
				RotateActive();
			}
			else
			if(strWndClicked == "DeleteZoneButton")
			{
				int id = m_pkEntityManager->GetZoneIndex(m_kZoneMarkerPos,-1,false);
				m_pkEntityManager->DeleteZone(id);
			}
			else
			if(strWndClicked == "DisableFreeZoneBuildBn")
			{
				m_bDisableFreeZonePlacement = IsButtonChecked((char*)strWndClicked.c_str());
				RebuildZonePosArray();
			}
		}
		else
		if(strMainWnd == "ObjectPage")
		{
			if(strWndClicked == "RotateObjectButton")
			{
				RotateActive();
			}
			else
			if(strWndClicked == "DeleteObjectButton")
			{		
				Entity* pkObj = m_pkEntityManager->GetEntityByID(m_iCurrentObject);		
				if(pkObj) 
				{
					m_pkEntityManager->Delete(pkObj);
					m_iCurrentObject = -1;
				}
			}
			else
			if(strWndClicked == "PlaceongroundButton")
			{
				m_bPlaceObjectsOnGround = IsButtonChecked((char*)strWndClicked.c_str());
				if(m_bPlaceObjectsOnGround)
					PlaceObjectOnGround(m_iCurrentObject);
			}
         else
		   if(strWndClicked == "AddToScriptBn")
			{            
            if(IsWndVisible("SaveScriptFileNameEb"))
               SaveCurrentToScript();
            else
            {
               ShowWnd("SaveScriptFileNameEb",true);
               GetWnd("ObjectTree")->Resize(200, 200-60, false);
            }
			}
		}
		else
		if(strMainWnd == "PropertyPage")
		{
			if(strWndClicked == "OpenAllPropertyWnd")
			{
				ShowWnd("EditPropertyWnd",false);
				ShowWnd("AddNewProperyWnd",!IsWndVisible("AddNewProperyWnd"));
				FillPropertyList();
			}
			else
			if(strWndClicked == "OpenEditPropertyWnd")
			{
				ShowWnd("AddNewProperyWnd",false);
				ShowWnd("EditPropertyWnd",!IsWndVisible("EditPropertyWnd"));
				FillPropertyValList();
			}
			else
			if(strWndClicked == "RemovePropertyBn")
			{
				RemoveSelProperty();
			}
			else
			if(strWndClicked == "PropertBn1")
			{
				Entity* pkEnt;
				P_AmbientSound* pkProp;

				if((pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject)))
				{
					if(pkProp = (P_AmbientSound*) pkEnt->GetProperty("P_AmbientSound"))
					{
						static int s_iPrevMode = m_iEditMode;
						if(IsButtonChecked("PropertBn1")) // v�xla �ver till att editera ambientarea
						{
							pkProp->SetFreeForm(true);

							s_iPrevMode = m_iEditMode;
							m_iEditMode = EDIT_AMBIENTSOUNDS;
						}
						else // st�ng igen polygonen och v�xla �ver till tidigare editmode
						{
							vector<Vector2> kPolys;
							pkProp->GetArea(kPolys);

							if(!kPolys.empty())
							{
								if(kPolys[0] != kPolys.back()) // st�ng bara igen en g�ng!
								{
									kPolys.push_back(kPolys[0]);
									pkProp->SetArea(kPolys);
									printf("st�nger igen!\n");
								}
							}

							m_iEditMode = s_iPrevMode;
						}
					}
				}
			}
			else
			if(strWndClicked == "PropertBn2")
			{
				Entity* pkEnt;
				P_AmbientSound* pkProp;

				if((pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject)))
				{
					if(pkProp = (P_AmbientSound*) pkEnt->GetProperty("P_AmbientSound"))
					{
						vector<Vector2> kEmpty;
						pkProp->SetArea(kEmpty); 
					}
				}
			}
		}
		else
		if(strMainWnd == "EditPropertyWnd")
		{
			if(strWndClicked == "SelectPropertyValBn")
			{
				char* szProperty;
				if((szProperty = GetSelItem("PropertyList")))
				{
					char* szPropertyVal;
					if((szPropertyVal = GetSelItem("PropertyValList")))
					{
						if( string(szProperty) == string("P_Mad") && string(szPropertyVal) == string("m_kMadFile"))
						{
							ShowWnd("SelectFileWnd", true, true);

							if(m_iSelectFileState != SELECT_MAD)
								BuildFileTree("SelectFileTree", "data/mad", ".mad");

							m_iSelectFileState = SELECT_MAD;
						}
						else
						if( (string(szProperty) == string("P_Sound") && string(szPropertyVal) == string("filename")) ||
							 (string(szProperty) == string("P_AmbientSound") && string(szPropertyVal) == string("filename")) )
						{
							ShowWnd("SelectFileWnd", true, true);

							if(m_iSelectFileState != SELECT_SOUND)
								BuildFileTree("SelectFileTree", "data/sound", ".wav");

							m_iSelectFileState = SELECT_SOUND;
						}
					}
				}
			}
		}
		else
		if(strMainWnd == "AddNewProperyWnd")
		{
			char* item;
//			Entity* pkEnt;
			if(strWndClicked == "AddPropertyBn")
			{
				if((item = GetSelItem("AllPropertyList")))
				{
					AddProperty(m_iCurrentObject,item);
				
				}
				//	if((pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject)))
				//		if(pkEnt->AddProperty(item))
				//			UpdatePropertyList(pkEnt->GetEntityID());
			}
		}
		else
		if(strMainWnd == "MainMenu")
		{
			if(!m_pkIni->Open("data/script/gui/menu.txt", false))
			{
				cout << "Failed to load ini file for menu!\n" << endl;
				return;
			}

			vector<string> akSections;
			m_pkIni->GetSectionNames(akSections);

			// Run Menu command
			for(unsigned int i=0; i<akSections.size(); i++)
			{
				if(strWndClicked == string(akSections[i].c_str()))
				{
					char* cmd = m_pkIni->GetValue(akSections[i].c_str(), "Cmd");
					m_pkZeroFps->m_pkConsole->Execute(cmd);

					break;
				}
			}

			m_pkIni->Close();
		}
	}
}

void ZeroEd::OnClickListbox(int iListBoxID, int iListboxIndex, ZGuiWnd* pkMain)
{
	if(pkMain == NULL)
		return;

	string strMainWndName, strListBox;

	ZGuiWnd* pkListBox = NULL;

	list<ZGuiWnd*> kChilds;
	pkMain->GetChildrens(kChilds); 

	list<ZGuiWnd*>::iterator it = kChilds.begin();
	for( ; it != kChilds.end(); it++)
	{
		if((*it)->GetID() == (unsigned int) iListBoxID)
		{
			pkListBox = (*it);
			break;
		}	 
	}

	strMainWndName = pkMain->GetName();

	if(pkListBox != NULL)
		strListBox = pkListBox->GetName();

	if(strMainWndName == "EnviromentPage")
	{
		if(strListBox == "EnviromentPresetList")
		{
			char *szPreset = static_cast<ZGuiListbox*>(pkListBox)->GetSelItem()->GetText();

			if(szPreset)
			{
				string szFull = "data/enviroments/" + string(szPreset);
				printf("setting enviroment %s\n", szFull.c_str());
				SetZoneEnviroment( szFull.c_str()  );  
			}
		}
	}

	if(strListBox == "PropertyList")
	{
		ShowWnd("AddNewProperyWnd",false);
		ShowWnd("EditPropertyWnd",true);
		FillPropertyValList();
		SetText("PropertyValEb", "");
	}

	if(strListBox == "PropertyValList")
	{
		char* szProperty, *item;
		Entity* pkEnt;
		Property* pkProp;

		if((szProperty=GetSelItem("PropertyList")))
			if((pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject)))
				if((item = GetSelItem("PropertyValList")))
				{
					if(string("Variables") == string(szProperty))
					{
						SetText("PropertyValEb", (char*)pkEnt->GetVarString(item).c_str());
					}
					
					else if((pkProp = pkEnt->GetProperty(szProperty)))
						SetText("PropertyValEb", (char*)pkProp->GetValue(item).c_str());
				}
	}
}

void ZeroEd::OnClickTreeItem(char *szTreeBox, char *szParentNodeText, 
											char *szClickNodeText, bool bHaveChilds)
{
	if(strcmp(szTreeBox, "ZoneModelTree") == 0)
	{
		if(szClickNodeText && bHaveChilds == false)
		{
			string strFullpath = string("data/mad/zones/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			m_strActiveZoneName = strFullpath;

			//m_iCurrentMarkedZone = m_pkEntityManager->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			m_iCurrentMarkedZone = GetZoneID(m_kZoneMarkerPos);
			
			// Setting new zone modell
			if(m_iCurrentMarkedZone != -1)	// �R n�gon zon markerad?
			{
				//pkObjectMan->LoadZone(m_iCurrentMarkedZone);
				//m_pkEntityManager->SetZoneModel(strFullpath.c_str(),m_iCurrentMarkedZone);
				
				SendSetZoneModel(strFullpath,m_iCurrentMarkedZone);
				
				
				printf("Setting new zone modell to %s\n", strFullpath.c_str());
			}

			AutoSetZoneSize(m_strActiveZoneName);
		}
	}
	else
	if(strcmp(szTreeBox, "ObjectTree") == 0)
	{
		if(szClickNodeText && bHaveChilds == false)
		{
			string strFullpath = string("data/script/objects/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			m_strActiveObjectName = strFullpath;
		}
	}
	else
	if(strcmp(szTreeBox, "CustomEntitiesTree") == 0)
	{
		if(szClickNodeText && bHaveChilds == false)
		{
			string strFullpath = string("data/script/custom_scripts/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			m_strActiveObjectName = strFullpath;
		}
	}
	else
	if(strcmp(szTreeBox, "SelectFileTree") == 0)
	{
		string strFullpath;

		switch(m_iSelectFileState)
		{
		case SELECT_MAD:

			strFullpath = string("data/mad/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			SetText("PropertyValEb", (char*) strFullpath.c_str());

			AddPropertyVal();

			ShowWnd("SelectFileWnd", false); // close window

			break;

		case SELECT_SOUND:

			strFullpath = string("data/sound/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			SetText("PropertyValEb", (char*) strFullpath.c_str());

			AddPropertyVal();
			
			ShowWnd("SelectFileWnd", false); // close window

			AddPropertyVal(); // m�ste anropa dubbelt, I don't know why...

			break;

		}
	}
}

void ZeroEd::OnClickTabPage(ZGuiTabCtrl *pkTabCtrl, int iNewPage, int iPrevPage)
{
	string strTabCtrlName = pkTabCtrl->GetName();

	if(strTabCtrlName == "WorkTabWnd")
	{
		switch(iNewPage)
		{
		case 0:
			m_iEditMode = EDIT_ZONES;
			if(GetWnd("AddNewProperyWnd"))GetWnd("AddNewProperyWnd")->Hide();
			if(GetWnd("EditPropertyWnd"))GetWnd("EditPropertyWnd")->Hide();
			ShowWnd("SelectFileWnd", false);
			break;
		case 1:
			m_iEditMode = EDIT_OBJECTS;
			if(GetWnd("AddNewProperyWnd"))GetWnd("AddNewProperyWnd")->Hide();
			if(GetWnd("EditPropertyWnd"))GetWnd("EditPropertyWnd")->Hide();
			ShowWnd("SelectFileWnd", false);
         BuildFileTree("ObjectTree", "data/script/objects/", ".lua");
			break;
		case 2:
			ShowWnd("SelectFileWnd", false);
			break;
		case 3:
			ShowWnd("SelectFileWnd", false);
			break;
		case 4:
			ShowWnd("SelectFileWnd", false);
			if(GetWnd("AddNewProperyWnd"))GetWnd("AddNewProperyWnd")->Hide();
			if(GetWnd("EditPropertyWnd"))GetWnd("EditPropertyWnd")->Hide();
			m_iEditMode = EDIT_AMBIENTSOUNDS;
			break;
		}
	}
}

char* ZeroEd::GetSelEnviromentString()
{
	ZGuiListbox* pkEnviromentList = static_cast<ZGuiListbox*>(GetWnd("EnviromentPresetList"));
	if(pkEnviromentList)
	{
		ZGuiListitem* pkSel = pkEnviromentList->GetSelItem(); 
		if(pkSel)
			return pkSel->GetText();
	}

	return NULL;
}