#include "madview.h"
#include "../../../zerofpsv2/gui/zgui.h"
#include "../../../zerofpsv2/basic/zguifont.h"

void MadView::SetupGuiEnviroment()
{
	// Create from script.
	LoadGuiFromScript("data/script/gui/madview.lua");

	m_pkGui->SetCursor( 0,0, m_pkTexMan->Load("data/textures/gui/blue_cursor.bmp", 0),
		m_pkTexMan->Load("data/textures/gui/blue_cursor_a.bmp", 0), 32, 32);

	BuildFileTree("SelectFileTree", "data/mad", ".mad");
	ShowWnd("SelectFileWnd", false);

	((ZGuiTreebox*)GetWnd("SelectFileTree"))->GetItemSkin(0)->m_iBkTexID = 
		m_pkTexMan->Load("data/textures/gui/tn_c.tga", 0);
	((ZGuiTreebox*)GetWnd("SelectFileTree"))->GetItemSkin(1)->m_iBkTexID = 
		m_pkTexMan->Load("data/textures/gui/tn_pc.tga", 0);
	((ZGuiTreebox*)GetWnd("SelectFileTree"))->GetItemSkin(2)->m_iBkTexID = 
		m_pkTexMan->Load("data/textures/gui/tn_po.tga", 0);

	GetWnd("SelectFileWnd")->m_bUseAlhpaTest = false;
	
}

void MadView::OnCommand(int iID, bool bRMouseBnClick, ZGuiWnd *pkMainWnd)
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

		if(strMainWnd == "MainMenu")
		{
			if(!m_pkIni->Open("data/script/gui/menu_madview.txt", false))
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

			m_pkGui->SetFocus(GetWnd("GuiMainWnd"));
			m_pkIni->Close();
		}
	}
}

void MadView::OnClickListbox(int iListBoxID, int iListboxIndex, ZGuiWnd* pkMain)
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

}

void MadView::OnClickTreeItem(char *szTreeBox, char *szParentNodeText, 
											char *szClickNodeText, bool bHaveChilds)
{
	if(strcmp(szTreeBox, "SelectFileTree") == 0)
	{
		string strFullpath;

		if(m_iSelFileMode == SEL_MAD)
		{
			strFullpath = string("data/mad/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			if(strFullpath.find(".mad") != string::npos)
			{
				ChangeMad(strFullpath);
				ShowWnd("SelectFileWnd", false); // close window
			}
		}
		else
		if(m_iSelFileMode == SEL_MATERIAL)
		{
			strFullpath = string("data/material/");

			if(szParentNodeText)
				strFullpath += string(szParentNodeText);

			if(szClickNodeText)
				strFullpath += string(szClickNodeText);

			if(strFullpath.find(".zmt") != string::npos)
			{
				P_Mad* pkMad = (P_Mad*) m_pkViewObject->GetProperty("P_Mad");

				Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 

				pkCore->GetMeshByID(0)->GetLODMesh(0)->SetTextureHandle(
					m_pkSelMesh->iTextureIndex, strFullpath);

				ShowWnd("SelectFileWnd", false); // close window
			}
		}
	}
	else
	if(strcmp(szTreeBox, "AnimationFileTree") == 0)
	{
		P_Mad* pkMad = (P_Mad*) m_pkViewObject->GetProperty("P_Mad");		
		Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 

		if(szClickNodeText)
			pkMad->SetAnimation(szClickNodeText, 0);

		m_pkGui->SetFocus(GetWnd("GuiMainWnd"));

		unsigned int min, max, pos;
		((ZGuiTreebox*)GetWnd("AnimationFileTree"))->GetScrollbar(true)->GetScrollInfo(min,max,pos);
		GetWnd("MaterialFileTree")->SetPos(2,120+max*20,false,true);
	}
	else
	if(strcmp(szTreeBox, "MaterialFileTree") == 0)
	{
		string strText = szClickNodeText;

		if(szClickNodeText && strText.find(string(" : ")) != string::npos)
		{
			P_Mad* pkMad = (P_Mad*) m_pkViewObject->GetProperty("P_Mad");
			Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 

			string format = strText;
			format.erase(0,5);
			int pos = format.find(":");
			format.erase(pos, format.length()-pos); 
			m_pkSelMesh = pkCore->GetMeshByID(0)->GetLODMesh(0)->GetSubMesh(atoi(format.c_str()));

			if(m_iSelFileMode != SEL_MATERIAL)
				BuildFileTree("SelectFileTree", "data/material", ".zmt");

			m_iSelFileMode = SEL_MATERIAL;
			ShowWnd("SelectFileWnd", true);
			ShowWnd("MadViewInfoWnd", false);
		}
	}

	// Resize view window
	int h = GetWnd("MaterialFileTree")->GetScreenRect().Top;

	unsigned int min, max, pos;
	((ZGuiTreebox*)GetWnd("MaterialFileTree"))->GetScrollbar(true)->GetScrollInfo(min,max,pos);
	h += max*20;

	GetWnd("MadViewInfoWnd")->Resize(-1, h); 
}

void MadView::ChangeMad(string strName)
{
	cout<<"loading mad:"<<strName<<endl;

	P_Mad* pkMad = (P_Mad*) m_pkViewObject->GetProperty("P_Mad");
	
	// Change mad
	m_strMadFile = strName;
	pkMad->SetBase(strName.c_str());

	m_fObjectRadius = 0.5;
		
	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 

	ZFAssert(pkCore, "Failed to get Mad_Core pointer!\n");
	if(pkCore == NULL)
		return;

	//dvoids flytta kameran hax :D
	m_pkCameraObject->SetWorldPosV(Vector3(0,-2.5,-pkMad->GetRadius()*2));			
	m_fObjectRadius = pkMad->GetRadius();
	
	char szText[100];

	// Update information in textboxes
	string strFormated = m_strMadFile;
	strFormated.erase(0, strlen("data/mad/"));
	sprintf(szText, "Name: %s", strFormated.c_str());
	SetText("MadNameLabel", szText, true);

	sprintf(szText, "Vertices: %i", pkMad->GetNumVertices());
	SetText("NumVertsLabel", szText);

	sprintf(szText, "Faces: %i", pkMad->GetNumFaces());
	SetText("NumFacesLabel", szText);

	sprintf(szText, "Radius: %.2f", pkMad->GetRadius());
	SetText("MadRadiusLabel", szText);

	// Add items to animations list
	((ZGuiTreebox*)GetWnd("AnimationFileTree"))->Clear(); 

	int iNumAnimations = pkCore->GetNumOfAnimations();

	sprintf(szText, "Bones: %i", pkCore->GetNumOfBones());
	SetText("NumBonesLabel", szText);
	
	sprintf(szText, "Animations (%i)", iNumAnimations);
	AddTreeItem("AnimationFileTree", "Animations", "RootNode", szText, 1, 2);

	for(int i=0; i<iNumAnimations; i++)
	{
		string strName = pkCore->GetAnimationName(i); 
		AddTreeItem("AnimationFileTree", strName.c_str(), "Animations", (char*) strName.c_str(), 0, 1);
	}

	// Add items to material list
	GetWnd("MaterialFileTree")->SetPos(2,120+20,false,true);

	((ZGuiTreebox*)GetWnd("MaterialFileTree"))->Clear(); 
	
	sprintf(szText, "Materials (%i)", pkCore->GetMeshByID(0)->m_kLodMesh[0].SizeTextures());
	AddTreeItem("MaterialFileTree", "Materials", "RootNode", szText, 1, 2);

	for(int i=0; i<pkCore->GetMeshByID(0)->m_kLodMesh[0].SizeTextures(); i++)
	{
		char mesh[64];
		sprintf(mesh, "mesh %i", i);
		string strName = mesh + string(" : ") + pkCore->GetMeshByID(0)->m_kLodMesh[0].GetTextureName(i);
		AddTreeItem("MaterialFileTree", strName.c_str(), "Materials", (char*) strName.c_str(), 0, 1);
	}

	// Resize view window
	int w = 50, h = GetWnd("MaterialFileTree")->GetScreenRect().Top + 20;
	char* wnds[] = {"CameraPosLabel","MadNameLabel"};
	for(int i=0; i<sizeof(wnds)/sizeof(wnds[0]); i++)
	{
		ZGuiWnd* pkWnd = GetWnd(wnds[i]);
		if(pkWnd->GetScreenRect().Width() > w) w = pkWnd->GetScreenRect().Width();
	}
	GetWnd("MadViewInfoWnd")->Resize(w+10, h); 

}