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
			m_strMadFile = strFullpath;

			P_Mad* pkMad = (P_Mad*) m_pkViewObject->GetProperty("P_Mad");
			pkMad->SetBase(m_strMadFile.c_str());
			pkMad->SetScale(1);

			ShowWnd("SelectFileWnd", false); // close window
		}
	}
}

void MadView::OnClickTabPage(ZGuiTabCtrl *pkTabCtrl, int iNewPage, int iPrevPage)
{
	
}
