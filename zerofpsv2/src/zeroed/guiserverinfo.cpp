#include "guiserverinfo.h"
#include <iostream.h>
#include "zeroed.h"

bool GUIServerInfo::ConvertFromFullName(string strFullName)
{
	strServerName = strFullName;
	strServerIP = strFullName;
	strUserName = strFullName;
	strPassword = strFullName;

	int p = 0 , l = strFullName.length();
	
	p = strServerName.find("|"); if(p == string::npos) return false;
	strServerName.erase(p, strServerName.length()-p);

	strServerIP.erase(0, strServerName.length()+1);
	p = strServerIP.find("|"); if(p == string::npos) return false;
	strServerIP.erase(p, strServerIP.length()-p);

	strUserName.erase(0, strServerIP.length()+1+strServerName.length()+1);
	p = strUserName.find("|"); if(p == string::npos) return false;
	strUserName.erase(p, strUserName.length()-p);

	strPassword.erase(0, strServerIP.length()+1+strServerName.length()+1+strUserName.length()+1);

	if(strPassword[strPassword.size()-1] == '\n')
		strPassword.erase(strPassword.size()-1, 1);

	return true;
}


void GUIServerInfo::Save(vector<GUIServerInfo>& rkServerAray)
{
	FILE* pkFile = pkFile = fopen("serverlist.txt", "wt");
	if(pkFile)
	{
		const int servers = rkServerAray.size();
		for(int i=0; i<servers; i++)
		{
			fprintf(pkFile, "%s", rkServerAray[i].FullName().c_str());

			if(i != servers-1)
				fprintf(pkFile, "\n");
		}

		fclose(pkFile);
	}
}

void GUIServerInfo::Load(vector<GUIServerInfo>& rkServerAray)
{
	FILE* pkFile = fopen("serverlist.txt", "rt");

	if(pkFile)
	{
		rkServerAray.clear();

		char strLine[512];
		while (!feof(pkFile))
		{
			if(fgets(strLine, 512, pkFile))
			{
				if(strlen(strLine) > 1)
				{
					GUIServerInfo info;
					if(info.ConvertFromFullName(strLine))
						rkServerAray.push_back(info);
				}
			}
		}
	}

	if(pkFile)
		fclose(pkFile);
}

void ZeroEd::GUIFillServerList()
{
	ZGuiMenu* pkMenu = ((ZGuiMenu*)GetWnd("MainMenu"));

	if(!pkMenu)
		return;

	char szMenuText[100];

	ClearListbox("ManageServerList");

	// Remove menuitems
	vector<string> kChilds;
	pkMenu->GetItems("Menu_File_Connect", kChilds);
	for(int i=0; i<kChilds.size(); i++)
		if(kChilds[i] != "Menu_File_Connect_Manage")
			pkMenu->RemoveItem( kChilds[i].c_str() );

	for(int i=0; i<m_vkServerList.size(); i++)
	{
		sprintf(szMenuText, "To %s as %s", m_vkServerList[i].strServerName.c_str(),
			m_vkServerList[i].strUserName.c_str());

		AddListItem("ManageServerList", (char*)m_vkServerList[i].FullName().c_str());

		pkMenu->AddItem(szMenuText, (char*)m_vkServerList[i].FullName().c_str(), 
			"Menu_File_Connect", false);	
		pkMenu->SetCheckMarkGroup((char*)m_vkServerList[i].FullName().c_str(), 191919);
		pkMenu->UseCheckMark((char*)m_vkServerList[i].FullName().c_str(), true);
	}

	((ZGuiMenu*)GetWnd("MainMenu"))->ResizeMenu();
}

void ZeroEd::OnGUIAddServer(bool bAdd)
{
	ZGuiMenu* pkMenu = ((ZGuiMenu*)GetWnd("MainMenu"));
	ZGuiListbox* pkList = (ZGuiListbox*) GetWnd("ManageServerList");
	char szMenuText[100];

	if(bAdd)
	{
		GUIServerInfo info(
			GetText("ServerNameTextbox"), GetText("ServerIPTextbox"),
			GetText("UserNameTextbox"), GetText("PasswordTextbox"));

		if(info.strServerName.size() < 1 ||
			info.strServerIP.size() < 7 || (info.strServerIP.find(".") == string::npos) ||
			info.strUserName.size() < 1 || info.strPassword.size() < 1)
		{
			printf("Bad names\n");
			return;
		}
		
		bool bExist = false;
		vector<GUIServerInfo>::iterator it = m_vkServerList.begin();
		for( ; it != m_vkServerList.end(); it++) 
		{
			if( (*it) == info ) 
			{
				int iID;
				if((iID=pkList->Find((char*)(*it).FullName().c_str())) != -1)
				{
					pkList->RemoveItem(pkList->GetItem(iID), false);
					pkMenu->RemoveItem((char*)(*it).FullName().c_str());
				}

				m_vkServerList.erase(it);
				bExist = true;
				break;
			}
		}

		sprintf(szMenuText, "To %s as %s", GetText("ServerNameTextbox"), 
			GetText("UserNameTextbox"));
		AddListItem("ManageServerList", (char*)info.FullName().c_str());
		pkMenu->AddItem(szMenuText, (char*)info.FullName().c_str(), 
			"Menu_File_Connect", false);
		pkMenu->SetCheckMarkGroup((char*)info.FullName().c_str(), 191919);
		pkMenu->UseCheckMark((char*)info.FullName().c_str(), true);
		m_vkServerList.push_back(info);

		if(!bExist)
		{
			SetText("ServerNameTextbox", "");
			SetText("ServerIPTextbox", "");
			SetText("UserNameTextbox", "");
			SetText("PasswordTextbox", "");
		}

	}
	else
	{
		if(pkList->GetSelItem())
		{
			char szText[100];
			sprintf(szText, "%s", pkList->GetSelItem()->GetText());

			int iID;
			if((iID=pkList->Find(szText)) != -1)
			{
				pkList->RemoveItem(pkList->GetItem(iID), false);
				((ZGuiMenu*)GetWnd("MainMenu"))->RemoveItem(szText);
				SetText("ServerNameTextbox", "");
				SetText("ServerIPTextbox", "");
				SetText("UserNameTextbox", "");
				SetText("PasswordTextbox", "");

				int counter = 0;
				vector<GUIServerInfo>::iterator it = m_vkServerList.begin();
				for( ; it != m_vkServerList.end(); it++) 
				{
					if(counter++ == iID)
					{
						m_vkServerList.erase(it);							
						break;
					}
				}
			}
		}
	}
}