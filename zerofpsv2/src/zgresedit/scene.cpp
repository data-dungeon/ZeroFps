// scene.cpp: implementation of the Scene class.
//
//////////////////////////////////////////////////////////////////////

#include "scene.h"
#include "../zerofpsv2/basic/zfsystem.h"
#include "../zerofpsv2/basic/zfbasicfs.h"
#include "../zerofpsv2/basic/zfresourcedb.h"
#include "../zerofpsv2/script/zfscript.h"
#include "../zerofpsv2/render/texturemanager.h"
#include <typeinfo>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Scene::Scene()
{
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
}

Scene::~Scene()
{

}

void Scene::Init(ZGuiApp *pkApp)
{
	m_pkApp = pkApp;
	CreateUI();
}

void Scene::CreateUI()
{
	//
	// Create workspace
	//

	m_pkApp->CreateWnd(Wnd, "WorkSpace", "", "", 800-204, 8, 200, 600-16, 0);
	(m_pkWorkSpace = m_pkApp->GetWnd("WorkSpace"))->SetMoveArea(Rect(-190,-490,800+190,600+490),true);

	m_pkApp->CreateWnd(Button, "UndoButton", "WorkSpace", "", 2, 0, 24, 22, 0);

	((ZGuiButton*)m_pkApp->GetWnd("UndoButton"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("UndoButton"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("UndoButton"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("UndoButton"))->GetButtonUpSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/undo_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("UndoButton"))->GetButtonHighLightSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/undo_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("UndoButton"))->GetButtonDownSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/undo_d.bmp", 0);

	m_pkApp->CreateWnd(Button, "DeleteWnd", "WorkSpace", "", 28, 0, 24, 22, 0);

	((ZGuiButton*)m_pkApp->GetWnd("DeleteWnd"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("DeleteWnd"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("DeleteWnd"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("DeleteWnd"))->GetButtonUpSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/delete_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("DeleteWnd"))->GetButtonHighLightSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/delete_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("DeleteWnd"))->GetButtonDownSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/delete_d.bmp", 0);

	m_pkApp->CreateWnd(Button, "ToogleViewWnd", "WorkSpace", "", 56, 0, 24, 22, 0);

	((ZGuiButton*)m_pkApp->GetWnd("ToogleViewWnd"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("ToogleViewWnd"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("ToogleViewWnd"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("ToogleViewWnd"))->GetButtonUpSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/show_window_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("ToogleViewWnd"))->GetButtonHighLightSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/show_window_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("ToogleViewWnd"))->GetButtonDownSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/show_window_d.bmp", 0);

	m_pkApp->CreateWnd(Button, "OpenBn",  "WorkSpace",  "", 84,  0, 24, 22, 0);

	((ZGuiButton*)m_pkApp->GetWnd("OpenBn"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("OpenBn"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("OpenBn"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("OpenBn"))->GetButtonUpSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/open_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("OpenBn"))->GetButtonHighLightSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/open_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("OpenBn"))->GetButtonDownSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/open_d.bmp", 0);

	m_pkApp->CreateWnd(Button, "SaveBn",  "WorkSpace",  "", 112,  0, 24, 22, 0);

	((ZGuiButton*)m_pkApp->GetWnd("SaveBn"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("SaveBn"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("SaveBn"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("SaveBn"))->GetButtonUpSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/save_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("SaveBn"))->GetButtonHighLightSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/save_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("SaveBn"))->GetButtonDownSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/save_d.bmp", 0);

	m_pkApp->CreateWnd(Label, "NewWindowLabel",  "WorkSpace",  "Create new window", 4,  28, 120, 20, 0);

	m_pkApp->CreateWnd(Combobox, "SelectWndType", "WorkSpace", "", 4, 48, 100, 16, 0);
	m_pkApp->AddListItem("SelectWndType", "Wnd");
	m_pkApp->AddListItem("SelectWndType", "Button");
	m_pkApp->AddListItem("SelectWndType", "Checkbox");
	m_pkApp->AddListItem("SelectWndType", "Combobox");
	m_pkApp->AddListItem("SelectWndType", "Label");
	m_pkApp->AddListItem("SelectWndType", "Listbox");
	m_pkApp->AddListItem("SelectWndType", "Radiobn.group");
	m_pkApp->AddListItem("SelectWndType", "Radiobutton");
	m_pkApp->AddListItem("SelectWndType", "H.Scrollbar");
	m_pkApp->AddListItem("SelectWndType", "V.Scrollbar");
	m_pkApp->AddListItem("SelectWndType", "Slider");
	m_pkApp->AddListItem("SelectWndType", "TabControl");
	m_pkApp->AddListItem("SelectWndType", "S.Textbox");
	m_pkApp->AddListItem("SelectWndType", "M.Textbox");
	m_pkApp->AddListItem("SelectWndType", "Treebox");

	((ZGuiCombobox*)m_pkApp->GetWnd("SelectWndType"))->SetNumVisibleRows(15);
	((ZGuiCombobox*)m_pkApp->GetWnd("SelectWndType"))->SetLabelText("Wnd"); 
	((ZGuiCombobox*)m_pkApp->GetWnd("SelectWndType"))->GetListbox()->SelItem(0); 

	m_pkApp->CreateWnd(Label, "NewNameLabel",  "WorkSpace",  "Name:", 4,  72, 25, 20, 0);
	m_pkApp->CreateWnd(Textbox, "NewWndNameTextbox", "WorkSpace", "Nonmame", 40,  72, 150, 20, 0);

	m_pkApp->CreateWnd(Button, "CreateWndBn", "WorkSpace", "OK", 108, 48, 80, 20, 0);

	((ZGuiButton*)m_pkApp->GetWnd("CreateWndBn"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("CreateWndBn"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("CreateWndBn"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("CreateWndBn"))->GetButtonUpSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bn_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("CreateWndBn"))->GetButtonHighLightSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bn_f.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("CreateWndBn"))->GetButtonDownSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bn_d.bmp", 0);

	m_pkApp->CreateWnd(Treebox, "TextureTree", "WorkSpace", "", 8,105,200-16,250,0);
	BuildFileTree("TextureTree", "data/textures/gui", ".bmp");

	ZGuiSkin* pkNewSkin = new ZGuiSkin();
	pkNewSkin->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bitmap.bmp", 0);
	((ZGuiTreebox*) m_pkApp->GetWnd("TextureTree"))->InsertBranchSkin(0, pkNewSkin, true);  

	pkNewSkin = new ZGuiSkin();
	pkNewSkin->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/folder_closed.bmp", 0);
	((ZGuiTreebox*) m_pkApp->GetWnd("TextureTree"))->InsertBranchSkin(1, pkNewSkin, true);  

	pkNewSkin = new ZGuiSkin();
	pkNewSkin->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/folder_open.bmp", 0);
	((ZGuiTreebox*) m_pkApp->GetWnd("TextureTree"))->InsertBranchSkin(2, pkNewSkin, true);  

	m_pkApp->CreateWnd(Radiobutton, "TextureBK", "WorkSpace", "Background", 2, 367, 100, 16, 0);
	m_pkApp->CreateWnd(Radiobutton, "TextureBdH", "WorkSpace", "Border, horz", 2, 385, 100, 16, 0);
	m_pkApp->CreateWnd(Radiobutton, "TextureBdV", "WorkSpace", "Border, vert", 2, 403, 100, 16, 0);
	m_pkApp->CreateWnd(Radiobutton, "TextureBdC", "WorkSpace", "Border, corner", 2, 421, 100, 16, 0);

	m_pkApp->CreateWnd(Radiobutton, "TextureAlphaBK", "WorkSpace", "(A)", 122, 367, 36, 16, 0);
	m_pkApp->CreateWnd(Radiobutton, "TextureAlphaBdH", "WorkSpace", "(A)", 122, 385, 36, 16, 0);
	m_pkApp->CreateWnd(Radiobutton, "TextureAlphaBdV", "WorkSpace", "(A)", 122, 403, 36, 16, 0);
	m_pkApp->CreateWnd(Radiobutton, "TextureAlphaBdC", "WorkSpace", "(A)", 122, 421, 36, 16, 0);

	m_pkApp->CreateWnd(Label, "SkinTypeCBLabel",  "WorkSpace",  "Skin type", 2,  440, 190, 19, 0);
	m_pkApp->CreateWnd(Combobox, "SkinTypeCB", "WorkSpace", "Skin type", 2, 460, 190, 20, 0);

	m_pkApp->CreateWnd(Checkbox, "StretchTextureCB", "WorkSpace", "Stretch", 2, 484, 16, 16, 0);
	m_pkApp->CreateWnd(Checkbox, "TransparentTextureCB", "WorkSpace", "Transparent", 100, 484, 16, 16, 0);

	m_pkApp->CreateWnd(Button, "RemoveTextureBn",  "WorkSpace",  "No tex", 120,  440, 50, 16, 0);
	
	((ZGuiButton*)m_pkApp->GetWnd("RemoveTextureBn"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("RemoveTextureBn"))->SetButtonDownSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("RemoveTextureBn"))->SetButtonHighLightSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("RemoveTextureBn"))->GetButtonUpSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bn_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("RemoveTextureBn"))->GetButtonDownSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bn_d.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("RemoveTextureBn"))->GetButtonHighLightSkin()->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bn_f.bmp", 0);

	((ZGuiRadiobutton*) m_pkApp->GetWnd("TextureBK"))->GetButton()->CheckButton(); 

	//
	// Create Def.Properties Wnd
	//
	m_pkApp->CreateWnd(Wnd, "DefPropWnd", "", "", 382, 8, 200, 200, 0);
	(m_pkDefProp = m_pkApp->GetWnd("DefPropWnd"))->SetMoveArea(Rect(-190,-190,800+190,600+190),true);
	m_pkApp->CreateWnd(Button, "DefPropWndCancelBn",  "DefPropWnd",  "Cancel", 200-50,  180, 45, 16, 0);
	m_pkApp->CreateWnd(Button, "DefPropWndOKBn",  "DefPropWnd",  "OK", 200-100,  180, 45, 16, 0);
	m_pkGui->ShowMainWindow(m_pkDefProp, false);
	

	//
	// Create toolbar
	//
	m_pkApp->CreateWnd(Wnd, "PropertyWnd", "", "", 800/2-300/2, 600-138, 300, 110, 0);
	(m_pkPropertyWnd = m_pkApp->GetWnd("PropertyWnd"))->SetMoveArea(Rect(-290,-100,800+290,600+100),true);

	m_pkApp->CreateWnd(Label, "NameLabel",  "PropertyWnd",  "Name:", 4,  4+2, 25, 20, 0);
	m_pkApp->CreateWnd(Textbox, "WndNameTextbox", "PropertyWnd", "", 40,  4, 250, 20, 0);

	m_pkApp->CreateWnd(Label, "TextLabel",  "PropertyWnd",  "Title:", 4,  28+2, 25, 20, 0);
	m_pkApp->CreateWnd(Textbox, "WndTitleTextbox", "PropertyWnd", "", 40,  28, 250, 20, 0);

	m_pkApp->CreateWnd(Button, "UpdateWndDataBn", "PropertyWnd", "OK", 300-44, 110-24, 40, 20, 0);

	m_pkApp->CreateWnd(Button, "ResizeWndToTexBn", "PropertyWnd", "", 300-44-28, 110-24, 24, 22, 0);

	((ZGuiButton*)m_pkApp->GetWnd("ResizeWndToTexBn"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("ResizeWndToTexBn"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("ResizeWndToTexBn"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("ResizeWndToTexBn"))->GetButtonUpSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/scale_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("ResizeWndToTexBn"))->GetButtonHighLightSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/scale_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("ResizeWndToTexBn"))->GetButtonDownSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/scale_d.bmp", 0);

	((ZGuiButton*)m_pkApp->GetWnd("UpdateWndDataBn"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("UpdateWndDataBn"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("UpdateWndDataBn"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("UpdateWndDataBn"))->GetButtonUpSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bn_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("UpdateWndDataBn"))->GetButtonHighLightSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bn_f.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("UpdateWndDataBn"))->GetButtonDownSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/bn_d.bmp", 0);

	m_pkApp->CreateWnd(Label, "PosXLabel",   "PropertyWnd", "X", 4,  54+2, 20, 20, 0);
	m_pkApp->CreateWnd(Textbox, "PosXTextbox",   "PropertyWnd", "", 20,  54, 30, 20, 0);

	m_pkApp->CreateWnd(Label, "PosYLabel",   "PropertyWnd", "Y", 55, 54+2, 20, 20, 0);
	m_pkApp->CreateWnd(Textbox, "PosYTextbox",   "PropertyWnd", "", 75, 54, 30, 20, 0);
	
	m_pkApp->CreateWnd(Label, "WidthLabel",  "PropertyWnd",  "W", 110,  54+2, 20, 20, 0);
	m_pkApp->CreateWnd(Textbox, "WidthTextbox",  "PropertyWnd", "", 130,  54, 30, 20, 0);

	m_pkApp->CreateWnd(Label, "HeightLabel", "PropertyWnd",  "H", 165, 54+2, 20, 20, 0);
	m_pkApp->CreateWnd(Textbox, "HeightTextbox", "PropertyWnd", "", 185, 54, 30, 20, 0);

	m_pkApp->CreateWnd(Label, "BdSizeLabel", "PropertyWnd",  "Border", 223, 54+2, 35, 20, 0);
	m_pkApp->CreateWnd(Textbox, "BdSizeTextbox", "PropertyWnd", "", 270, 54, 20, 20, 0);

	m_pkApp->CreateWnd(Label, "ParentLabel2", "PropertyWnd",  "Parent:", 4, 80+2, 50, 20, 0);
	m_pkApp->CreateWnd(Label, "ParentLabel", "PropertyWnd",  "", 54, 80+2, 150, 20, 0);

	//
	// Create view window
	//
	m_pkApp->CreateWnd(Wnd, "ViewWindow", "",  "", 580, 50, 200, 200, 0);
	(m_pkViewWindow = m_pkApp->GetWnd("ViewWindow"))->SetMoveArea(Rect(-190,-190,800+190,600+190),true);
	m_pkViewWindow->Hide();

	m_pkApp->CreateWnd(Checkbox, "ShowHideWndCB", "ViewWindow",  "Visible", 8, 8, 16, 16, 0);
	m_pkApp->CreateWnd(Listbox, "MainWndList", "ViewWindow",  "", 8, 50, 200-16, 150-8, 0);

	m_pkApp->CreateWnd(Button, "CloseViewWnd", "ViewWindow", "", 200-20, 4, 16, 16, 0);

	((ZGuiButton*)m_pkApp->GetWnd("CloseViewWnd"))->SetButtonUpSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("CloseViewWnd"))->SetButtonHighLightSkin(new ZGuiSkin());
	((ZGuiButton*)m_pkApp->GetWnd("CloseViewWnd"))->SetButtonDownSkin(new ZGuiSkin());

	((ZGuiButton*)m_pkApp->GetWnd("CloseViewWnd"))->GetButtonUpSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/delete_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("CloseViewWnd"))->GetButtonHighLightSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/delete_u.bmp", 0);
	((ZGuiButton*)m_pkApp->GetWnd("CloseViewWnd"))->GetButtonDownSkin()->
		m_iBkTexID = m_pkTexMan->Load("data/textures/gui/delete_d.bmp", 0);

	m_pkGui->ShowMainWindow(m_pkViewWindow, false);

	//
	// Create select file window
	//
	m_pkApp->CreateWnd(Wnd, "SelectFileWnd", "SelectFile", "", 180, 8, 400, 400, 0);
	(m_pkSelectFileWnd = m_pkApp->GetWnd("SelectFileWnd"))->SetMoveArea(Rect(-390, -390, 800+390, 600+390),true);
	m_pkGui->ShowMainWindow(m_pkSelectFileWnd, true);

	m_pkApp->CreateWnd(Treebox, "FileTree", "SelectFileWnd", "", 8,8,400-16,400-16-64+15+2,0);

	pkNewSkin = new ZGuiSkin();
	pkNewSkin->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/textfile.bmp", 0);
	((ZGuiTreebox*) m_pkApp->GetWnd("FileTree"))->InsertBranchSkin(0, pkNewSkin, true);  

	pkNewSkin = new ZGuiSkin();
	pkNewSkin->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/folder_closed.bmp", 0);
	((ZGuiTreebox*) m_pkApp->GetWnd("FileTree"))->InsertBranchSkin(1, pkNewSkin, true);  

	pkNewSkin = new ZGuiSkin();
	pkNewSkin->m_iBkTexID = m_pkTexMan->Load("data/textures/gui/folder_open.bmp", 0);
	((ZGuiTreebox*) m_pkApp->GetWnd("FileTree"))->InsertBranchSkin(2, pkNewSkin, true); 
	
	m_pkApp->CreateWnd(Textbox, "SelectedFileEB", "SelectFileWnd", "", 0,16+400-16-64+15,400,20,0);

	m_pkApp->CreateWnd(Button, "SelectedFileCancel", "SelectFileWnd", "Cancel", 400-68,16+400-16-64+24+15,60,20,0);
	m_pkApp->CreateWnd(Button, "SelectedFileOK", "SelectFileWnd", "OK", 400-140,16+400-16-64+24+15,60,20,0);
	m_pkApp->CreateWnd(Button, "SelectedFileUpdate", "SelectFileWnd", "(update list)", 8,16+400-16-64+24+15,92,20,0);
	
	m_pkGui->ShowMainWindow(m_pkSelectFileWnd, false);

	((ZGuiTreebox*)m_pkApp->GetWnd("FileTree"))->Clear();
	BuildFileTree("FileTree", "data", ".lua");
	m_pkApp->SetText("SelectedFileEB", "");

	m_pkApp->CreateWnd(Label, "OwerwriteWarning", "SelectFileWnd", "", 400-240, 16+400-16-64+24+15, 100, 20, 0);
	
}

bool Scene::BuildFileTree(char* szTreeBoxName, char* szRootPath, char* szExtension)
{
	// kolla inparametrar
	if(szRootPath == NULL || szTreeBoxName == NULL)
		return false;

	// sista tecknet får inte vara ett '/' tecken
	if(szRootPath[strlen(szRootPath)] == '/')
		szRootPath[strlen(szRootPath)] = '\0';

	set<string> kSearchedFiles;
	list<string> dir_list;
	string strPrevNode;

	dir_list.push_back(string(szRootPath));
	strPrevNode = "RootNode";

	ZFVFileSystem* pkZFVFileSystem = static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));
	
	while(1)
	{
		list<string> vkFileNames;
		string currentFolder = dir_list.back();

		// Hämta filerna i den aktuella katalogen och sortera listan.
		vector<string> t;
		pkZFVFileSystem->ListDir(&t, currentFolder);
		for(unsigned int i=0; i<t.size(); i++)
			vkFileNames.push_back(t[i]); 
		t.clear(); vkFileNames.sort(SortFiles);

		// Lägg till alla filer
		for(list<string>::iterator it = vkFileNames.begin(); it != vkFileNames.end(); it++)  
		{
			string strLabel = (*it);
			string id = currentFolder + string("/") + strLabel;

			bool bIsFolder = strLabel.find(".") == string::npos;

			if(kSearchedFiles.find(id) == kSearchedFiles.end())
			{			
				if(bIsFolder)
				{
					string d = currentFolder + string("/") + strLabel;
					dir_list.push_back(d);

					m_pkApp->AddTreeItem(szTreeBoxName, id.c_str(), 
						strPrevNode.c_str(), (char*) strLabel.c_str(), 1, 2);
				}
				else
				{
					bool bCorectExt = true;

					if(szExtension != NULL)
						if(strLabel.find(szExtension) == string::npos)
							bCorectExt = false;

					if(bCorectExt)
						m_pkApp->AddTreeItem(szTreeBoxName, id.c_str(), 
							strPrevNode.c_str(), (char*) strLabel.c_str(), 0, 1);	
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

		strPrevNode = dir_list.back();
	}

	return true;

}

bool Scene::IsSceneWnd(ZGuiWnd* pkWnd)
{
	if(pkWnd == m_pkPropertyWnd)
		return true;
	if(pkWnd == m_pkWorkSpace)
		return true;
	if(pkWnd == m_pkViewWindow)
		return true;
	if(pkWnd == m_pkSelectFileWnd)
		return true;
	if(pkWnd == m_pkDefProp)
		return true;
	
	ZGuiWnd* pkParent = pkWnd->GetParent();

	if(pkParent)
		return IsSceneWnd(pkParent);

	return false;
}

bool Scene::RenameWnd(ZGuiWnd *pkWnd, const char *szName)
{
	ZGuiWnd* pkExistingWnd = m_pkGui->GetResMan()->Wnd(string(szName));

	if( pkExistingWnd == NULL || pkExistingWnd == pkWnd ) // om inget annat fönster heter så...
	{
		map<ZGuiWnd*, string>::iterator itWnd;
		for(itWnd = m_kNickNameWnds.begin(); itWnd != m_kNickNameWnds.end(); itWnd++)
		{
			if(itWnd->second == szName && itWnd->first != pkWnd) 
			{
				return false; // avbryt om ett annat fönster är döpt till det..
			}
		}

		map<ZGuiWnd*, string>::iterator itWnd2;
		itWnd2 = m_kNickNameWnds.find(pkWnd);
		if(itWnd2 != m_kNickNameWnds.end())
		{
			itWnd2->second = szName; // fönstret fanns redan med i tabellen... byt namn
		}
		else
		{
			// lägg till fönstret till aliastabellen
			m_kNickNameWnds.insert( 
				map<ZGuiWnd*, string>::value_type(pkWnd, string(szName)) );
		}

		return true;
	}

	return false;
}

const char* Scene::GetAlias(ZGuiWnd *pkWnd)
{
	map<ZGuiWnd*, string>::iterator itWnd;
	itWnd = m_kNickNameWnds.find(pkWnd);
	if(itWnd != m_kNickNameWnds.end())
	{
		return itWnd->second.c_str();
	}	

	return NULL;
}

ZGuiWnd* Scene::GetWnd(const char* szName)
{
	map<ZGuiWnd*, string>::iterator itWnd;
	for(itWnd = m_kNickNameWnds.begin(); itWnd != m_kNickNameWnds.end(); itWnd++)
	{
		if(itWnd->second == szName) 
		{
			return itWnd->first; // avbryt om ett annat fönster är döpt till det..
		}
	}

	return m_pkApp->GetWnd(szName);
}

void Scene::RemoveAlias(ZGuiWnd *pkWnd)
{
	map<ZGuiWnd*, string>::iterator itWnd;
	itWnd = m_kNickNameWnds.find(pkWnd);
	if(itWnd != m_kNickNameWnds.end())
	{
		m_kNickNameWnds.erase(itWnd);
	}	
}

void Scene::ScaleWndToTexSize(ZGuiWnd *pkWnd, char *szSelSkinType)
{
	vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
	pkWnd->GetWndSkinsDesc(vkSkinDesc);

	for(unsigned int i=0; i<vkSkinDesc.size(); i++)
		if(strcmp(szSelSkinType, vkSkinDesc[i].second.c_str()) == 0)
		{
			ZGuiSkin* pkSkin = (*vkSkinDesc[i].first);

			if(pkSkin)
			{
				if(pkSkin->m_iBkTexID)
				{
					m_pkTexMan->BindTexture(pkSkin->m_iBkTexID);
					m_pkTexMan->EditStart(pkSkin->m_iBkTexID);
					Image* surface = m_pkTexMan->EditGetImage(pkSkin->m_iBkTexID);
					pkWnd->Resize(surface->m_iWidth, surface->m_iHeight);
					m_pkTexMan->EditEnd(pkSkin->m_iBkTexID);
				}
			}

			break;
		}
}

/*ZGuiWnd* Scene::DeleteWnd(ZGuiWnd *pkWnd)
{
	ZGuiWnd* pkReturnWnd = NULL;

	if(pkWnd != NULL)
	{
		if(pkWnd->GetParent())
		{
			pkReturnWnd = pkWnd->GetParent();

			RemoveAlias(pkWnd);

			if(m_pkApp->GetType(pkWnd->GetParent()) == TabControl)
			{
				ZGuiTabCtrl* pkTabCtrl =  ((ZGuiTabCtrl*) pkWnd->GetParent());

				unsigned int current_page = pkTabCtrl->GetCurrentPage(); 
				pkTabCtrl->DeletePage(current_page);

				if(pkTabCtrl->GetNumPages() != 0)
				{
					int new_page = current_page-1;
					if(new_page < 0)
						new_page = 0;
					
					pkTabCtrl->SetCurrentPage(new_page); 	
					ZGuiWnd* pkPage = pkTabCtrl->GetPage(new_page);
					m_pkGui->SetFocus(pkPage);
					return pkPage;
				}
			}
		}

		m_pkGui->UnregisterWindow(pkWnd);
		pkWnd = NULL;
	}
	
	return pkReturnWnd;
}*/

ZGuiWnd* Scene::CloneWnd(ZGuiWnd *pkWnd, int xpos, int ypos)
{
	ZGuiWnd* pkNewWnd = NULL;

	GuiType eWndType = m_pkApp->GetType(pkWnd);
	char szParent[100] = "";
	char szLabel[100];
	char szName[100];

	ZGuiWnd* pkParent = pkWnd->GetParent();

	int iWidth = pkWnd->GetScreenRect().Width();
	int iHeight = pkWnd->GetScreenRect().Height();
	
	if(pkParent)
	{
		if(GetAlias(pkParent))
			strcpy(szParent, GetAlias(pkParent));
		else
			strcpy(szParent, pkParent->GetName());

		xpos -= pkParent->GetScreenRect().Left;
		ypos -= pkParent->GetScreenRect().Top;

		if(xpos + iWidth > pkParent->GetScreenRect().Width() || 
			ypos + iHeight > pkParent->GetScreenRect().Height() || 
			xpos < 0 || ypos < 0)
			return NULL;
	}

	if(eWndType == Wnd)				strcpy(szName, "Wnd");
	else if(eWndType == Button)			strcpy(szName, "Button");
	else if(eWndType == Checkbox)		strcpy(szName, "Checkbox");
	else if(eWndType == Combobox)		strcpy(szName, "Combobox");
	else if(eWndType == Label)			strcpy(szName, "Label");
	else if(eWndType == Listbox)			strcpy(szName, "Listbox");
	else if(eWndType == Radiobutton)	strcpy(szName, "Radiobutton");
	else if(eWndType == Scrollbar)		strcpy(szName, "Scrollbar");
	else if(eWndType == Slider)			strcpy(szName, "Slider");
	else if(eWndType == TabControl)		strcpy(szName, "TabControl");
	else if(eWndType == Textbox)			strcpy(szName, "Textbox");
	else if(eWndType == Treebox)			strcpy(szName, "Treebox");

	strcpy(szLabel, pkWnd->GetText());
	char szNumber[10];
	sprintf(szNumber, "%i", rand() % 1000);
	strcat(szName, szNumber);

	if( (pkNewWnd = m_pkApp->CreateWnd( eWndType, szName,  szParent, szLabel, xpos,  
		ypos, iWidth, iHeight, 0)) == NULL)
	{
		printf("Failed to create window!\n");
	}

	vector<ZGuiWnd::SKIN_DESC> vkSkinDesc;
	pkWnd->GetWndSkinsDesc(vkSkinDesc);

	for(unsigned int i=0; i<vkSkinDesc.size(); i++)
	{
		vector<ZGuiWnd::SKIN_DESC> vkNewSkinDesc;
		pkNewWnd->GetWndSkinsDesc(vkNewSkinDesc);

		for(unsigned int j=0; j<vkNewSkinDesc.size(); j++)
		{	
			if(j==i)
			{
				ZGuiSkin* pkOldSkin = (*vkNewSkinDesc[j].first);

//				måste kommentera ut... krashar av någon anledning (borde inte göra det)
//				if(pkOldSkin)
//					delete pkOldSkin;	

				(*vkNewSkinDesc[j].first) = new ZGuiSkin((*vkSkinDesc[i].first));

				break;
			}
		}

		vkNewSkinDesc.clear(); 
	}

	AddStandardElements(pkNewWnd);

	pkNewWnd->Disable();

	return pkNewWnd;
}

void Scene::AddStandardElements(ZGuiWnd *pkWnd)
{
	GuiType eWndType = m_pkApp->GetType(pkWnd);

	char* szName = (char*) pkWnd->GetName();

	//
	// Lägg till element till listboxar och trädboxar
	//
	if(eWndType == Treebox)
	{
		((ZGuiTreebox*) GetWnd(szName))->Clear();

		m_pkApp->AddTreeItem(szName, "ParentNode", "RootNode", "Parent", 1, 2);

		char szNodeName[50], szNodeID[50];

		for(int i=0; i<25; i++)
		{	
			sprintf(szNodeID, "ChildNode%i", i);
			sprintf(szNodeName, "This is Child number %i", i);
			m_pkApp->AddTreeItem(szName, szNodeID, "ParentNode", szNodeName, 0, 1);
		}
	}
	else
	if(eWndType == Listbox || eWndType == Combobox)
	{
		m_pkApp->ClearListbox(szName);

		char szText[50];

		for(int i=0; i<20; i++)
		{
			sprintf(szText, "Item%i", i);
			m_pkApp->AddListItem(szName, szText);
		}
	}
}