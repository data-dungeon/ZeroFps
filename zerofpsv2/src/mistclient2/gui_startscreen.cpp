#include "mistclient.h"
#include "gui_optionsdlg.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "gui_inventory.h"
#include "gui_equipwnd.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../mcommon/p_enviroment.h"

extern MistClient g_kMistClient;

void GuiMsgStartScreen( string strMainWnd, string strController,
								unsigned int msg, int numparms, void *params )
{
	static bool s_bChangeServerName = false;

	////////////////////////////////////////////////////////////////////////////
	// ZGM_COMMAND (hantera meddelanden för Klicka på knappar och annat)
	////////////////////////////////////////////////////////////////////////////
	if(msg == ZGM_COMMAND)
	{
		if(strMainWnd == "MLStartWnd")
		{
			if(strController == "StarNewGameBn")
			{
				g_kMistClient.ShowWnd("ConnectWnd",	true,	true,	true);
				g_kMistClient.UpdateServerListbox();
			}
			else
			if(strController == "QuitBn")
			{		
				g_kMistClient.m_pkZeroFps->QuitEngine();
			}
			else
			if(strController == "OptionsBn")
			{
				if(g_kMistClient.LoadGuiFromScript(g_kMistClient.m_kGuiScrips[GSF_OPTION].c_str()))
				{
					g_kMistClient.ShowWnd("OptionsWnd",	true,	true,	true);
					g_kMistClient.m_pkOptionsDlg->Open();
				}
				else
				{
					printf("Failed to load GUI script!\n");
					return;
				}
			}
			else
			if(strController == "ContinueGameBn")
			{	
				g_kMistClient.LoadInGameGui(); 
			}
		}
		else
		if(strMainWnd == "ConnectWnd")
		{
			if(strController == "CloseServerWndBn")
				g_kMistClient.ShowWnd("ConnectWnd",	false);
			else
			if(strController == "AddServerBn")
			{
				s_bChangeServerName = false;
				g_kMistClient.ShowWnd("AddNewServerWnd", true, true, true);
				g_kMistClient.SetText("NewServerNameEB", "");
				g_kMistClient.SetText("NewServerIPName", "");
			}
			else
			if(strController == "EditServerBn")
			{
				string strName, strIP;
				if(g_kMistClient.NameIPFromServerList(strName, strIP))
				{
					s_bChangeServerName = true;
					g_kMistClient.ShowWnd("AddNewServerWnd", true, true, true);

					g_kMistClient.SetText("NewServerNameEB", (char*)strName.c_str());
					g_kMistClient.SetText("NewServerIPName", (char*)strIP.c_str());
				}
			}
			else
			if(strController == "RemoveServerBn")
			{
				string strName, strIP;
				if(g_kMistClient.NameIPFromServerList(strName, strIP))
				{
					g_kMistClient.AddRemoveServer(strName.c_str(), strIP.c_str(), false);
					g_kMistClient.UpdateServerListbox();
				}
			}
			else
			if(strController == "ConnectBn")
			{
				string strName, strIP;
				if(g_kMistClient.NameIPFromServerList(strName, strIP))
				{
					string strTitle =	string("Connect to ") +	strName;
					g_kMistClient.SetText("LoginWndLabel",	(char*)strTitle.c_str());

					g_kMistClient.SetText("LoginNameEB", (char*)	g_kMistClient.m_strLoginName.c_str());
					g_kMistClient.SetText("LoginPWEb", (char*) g_kMistClient.m_strLoginPW.c_str());
					g_kMistClient.ShowWnd("LoginWnd", true, true, true);
				}
			}
		}
		else
		if(strMainWnd == "AddNewServerWnd")
		{
			if(strController == "AddNewServerCancelBn")
			{
				g_kMistClient.ShowWnd("AddNewServerWnd", false);		
				g_kMistClient.ShowWnd("ConnectWnd",	true,	true,	true);
			}
			else
			if(strController == "AddNewServerOK")
			{
				g_kMistClient.ShowWnd("AddNewServerWnd", false);
				g_kMistClient.ShowWnd("ConnectWnd",	true,	true,	true);

				char*	szName =	g_kMistClient.GetText("NewServerNameEB");
				char*	szIP = g_kMistClient.GetText("NewServerIPName");

				if( (szName	!=	NULL && szIP != NULL) )
				{
					if( strlen(szName) >	0 && strlen(szIP)	> 0)
					{
						if(s_bChangeServerName)
						{
							string szOLDName,	szOLDIP;
							if(g_kMistClient.NameIPFromServerList(szOLDName, szOLDIP))
								g_kMistClient.AddRemoveServer(szOLDName.c_str(), szOLDIP.c_str(),	false);
						}

						g_kMistClient.AddRemoveServer(szName, szIP);
						g_kMistClient.UpdateServerListbox();
					}
				}
			}
		}
		else
		if(strMainWnd == "LoginWnd")
		{
			if(strController == "LoginOK")
			{
				if(!g_kMistClient.LoadGuiFromScript(g_kMistClient.m_kGuiScrips[GSF_CHARGEN].c_str()))
				{
					printf("failed to load character generation screen\n");
				}

				g_kMistClient.m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("CharGen_SelectCharWnd"), true);
				g_kMistClient.m_pkGui->SetCaptureToWnd(g_kMistClient.GetWnd("CharGen_SelectCharWnd"));
				
			}
			else
			if(strController == "LoginCancel")
			{
				 g_kMistClient.ShowWnd("LoginWnd", false);
			}
		}
		else
		if(strMainWnd == "CharGen_SelectCharWnd")
		{
			if(strController == "CharGen_PlayBn")
			{
				string strLogin, strPassword,	strServerName,	strServerIP;

				char*	text;

				if(g_kMistClient.NameIPFromServerList(strServerName, strServerIP))
				{
					if((text	= g_kMistClient.GetText("LoginNameEB")))
						strLogin	= text;

					if((text	= g_kMistClient.GetText("LoginPWEb")))
						strPassword	= text;

					if(!strLogin.empty()	&&	!strPassword.empty()	&&	!strServerIP.empty())
					{
						g_kMistClient.m_pkZeroFps->StartClient(strLogin, strPassword, strServerIP);
						g_kMistClient.m_strLoginName = strLogin;
						g_kMistClient.m_strLoginPW	= strPassword;
						g_kMistClient.m_strQuickStartAddress = strServerIP;
					}
					else
						printf("Input error,	failed to connect.");
				}

			}
			else
			if(strController == "CharGen_PickCharCacel")
			{
				g_kMistClient.ShowWnd("CharGen_SelectCharWnd", false);
				g_kMistClient.m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("LoginWnd"), true);
				g_kMistClient.m_pkGui->SetCaptureToWnd(g_kMistClient.GetWnd("LoginWnd"));
			}
			else
			if(strController == "CharGen_CreateNewCharBn")
			{
				g_kMistClient.ShowWnd("CharGen_CreateCharWnd", true, true, true);

				ZGuiWnd* pkModelWnd = g_kMistClient.GetWnd("CharGen_ModelPreviewLabel");

				Camera* pkCam = dynamic_cast<Camera*>(pkModelWnd->GetRenderTarget());

				if(pkCam == NULL)
				{
					g_kMistClient.AddListItem("CharGen_ModellList", "Good guy");
					g_kMistClient.AddListItem("CharGen_ModellList", "Bad guy");
					((ZGuiCombobox*)g_kMistClient.GetWnd("CharGen_ModellList"))->SetNumVisibleRows(2);

					g_kMistClient.SelListItemByIndex("CharGen_ModellList", 1);

					Entity* pkModellEnt = g_kMistClient.m_pkEntityManager->CreateEntity();
					pkModellEnt->SetSave(false);
					pkModellEnt->SetWorldPosV(Vector3(0,0,0));
					pkModellEnt->AddProperty("P_LightUpdate");
					pkModellEnt->AddProperty("P_Mad");

					Vector4 kCurrentDiffuse(1,1,1,1);
					Vector4 kCurrentAmbient(1,1,1,1);
					Vector3 kSunPos(0,0,0);

					P_Light* pkLight = (P_Light*)pkModellEnt->AddProperty("P_Light");

					pkLight->SetType(DIRECTIONAL_LIGHT);
					pkLight->SetDiffuse(kCurrentDiffuse);
					pkLight->SetAmbient(kCurrentAmbient);		
					pkLight->SetRot(kSunPos);	

					string szMadFile = string("data/mad/") + string("player2.mad");
					((P_Mad*)pkModellEnt->GetProperty("P_Mad"))->SetBase(szMadFile.c_str());	

					float aspect = (float) pkModelWnd->GetScreenRect().Width() /
						(float) pkModelWnd->GetScreenRect().Height();

					pkCam = new Camera(Vector3(0,0,0),Vector3(0,0,0),70,aspect,0.01,200);	
					pkCam->SetDebugGraphs(false);
					pkCam->SetClearViewPort(false);  
					pkCam->SetRootEntityID(pkModellEnt->GetEntityID());

					pkModelWnd->SetRenderTarget(pkCam);

					P_Mad* pkMad = ((P_Mad*)pkModellEnt->GetProperty("P_Mad"));
					pkModellEnt->SetWorldPosV(Vector3(0,-1.0f,-pkMad->GetRadius()));
				}
			}
		}
		else
		if(strMainWnd == "CharGen_CreateCharWnd")
		{
			if(strController == "CharGen_NewCharOK")
			{
				g_kMistClient.ShowWnd("CharGen_CreateCharWnd", false);
				g_kMistClient.m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("CharGen_SelectCharWnd"), true);
				g_kMistClient.m_pkGui->SetCaptureToWnd(g_kMistClient.GetWnd("CharGen_SelectCharWnd"));
				g_kMistClient.AddListItem("CharGen_CharList", g_kMistClient.GetText("CharGen_CharNameEb"), true);				
			}
			else
			if(strController == "CharGen_NewCharCancel")
			{
				g_kMistClient.ShowWnd("CharGen_CreateCharWnd", false);
				g_kMistClient.m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("CharGen_SelectCharWnd"), true);
				g_kMistClient.m_pkGui->SetCaptureToWnd(g_kMistClient.GetWnd("CharGen_SelectCharWnd"));
			}
		}		
	}

	////////////////////////////////////////////////////////////////////////////
	//	ZGM_KEYPRESS (hantera meddelanden för tagentbords)
	////////////////////////////////////////////////////////////////////////////
	if(msg == ZGM_KEYPRESS)
	{
		if( ((int*)params)[0] == KEY_TAB)
		{	
			if( strController	==	"NewServerNameEB"	)
				g_kMistClient.m_pkGui->SetFocus(	g_kMistClient.GetWnd("NewServerIPName") ); 
			if( strController	==	"NewServerIPName"	)
				g_kMistClient.m_pkGui->SetFocus(	g_kMistClient.GetWnd("NewServerNameEB") ); 

			if( strController	==	"LoginPWEb"	)
				g_kMistClient.m_pkGui->SetFocus(	g_kMistClient.GetWnd("LoginNameEB")	);	
			if( strController	==	"LoginNameEB" )
				g_kMistClient.m_pkGui->SetFocus(	g_kMistClient.GetWnd("LoginPWEb") ); 
		}
	}	

	if(msg == ZGM_CBN_SELENDOK)
	{
		if(strMainWnd == "CharGen_CreateCharWnd")
		{
			if(strController == "CharGen_ModellList")
			{
				ZGuiWnd* pkModelWnd = g_kMistClient.GetWnd("CharGen_ModelPreviewLabel");
			
				if(Camera* pkCam = dynamic_cast<Camera*>(pkModelWnd->GetRenderTarget()))
				{
					Entity* pkEnt = g_kMistClient.m_pkEntityManager->GetEntityByID(pkCam->GetRootEntityID());

					string szMadFile = string("data/mad/");
					
					if(string(g_kMistClient.GetSelItem((char*)strController.c_str())) == "Good guy")
						szMadFile += string("player.mad");
					else
					if(string(g_kMistClient.GetSelItem((char*)strController.c_str())) == "Bad guy")
						szMadFile += string("player2.mad");

					P_Mad* pkMad = ((P_Mad*)pkEnt->GetProperty("P_Mad"));

					pkMad->SetBase(szMadFile.c_str());
					pkEnt->SetWorldPosV(Vector3(0,-1.0f,-pkMad->GetRadius()));
				}
			}
		}
	}
}

bool MistClient::NameIPFromServerList(string& strName, string& strIP)
{
	char*	szSelItem =	GetSelItem("ServerList");
	if( szSelItem )
	{
		string strText	= string(szSelItem);
		int pos = (int) strText.find("-");
		strName = strText.substr(0, pos-1);
		strIP	= strText.substr(pos+2,	strText.length()-pos);
		return true;
	}

	return false;
}

void MistClient::LoadStartScreenGui(bool bShowSplashImage)
{
	SetGuiCapture(true);

	ShowWnd("ActionBar",	false);
	ShowWnd("ChatDlgMainWnd",	false);

	ShowWnd("ConnectWnd", false);
	ShowWnd("AddNewServerWnd", false);
	ShowWnd("LoginWnd", false);
	ShowWnd("OptionsWnd", false);

	ShowWnd("MLStartWnd", true);
	
	GetWnd("ContinueGameBn")->Show();

	if(bShowSplashImage == false)
	{
		GetWnd("MLStartWnd")->m_bUseAlhpaTest = false;
		GetWnd("MLStartWnd")->GetSkin()->m_bTransparent = true;
	}
	else
	{
		GetWnd("MLStartWnd")->GetSkin()->m_bTransparent = false;
		m_pkInventoryDlg->Close();
	}
}
