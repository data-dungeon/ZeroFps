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
	if(strcmp(szTreeBox, "AnimationFileTree") == 0)
	{
		P_Mad* pkMad = (P_Mad*) m_pkViewObject->GetProperty("P_Mad");		
		Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 

		if(szClickNodeText)
			pkMad->SetAnimation(szClickNodeText, 0);

		m_pkGui->SetFocus(GetWnd("GuiMainWnd"));
	}
}

void MadView::ChangeMad(string strName)
{
	m_strMadFile = strName;

	P_Mad* pkMad = (P_Mad*) m_pkViewObject->GetProperty("P_Mad");

	if(pkMad == NULL)
	{
		pkMad = (P_Mad*) m_pkViewObject->AddProperty("P_Mad");
	}

	pkMad->SetBase(m_strMadFile.c_str());
	
	//sätter skalan så att modellerna alltid är lika stora på skärmen // dvoid
	// ..men det ska dom inte vara, för att man vill kunna se hur stort ett object är,
	// samt att den inte verkar funka något vidare :)
	//pkMad->SetScale(1.0 / (pkMad->GetRadius()*0.2) );

	char szText[100];

	sprintf(szText, "Name: %s", m_strMadFile.c_str());
	SetText("MadNameLabel", szText);

	sprintf(szText, "Vertices: %i", pkMad->GetNumVertices());
	SetText("NumVertsLabel", szText);

	sprintf(szText, "Faces: %i", pkMad->GetNumFaces());
	SetText("NumFacesLabel", szText);

	sprintf(szText, "Radius: %.2f", pkMad->GetRadius());
	SetText("MadRadiusLabel", szText);

	((ZGuiTreebox*)GetWnd("AnimationFileTree"))->Clear(); 

	Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 
	int iNumAnimations = pkCore->GetNumOfAnimations();

	sprintf(szText, "Bones: %i", pkCore->GetNumOfBones());
	SetText("NumBonesLabel", szText);
	
	sprintf(szText, "Animations (%i)", iNumAnimations);
	AddTreeItem("AnimationFileTree", "Animations", "RootNode", szText, 1, 2);

	if(iNumAnimations > 0)
	{
		for(int i=0; i<pkCore->GetNumOfAnimations(); i++)
		{
			string strName = pkCore->GetAnimationName(i); 
			AddTreeItem("AnimationFileTree", strName.c_str(), "Animations", (char*) strName.c_str(), 0, 1);
		}
	}

	printf("Num meshes = %i\n", pkCore->NumOfMeshes());
	printf("Num sub meshes = %i\n", pkCore->GetMeshByID(0)->m_kLodMesh[0].SizeSubMesh() );
	printf("Num textures = %i\n", pkCore->GetMeshByID(0)->m_kLodMesh[0].SizeTextures() );

	for(int i=0; i<pkCore->GetMeshByID(0)->m_kLodMesh[0].SizeTextures(); i++)
	{
		printf("%s\n", pkCore->GetMeshByID(0)->m_kLodMesh[0].GetTextureName(i));
	}
}