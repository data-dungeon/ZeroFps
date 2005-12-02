#include "zeroed.h"
#include "../zerofpsv2/engine_systems/propertys/p_ambientsound.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/basic/zfbasicfs.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "../zerofpsv2/engine/inputhandle.h"

void ZeroEd::SetupGuiEnviroment()
{
	// Create from script.
	if(!LoadGuiFromScript("data/script/gui/zeroed2.lua"))
	{
		printf("Failed to load GUI script!\n");
		return;
	}

   if(!LoadGuiFromScript("data/script/gui/terrain.lua"))
	{
		printf("Error loading gui script!\n");
		return;
	}

	GetWnd("worktab")->Hide();
	m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("gui/blue_cursor.bmp", 0),
		m_pkTexMan->Load("gui/blue_cursor_a.bmp", 0), 32, 32);

   char szFontData[512], szFontTex[512];
   sprintf(szFontData, "data/textures/gui/fonts/%s.fnt", "book_antiqua_10_bold_outlined");

   sprintf(szFontTex, "gui/fonts/%s.tga", "book_antiqua_10_bold_outlined");
   ZGuiFont* pkOutLineFont = new ZGuiFont("OutLineFont");

   pkOutLineFont->Create(szFontData, m_pkTexMan->Load(szFontTex, 0), 1);

	m_pkGuiMan->Add("OutLineFont", pkOutLineFont);

	// Fill zone- and object treebox.
	BuildFileTree("ZoneModelTree", "data/mad/zones", ".mad");
	BuildFileTree("ObjectTree", "data/script/objects", ".lua");

	SetFont("ZoneModelTree", "small7");
	static_cast<ZGuiTreebox*>(GetWnd("ZoneModelTree"))->ChangeIconSize(8); 

	SetFont("ObjectTree", "small7");
	static_cast<ZGuiTreebox*>(GetWnd("ObjectTree"))->ChangeIconSize(8); 



	
	//disable shortkey thingy for enviroment listbox
	if(ZGuiListbox* pkLB = (ZGuiListbox*)GetWnd("EnviromentPresetList"))
		pkLB->m_bCanHaveKeyboarFocus =false;
	
	// Fill enviroment listbox.
	vector<string> vkFileNames;
	m_pkZFVFileSystem->ListDir(vkFileNames, "/data/enviroments", false);
	for(unsigned int i=0; i<vkFileNames.size(); i++)
		AddListItem("EnviromentPresetList", (char*) vkFileNames[i].c_str());

	m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"ToggleLight"/*)*/,"Light");
	m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"OpenWorkTabButton", /*)*/"Worktab");
	m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"RotateZoneModellButton", /*)*/"Rotate Zone");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"DeleteZoneButton", /*)*/"Delete Zone");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"AddToScriptBn", /*)*/"Create Script From Current");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"PlaceongroundButton", /*)*/"Place object on ground");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"DeleteObjectButton", /*)*/"Delete object");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"RotateObjectButton", /*)*/"Rotate object");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"DisableFreeZoneBuildBn", /*)*/"Free Zone Build");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"RemovePropertyBn", /*)*/"Remove Selected Property");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"OpenAllPropertyWnd", /*)*/"Add New Property");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"OpenEditPropertyWnd", /*)*/"Edit Selected Property");
   m_pkGui->GetToolTip()->AddToolTip(/*GetWnd(*/"AddPropertyBn", /*)*/"Add Selected Property");

	CheckButton("PlaceongroundButton", m_bPlaceObjectsOnGround);

	ShowWnd("SelectFileWnd", false);
	ShowWnd("PreviewWnd", false);

	CreateWnd(Label, "vp1Label", "Perspective", GetWnd("vp1"), 2, 0, 100, 20, 0);
	CreateWnd(Label, "vp2Label", "Top", GetWnd("vp2"), 2, 0, 100, 20, 0);
	CreateWnd(Label, "vp3Label", "Left", GetWnd("vp3"), 2, 0, 100, 20, 0);
	CreateWnd(Label, "vp4Label", "Front", GetWnd("vp4"), 2, 0, 100, 20, 0);

	GetWnd("ZeroEdInfoLabel")->SetFont(pkOutLineFont);
	GetWnd("ZeroEdInfoLabel")->SetTextColor(255,255,255); 

	CreateMenu("data/script/gui/objectmenu.txt", "ObjectMenu", true);
	GetWnd("ObjectMenu")->Hide();

	GUIServerInfo::Load(m_vkServerList);
	GUIFillServerList();

	AddLoadFoldersToMenu();
	
	((ZGuiButton*)GetWnd("OpenWorkTabButton"))->m_bAcceptRightClicks = true;
}

void ZeroEd::SetupGui_Terrain()
{
	// Add Terrain Commands.
	ZGuiCombobox* pkEditMode = dynamic_cast<ZGuiCombobox*>(GetWnd("TerrEditMode"));
	pkEditMode->AddItem("Paint",			0);	// HMAP_EDITVERTEX
	pkEditMode->AddItem("Flatten",		1);	// HMAP_DRAWSMFLAT
	pkEditMode->AddItem("Material",		2);	// HMAP_DRAWMASK
	pkEditMode->AddItem("Visibility",	3);	// HMAP_DRAWVISIBLE
	pkEditMode->AddItem("Smoothing",		4);	// HMAP_SMOOTH
	pkEditMode->AddItem("Stitch",			5);	// HMAP_STITCH
	pkEditMode->SetNumVisibleRows( 6 );

	//add terrain materials
	
	
	//get file list
	vector<string> kFiles;
	vector<string> kFilters;
	kFilters.push_back(".zlm");
	kFilters.push_back(".zmt");	
	m_pkZFVFileSystem->ListDirFilter(kFiles,kFilters,"data/material/heightmap/",true);
			
	//add files to gui list
	pkEditMode = dynamic_cast<ZGuiCombobox*>(GetWnd("TerrTexture"));	
	pkEditMode->SetNumVisibleRows( 6 );	
	for(int i = 0;i<kFiles.size();i++)
		pkEditMode->AddItem( (char*)(string("heightmap/")+kFiles[i]).c_str(),i);

	


	((ZGuiSlider*)GetWnd("TerrInnerSlider"))->AddBuddyWindow( GetWnd("TerrInnerBox") );
	((ZGuiSlider*)GetWnd("TerrInnerSlider"))->SetRange(0,25);
	((ZGuiSlider*)GetWnd("TerrInnerSlider"))->SetPos((int)m_fHMInRadius, true);

	((ZGuiSlider*)GetWnd("TerrOuterSlider"))->AddBuddyWindow( GetWnd("TerrOuterBox") );
	((ZGuiSlider*)GetWnd("TerrOuterSlider"))->SetRange(0,25);
	((ZGuiSlider*)GetWnd("TerrOuterSlider"))->SetPos((int)m_fHMOutRadius, true);

	((ZGuiSlider*)GetWnd("TerrStrSlider"))->AddBuddyWindow( GetWnd("TerrStrBox") );
	((ZGuiSlider*)GetWnd("TerrStrSlider"))->SetRange(0,100);
	((ZGuiSlider*)GetWnd("TerrStrSlider"))->SetPos((int)100, true);
}

void ZeroEd::OnKeyPress(int iID, ZGuiWnd* win)
{
	if(win == NULL)
		win = GetWnd("GuiMainWnd");

	if(iID == KEY_RETURN)
	{
		if(strcmp("PropertyValEb", win->GetName()) == 0)
		{
			AddPropertyVal();
			AddPropertyVal();
		}
      else
      if(strcmp("SaveScriptFileNameEb", win->GetName()) == 0)
      {
         SaveCurrentToScript();
      }
	}

	string strController;
	strController = win->GetName();

	if( iID == KEY_TAB || iID == KEY_RETURN)
	{	
		if( strController	==	"ServerNameTextbox"	)
			m_pkGui->SetFocus( GetWnd("ServerIPTextbox")   ); 
		if( strController	==	"ServerIPTextbox"	)
			m_pkGui->SetFocus( GetWnd("UserNameTextbox")   ); 
		if( strController	==	"UserNameTextbox"	)
			m_pkGui->SetFocus( GetWnd("PasswordTextbox")	  );	
		if( strController	==	"PasswordTextbox"	)
		{
			if(iID == KEY_TAB)
				m_pkGui->SetFocus( GetWnd("ServerNameTextbox") );	
			else
				OnCommand(GetWnd("ManageConnectionsAddServerBn")->GetID(), false, 
					GetWnd("ManageConnectionsWnd"));
		}
	}
}

void ZeroEd::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
{
	ZGuiWnd* pkWndClicked = NULL;
	string strMenuFile = "";

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

		if(GetWndType(pkMainWnd) == Menu)
			strWndClicked = ((ZGuiMenu*) pkMainWnd)->GetItemName(iID);

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
					GetWnd("worktab")->Hide(); 

					state[0] = GetWnd("AddNewProperyWnd")->IsVisible();
					state[1] = GetWnd("EditPropertyWnd")->IsVisible();

					GetWnd("AddNewProperyWnd")->Hide();
					GetWnd("EditPropertyWnd")->Hide();
					ShowWnd("SelectFileWnd", false);
					ShowWnd("PreviewWnd", false);
				}
				else 
				{					
					int iSize = (bRMouseBnClick == true) ? (GetHeight()-100) : 257;
					ResizeWorkTab(iSize);

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
				ToogleLight( );
			}
		}
		else
		if(strMainWnd == "ZonePage")
		{
			if(strWndClicked == "RotateZoneModellButton")
			{
				//RotateActive();
				m_iCurrentMarkedZone = GetZoneID(m_kZoneMarkerPos);
				SendRotateZoneModel(m_iCurrentMarkedZone);
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
				if(m_iAutoSnapZoneCorner==-1) m_iAutoSnapZoneCorner=0; else m_iAutoSnapZoneCorner=-1;
			}
		}
		else
		if(strMainWnd == "ObjectPage")
		{
			if(strWndClicked == "RotateObjectButton")
			{
				SendRotateEntity(m_iCurrentObject,Vector3(0,90,0));				
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
		if(strMainWnd == "EnviromentPage")
		{
			SendSetZoneEnviroment("",GetZoneID(m_kZoneMarkerPos));
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
						if(IsButtonChecked("PropertBn1")) // v�la �er till att editera ambientarea
						{
							pkProp->SetFreeForm(true);

							s_iPrevMode = m_iEditMode;
							m_iEditMode = EDIT_AMBIENTSOUNDS;
						}
						else // st�g igen polygonen och v�la �er till tidigare editmode
						{
							float fFloor, fRoof;
							vector<Vector2> kPolys;
							pkProp->GetArea(kPolys, fFloor, fRoof);

							if(!kPolys.empty())
							{
								if(kPolys[0] != kPolys.back()) // st�g bara igen en g�g!
								{
									kPolys.push_back(kPolys[0]);
									pkProp->SetArea(kPolys, fFloor, fRoof);
									printf("st�ger igen!\n");
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
						pkProp->SetArea(kEmpty, 0, 0); 
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
								BuildFileTree("SelectFileTree", "data/audio", ".wav");

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
		if(strMainWnd == "PreviewWnd")
		{
			if(strWndClicked == "PreviewOKBn")
			{
				AddPropertyVal();
				ShowWnd("SelectFileWnd", false); // close window
				ShowWnd("PreviewWnd", false); // close window
			}
			else
			if(strWndClicked == "PreviewCancelBn")
			{
				ShowWnd("PreviewWnd", false); // close window
			}
		}
		else
		if(strMainWnd == "ManageConnectionsWnd")
		{
			if(strWndClicked == "LoginCheckbox")
			{
				if(IsButtonChecked("LoginCheckbox"))
				{	
					GetWnd("UserNameTextbox")->Enable();
					GetWnd("PasswordTextbox")->Enable();
				}
				else
				{
					GetWnd("UserNameTextbox")->Disable();

					GetWnd("PasswordTextbox")->Disable();

					SetText("UserNameTextbox", "anonymous");
					SetText("PasswordTextbox", "nopass");
				}
			}
			else
			if(strWndClicked == "ManageConnectionsCloseBn")
			{
				ShowWnd("ManageConnectionsWnd", false);
				((ZGuiMenu*)GetWnd("MainMenu"))->ResizeMenu();
				GUIServerInfo::Save(m_vkServerList);
			}
			else
			if(strWndClicked == "ManageConnectionsAddServerBn")
			{
				OnGUIAddServer(true);
			}
			else
			if(strWndClicked == "ManageConnectionsRemoveServerBn")
			{
				OnGUIAddServer(false);
			}
		}
		else
		if(strMainWnd == "MainMenu")
		{
			strMenuFile = "data/script/gui/menu.txt";
		}
		else
		if(strMainWnd == "ObjectMenu")
		{
			strMenuFile = "data/script/gui/objectmenu.txt";

			if(strWndClicked == "Remove_Object")
			{
				SendDeleteSelected();
			}
			else
			if(strWndClicked == "Rotate_180_degree")
			{
				SendRotateEntity(m_iCurrentObject,Vector3(0,180,0));
			}			
			else
			if(strWndClicked == "Rotate_90_degree")
			{
				SendRotateEntity(m_iCurrentObject,Vector3(0,90,0));
			}
			else
			if(strWndClicked == "Rotate_45_degree")
			{
				SendRotateEntity(m_iCurrentObject,Vector3(0,45,0));
			}
			else
			if(strWndClicked == "Menu_PlaceOnGroup")
			{
				if(m_iCurrentObject)
					PlaceObjectOnGround(m_iCurrentObject);
			}
		}

		// Run menu command.
		if(!strMenuFile.empty())
		{
			if(!m_pkIni->Open(strMenuFile.c_str(), false))
			{
				cerr << "Warning: Failed to load ini file for menu!\n" << endl;
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
					if(cmd)
						m_pkZeroFps->m_pkConsole->Execute(cmd);
					break;
				}
			}

			m_pkIni->Close();

			ZGuiMenu* pkMenu = ((ZGuiMenu*)GetWnd("MainMenu"));

			if(pkMenu)
			{
				ZGuiMenuItem* pkItem = pkMenu->GetItem(strWndClicked.c_str());
				if(pkItem)
				{
					ZGuiMenuItem* pkParentItem = pkItem->pkParent;

					if(strWndClicked == "Menu_File_Connect_Manage")
					{
						bool bFistTime = GetWnd("ManageConnectionsWnd") == NULL;

						if(!LoadGuiFromScript("data/script/gui/manageconnection.lua"))
						{
							printf("Failed to load GUI script!\n");
							return;
						}

						GUIFillServerList();

						if(bFistTime)
						{
							GetWnd("UserNameTextbox")->Disable();
							GetWnd("PasswordTextbox")->Disable();

							SetText("UserNameTextbox", "anonymous");
							SetText("PasswordTextbox", "nopass");
						}
					}
					else
					if(pkParentItem && !strcmp(pkParentItem->szNameID, "Menu_File_Connect"))
					{
						ZGuiMenu* pkMenu = ((ZGuiMenu*)GetWnd("MainMenu"));

						GUIServerInfo item;
						item.ConvertFromFullName(strWndClicked);
						printf("------------------------------------\n");
						printf("strServerName = %s\n", item.strServerName.c_str());
						printf("strServerIP = %s\n", item.strServerIP.c_str());
						printf("strUserName = %s\n", item.strUserName.c_str());
						printf("strPassword = %s\n", item.strPassword.c_str());
						printf("------------------------------------\n");
						m_pkZeroFps->StartClient(item.strUserName, item.strPassword, item.strServerIP);
					}
					else
					if(pkParentItem && !strcmp(pkParentItem->szNameID, "Menu_Loadmap"))
					{
						char szCommand[512];
						sprintf(szCommand, "load %s", pkItem->pkButton->GetText()); 
						m_pkZeroFps->m_pkConsole->Execute(szCommand);
					}

				}
			}

		}
	}


}

void ZeroEd::OnClickListbox(int iListBoxID, int iListboxIndex, ZGuiWnd* pkMain)
{
	//cout << "ZeroEd::OnClickListbox: iListBoxID=" << iListBoxID << ", iListboxIndex=" << iListboxIndex << endl;

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
				//printf("setting enviroment %s\n", szFull.c_str());
				
				m_iCurrentMarkedZone = GetZoneID(m_kZoneMarkerPos);			
				
				if(m_iCurrentMarkedZone != -1)
				{
					SendSetZoneEnviroment(szFull,m_iCurrentMarkedZone);
					//SetZoneEnviroment( szFull.c_str()  );  
				}
			}
		}
	}

	
	if(strMainWndName == "TerrEditMode")
	{
		if(iListboxIndex == 0)		 m_iHMapEditMode = HMAP_EDITVERTEX;	
		if(iListboxIndex == 1)		 m_iHMapEditMode = HMAP_DRAWSMFLAT;	
		if(iListboxIndex == 2)		 m_iHMapEditMode = HMAP_DRAWMASK;	
		if(iListboxIndex == 3)		 m_iHMapEditMode = HMAP_DRAWVISIBLE;	
		if(iListboxIndex == 4)		 m_iHMapEditMode = HMAP_SMOOTH;
		if(iListboxIndex == 5)		 m_iHMapEditMode = HMAP_STITCH;
	}

	if(strMainWndName == "TerrTexture")
	{
		char *szPreset = static_cast<ZGuiListbox*>(pkListBox)->GetSelItem()->GetText();
	
		m_strHeightmapMaterial = szPreset;
	
// 		m_cDrawTexture = iListboxIndex;
// 		if(m_cDrawTexture < 0)		m_cDrawTexture  = 0;
// 		if(m_cDrawTexture > 255)	m_cDrawTexture  = 255;
	}


	if(strListBox == "PropertyList")
	{
		ResizeWorkTab(257); // change to small to fit addpropery

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
					if(string("Entity") == string(szProperty))
					{
						SetText("PropertyValEb", (char*)pkEnt->Edit_GetDataString(item).c_str());
					}
					
					else if(string("Variables") == string(szProperty))
					{
						SetText("PropertyValEb", (char*)pkEnt->GetVarString(item).c_str());
					}
					
					else if((pkProp = pkEnt->GetProperty(szProperty)))
						SetText("PropertyValEb", (char*)pkProp->GetValue(item).c_str());
				}
	}

	if(strListBox == "ManageServerList")
	{
		ZGuiListbox* pkList = (ZGuiListbox*) GetWnd("ManageServerList");

		if(pkList->GetSelItem() && iListboxIndex < m_vkServerList.size())
		{
			GUIServerInfo item;
			item.ConvertFromFullName(pkList->GetSelItem()->GetText());

			SetText("ServerNameTextbox", (char*)item.strServerName.c_str());
			SetText("ServerIPTextbox", (char*)item.strServerIP.c_str());
			SetText("UserNameTextbox", (char*)item.strUserName.c_str());
			SetText("PasswordTextbox", (char*)item.strPassword.c_str());		
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
			string strFullpath = string("zones/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			m_strActiveZoneName = strFullpath;

			//m_iCurrentMarkedZone = m_pkEntityManager->GetZoneIndex(m_kZoneMarkerPos,-1,false);
			m_iCurrentMarkedZone = GetZoneID(m_kZoneMarkerPos);
			
			// Setting new zone modell
			if(m_iCurrentMarkedZone != -1)	// � n�on zon markerad?
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
		m_iEditMode = EDIT_CREATEOBJECT;	

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

			strFullpath = string("");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			SetText("PropertyValEb", (char*) strFullpath.c_str());

			//AddPropertyVal();

			SetupPreviewWnd(strFullpath.c_str());

			//ShowWnd("SelectFileWnd", false); // close window

			break;

		case SELECT_SOUND:

			strFullpath = string("data/audio/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			SetText("PropertyValEb", (char*) strFullpath.c_str());

			AddPropertyVal();
			
			ShowWnd("SelectFileWnd", false); // close window

			AddPropertyVal(); // m�te anropa dubbelt, I don't know why...

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
			if(GetWnd("wndTerrain"))	GetWnd("wndTerrain")->Hide();
			ShowWnd("SelectFileWnd", false);
			ShowWnd("PreviewWnd", false);
			break;
		case 1:
			m_iEditMode = EDIT_CREATEOBJECT;	
			if(GetWnd("AddNewProperyWnd"))GetWnd("AddNewProperyWnd")->Hide();
			if(GetWnd("EditPropertyWnd"))GetWnd("EditPropertyWnd")->Hide();
			if(GetWnd("wndTerrain"))	GetWnd("wndTerrain")->Hide();
			ShowWnd("SelectFileWnd", false);
         ShowWnd("PreviewWnd", false);
			BuildFileTree("ObjectTree", "data/script/objects", ".lua");
			break;
		case 2:			
			m_iEditMode = EDIT_OBJECTS;
			ShowWnd("SelectFileWnd", false);
			ShowWnd("PreviewWnd", false);
			if(GetWnd("wndTerrain")) GetWnd("wndTerrain")->Hide();
			break;
		case 3:
			ShowWnd("SelectFileWnd", false);
			ShowWnd("PreviewWnd", false);
			if(GetWnd("wndTerrain")) GetWnd("wndTerrain")->Hide();
			if(GetWnd("AddNewProperyWnd"))GetWnd("AddNewProperyWnd")->Hide();
			if(GetWnd("EditPropertyWnd"))GetWnd("EditPropertyWnd")->Hide();
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

void ZeroEd::SetupPreviewWnd(const char* szMadFile)
{
	static Camera* s_pkCamera;

	if(m_pkPreviewEntity == NULL)
	{
		m_pkPreviewEntity = m_pkEntityManager->CreateEntity();
		m_pkPreviewEntity->SetSave(false);
		m_pkPreviewEntity->SetWorldPosV(Vector3(0,0,0));
		m_pkPreviewEntity->AddProperty("P_Mad");

		float aspect = (float) GetWnd("PreviewRenderLabel")->GetScreenRect().Width() /
			(float) GetWnd("PreviewRenderLabel")->GetScreenRect().Height();

		s_pkCamera = new Camera(Vector3(0,0,0),Vector3(0,0,0),70,aspect,0.0025,250);	
		s_pkCamera->SetShadowMap(false);
		s_pkCamera->SetDrawInterface(false);
		s_pkCamera->SetClearViewPort(false);  
		s_pkCamera->SetRootEntityID(m_pkPreviewEntity->GetEntityID());
		GetWnd("PreviewRenderLabel")->SetRenderTarget(s_pkCamera);
		GetWnd("PreviewWnd")->SetMoveArea(Rect(0,0,1024,768), true);
	}

	((P_Mad*)m_pkPreviewEntity->GetProperty("P_Mad"))->SetBase(szMadFile);	

	float r = ((P_Mad*)m_pkPreviewEntity->GetProperty("P_Mad"))->GetRadius(); 
	s_pkCamera->SetPos(Vector3(0,-r/8900,r*2));

	ShowWnd("PreviewWnd", true, true);
}

void ZeroEd::UpdatePreviewObject()
{
	if(m_pkPreviewEntity == NULL)
		return;

	static float s_fRotTimer = 0;
	static float s_fObjRotDelay = 0.009f;
	static Vector3 kRot(0,0,0);
	
	float fTime = m_pkZeroFps->GetEngineTime();

	float fTimeSinceLastFrame = fTime - s_fRotTimer;
	float dif = fTimeSinceLastFrame / s_fObjRotDelay;

	s_fRotTimer = fTime;

	static int dir=0;

	if(kRot.x >= 360 || kRot.y >= 360 || kRot.z >= 360)
	{
		if(dir == 0)
			kRot.z = 0.0f;
		if(dir == 1)
			kRot.y = 0.0f;
		if(dir == 2)
			kRot.x = 0.0f;

		dir++;

		if(dir == 3)
			dir = 0;
	}

	if(dir == 0)
		kRot.z += dif;
	if(dir == 1)
		kRot.y += dif;
	if(dir == 2)
		kRot.x += dif;

	m_pkPreviewEntity->SetWorldRotV(kRot); 
}

void ZeroEd::InitMainMenu()
{
	ZGuiMenu* pkMenu = ((ZGuiMenu*)GetWnd("MainMenu"));
	pkMenu->SetCheckMark("Menu_PageCommon", m_pkZeroFps->DevPrintPageVisible("common"));
	pkMenu->SetCheckMark("Menu_PageConn", m_pkZeroFps->DevPrintPageVisible("conn"));
	pkMenu->SetCheckMark("Menu_PageOm", m_pkZeroFps->DevPrintPageVisible("om"));
	pkMenu->SetCheckMark("Menu_PageServer", m_pkZeroFps->DevPrintPageVisible("server"));
	pkMenu->SetCheckMark("Menu_PageTime", m_pkZeroFps->DevPrintPageVisible("time"));	
	pkMenu->SetCheckMark("Menu_PageEditor", m_pkZeroFps->DevPrintPageVisible("editor"));	
	pkMenu->SetCheckMark("Menu_SysSimToggle", m_pkZeroFps->m_kbRunWorldSim.GetBool());
	
	vector<string> akSections;
	if(m_pkIni->Open("ZeroEd.ini", false))
	{
		pkMenu->SetCheckMark("Menu_DebugShowZones", m_pkIni->GetIntValue("ZSSEntityManager", "l_showzones"));
		pkMenu->SetCheckMark("Menu_DebugShowZoneConn", m_pkIni->GetIntValue("ZSSEntityManager", "l_showconn"));
		pkMenu->SetCheckMark("Menu_SysRender", m_pkIni->GetIntValue("ZSSZeroFps", "r_render"));
		pkMenu->SetCheckMark("Menu_SysLockFps", m_pkIni->GetIntValue("ZSSZeroFps", "e_lockfps"));

		pkMenu->SetCheckMark("Menu_AiNavMesh", m_pkIni->GetIntValue("ZSSAStar", "ai_shownavmesh"));
		pkMenu->SetCheckMark("Menu_AiShowPath", m_pkIni->GetIntValue("ZSSAStar", "ai_showpath"));

		m_pkIni->Close();
	}
}

void ZeroEd::AddLoadFoldersToMenu()
{
	vector<string> vkFolders;
	m_pkBasicFS->ListDir(&vkFolders, m_pkBasicFS->GetCWD() , true);

	ZGuiMenu* pkMenu = ((ZGuiMenu*)GetWnd("MainMenu"));

	for(int i=0; i<vkFolders.size(); i++)
	{
		string strZonesPunktDat = vkFolders[i] + "/" + string("zones.dat");
		if(m_pkBasicFS->FileExist(strZonesPunktDat.c_str()))
		{			
			string strID = string("Menu_Loadmap_") + vkFolders[i].c_str();
			pkMenu->AddItem(vkFolders[i].c_str(), strID.c_str(), "Menu_Loadmap");
		}
	}
	
	pkMenu->ResizeMenu();
}

void ZeroEd::ResizeWorkTab(int iSize)
{
	int x=GetWidth()-273;
	int y=GetHeight()-iSize-50;

	GetWnd("worktab")->SetPos(x, y, true, true); 
	GetWnd("worktab")->Resize(256, iSize, true); 
	GetWnd("WorkTabWnd")->Resize(256, iSize, true); 
	GetWnd("ZoneModelTree")->Resize(200, iSize-57, true);
	GetWnd("ObjectTree")->Resize(200, iSize-57, true);
	GetWnd("EnviromentPresetList")->Resize(200, iSize-57, true);
	GetWnd("PropertyList")->Resize(200, iSize-57, true);
	
}

