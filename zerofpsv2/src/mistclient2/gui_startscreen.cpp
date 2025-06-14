#include "mistclient.h"
#include "gui_optionsdlg.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "gui_inventory.h"
#include "gui_equipwnd.h"
#include "../zerofpsv2/engine_systems/propertys/p_mad.h"
#include "../mcommon/p_enviroment.h"
#include "../zerofpsv2/engine/inputhandle.h"
#include "../mcommon/ml_netmessages.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"


extern MistClient g_kMistClient;

void GuiMsgStartScreen( string strMainWnd, string strController,
								unsigned int msg, int numparms, void *params )
{
	static bool s_bChangeServerName = false;

	////////////////////////////////////////////////////////////////////////////
	// ZGM_COMMAND (hantera meddelanden f�r Klicka p� knappar och annat)
	////////////////////////////////////////////////////////////////////////////
	if(msg == ZGM_COMMAND)
	{
		cout << "strController: " << strController << "\n";
 
		if(strMainWnd == "MLStartWnd")
		{
			if(strController == "StarNewGameBn")
			{
				g_kMistClient.ShowWnd("ConnectWnd",	true,	true,	true);
				g_kMistClient.UpdateServerListbox();
				
				//ugly list server hack
			 	g_ZFObjSys.RunCommand("msref",CSYS_SRC_SUBSYS);	
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
			if(strController == "UpdateServerListBn")
			{
				g_kMistClient.m_pkNetwork->MS_RequestServers();			
			}			
			else
			if(strController == "ConnectBn")
			{
				string strName, strIP;
				if(g_kMistClient.NameIPFromServerList(strName, strIP))
				{
					string strTitle =	string("Connect to ") +	strName;
					g_kMistClient.SetText("LoginWndLabel",	(char*)strTitle.c_str());

					g_kMistClient.SetText("LoginNameEB", (char*)	g_kMistClient.m_kstrLoginName.GetString().c_str());
					g_kMistClient.SetText("LoginPWEb", (char*) g_kMistClient.m_kstrLoginPW.GetString().c_str());
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
							g_kMistClient.m_kstrLoginName.SetString(strLogin);
							g_kMistClient.m_kstrLoginPW.SetString(strPassword);
							g_kMistClient.m_kstrQuickStartAddress.SetString(strServerIP);
						}
						else
							printf("Input error,	failed to connect.");
					}

				//if(!g_kMistClient.LoadGuiFromScript(g_kMistClient.m_kGuiScrips[GSF_CHARGEN].c_str()))
				//{
				//	printf("failed to load character generation screen\n");

				//	ZGuiWnd* pkModelWnd = g_kMistClient.GetWnd("CharGen_ModelPreviewLabel");

				//	Camera* pkCam = dynamic_cast<Camera*>(pkModelWnd->GetRenderTarget());

				//	if(pkCam == NULL)
				//	{
				//		g_kMistClient.AddListItem("CharGen_ModellList", "Good guy");
				//		g_kMistClient.AddListItem("CharGen_ModellList", "Bad guy");
				//		((ZGuiCombobox*)g_kMistClient.GetWnd("CharGen_ModellList"))->SetNumVisibleRows(2);

				//		g_kMistClient.SelListItemByIndex("CharGen_ModellList", 1);

				//		Entity* pkModellEnt = g_kMistClient.m_pkEntityManager->CreateEntity();
				//		pkModellEnt->SetSave(false);
				//		pkModellEnt->SetWorldPosV(Vector3(0,0,0));
				//		pkModellEnt->AddProperty("P_Mad");

				//		Vector4 kCurrentDiffuse(1,1,1,1);
				//		Vector4 kCurrentAmbient(1,1,1,1);
				//		Vector3 kSunPos(0,0,0);

				//		P_Light* pkLight = (P_Light*)pkModellEnt->AddProperty("P_Light");

				//		pkLight->SetType(DIRECTIONAL_LIGHT);
				//		pkLight->SetDiffuse(kCurrentDiffuse);
				//		pkLight->SetAmbient(kCurrentAmbient);		
				//		pkLight->SetRot(kSunPos);	

				//		string szMadFile = string("data/mad/") + string("player2.mad");
				//		((P_Mad*)pkModellEnt->GetProperty("P_Mad"))->SetBase(szMadFile.c_str());	

				//		float aspect = (float) pkModelWnd->GetScreenRect().Width() /
				//			(float) pkModelWnd->GetScreenRect().Height();

				//		pkCam = new Camera(Vector3(0,0,0),Vector3(0,0,0),70,aspect,0.01,200);	
				//		pkCam->SetDebugGraphs(false);
				//		pkCam->SetClearViewPort(false);  
				//		pkCam->SetRootEntityID(pkModellEnt->GetEntityID());

				//		pkModelWnd->SetRenderTarget(pkCam);

				//		P_Mad* pkMad = ((P_Mad*)pkModellEnt->GetProperty("P_Mad"));					
				//		pkModellEnt->SetWorldPosV(Vector3(0,-1.0f,-pkMad->GetRadius()));

				//		//Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 					
				//		//pkMad->SetAnimation(pkCore->GetAnimationName(1).c_str(), 0);				
				//	}
				//}

				/*
				g_kMistClient.ShowWnd("CharGen_SelectCharWnd", true, true, true);

				g_kMistClient.m_pkGui->PlaceWndFrontBack(g_kMistClient.GetWnd("CharGen_SelectCharWnd"), true);
				g_kMistClient.m_pkGui->SetCaptureToWnd(g_kMistClient.GetWnd("CharGen_SelectCharWnd"));
				*/
				
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
				char*	szSelItem =	g_kMistClient.GetSelItem("CharGen_CharList");
				if(szSelItem)
				{
					g_kMistClient.RegBeginPlay(string(szSelItem));
					g_kMistClient.LoadInGameGui();
				
					//request playerlist on join
					g_kMistClient.SendRequestPlayerList();				
				}

			/*	string strLogin, strPassword,	strServerName,	strServerIP;

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
				}*/

			}
			else
			if(strController == "CharGen_DeleteSelChar")
			{
				char*	szSelItem =	g_kMistClient.GetSelItem("CharGen_CharList");
				if(szSelItem)
				{
					g_kMistClient.DeleteChar(string(szSelItem));
					cout << "Should delete sel char" << szSelItem << "\n";
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

				Camera* pkCam = static_cast<Camera*>(pkModelWnd->GetRenderTarget());

				if(pkCam == NULL)
				{
					g_kMistClient.AddListItem("CharGen_ModellList", "human_male");
					//g_kMistClient.AddListItem("CharGen_ModellList", "human_female");
					//g_kMistClient.AddListItem("CharGen_ModellList", "goblin_male");
					((ZGuiCombobox*)g_kMistClient.GetWnd("CharGen_ModellList"))->SetNumVisibleRows(3);

					g_kMistClient.SelListItemByIndex("CharGen_ModellList",0);

					Entity* pkModellEnt = g_kMistClient.m_pkEntityManager->CreateEntity();
					pkModellEnt->SetSave(false);
					pkModellEnt->SetWorldPosV(Vector3(0,0,0));
					pkModellEnt->AddProperty("P_Mad");

// 					Vector4 kCurrentDiffuse(1,1,1,1);
// 					Vector4 kCurrentAmbient(1,1,1,1);
// 					Vector3 kSunPos(0,0,0);
// 
// 					P_Light* pkLight = (P_Light*)pkModellEnt->AddProperty("P_Light");
// 
// 					pkLight->SetType(DIRECTIONAL_LIGHT);
// 					pkLight->SetDiffuse(kCurrentDiffuse);
// 					pkLight->SetAmbient(kCurrentAmbient);		
// 					pkLight->SetRot(kSunPos);	

					string szMadFile = string("player2.mad");
					((P_Mad*)pkModellEnt->GetProperty("P_Mad"))->SetBase(szMadFile.c_str());	
					((P_Mad*)pkModellEnt->GetProperty("P_Mad"))->SetAnimation("idle",0);

					float aspect = (float) pkModelWnd->GetScreenRect().Width() /
						(float) pkModelWnd->GetScreenRect().Height();

					pkCam = new Camera(Vector3(0,0,0),Vector3(0,0,0),70,aspect,0.01,200);	
					pkCam->SetDrawInterface(false);
					pkCam->SetClearViewPort(false);  
					pkCam->SetRootEntityID(pkModellEnt->GetEntityID());
					pkCam->SetForceLighting(LIGHT_ALWAYS_OFF);
					
					cout<<"blubba mera"<<endl;
					
					pkModelWnd->SetRenderTarget(pkCam);

					P_Mad* pkMad = ((P_Mad*)pkModellEnt->GetProperty("P_Mad"));					
					pkModellEnt->SetWorldPosV(Vector3(0,-1.0f,-pkMad->GetRadius()));

					//Mad_Core* pkCore = dynamic_cast<Mad_Core*>(pkMad->kMadHandle.GetResourcePtr()); 					
					//pkMad->SetAnimation(pkCore->GetAnimationName(1).c_str(), 0);				
				}

				// Set keyboard focus to name textbox.
				g_kMistClient.m_pkGui->SetFocus(g_kMistClient.GetWnd("CharGen_CharNameEb"), true);
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
		
				char*	szSelItem =	g_kMistClient.GetText("CharGen_CharNameEb");
				char*	szSelMod =	g_kMistClient.GetSelItem("CharGen_ModellList");
				if(szSelItem && szSelMod)
				{
					g_kMistClient.AddChar(string(szSelItem),string(szSelMod));
				}

				/*
				g_kMistClient.AddListItem("CharGen_CharList", g_kMistClient.GetText("CharGen_CharNameEb"), true);	
				int iItem = ((ZGuiCombobox*)g_kMistClient.GetWnd("CharGen_CharList"))->GetListbox()->GetItemCount();
				if(iItem > 20) iItem = 20;
				((ZGuiCombobox*)g_kMistClient.GetWnd("CharGen_CharList"))->SetNumVisibleRows(iItem);*/
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
	//	ZGM_KEYPRESS (hantera meddelanden f�r tagentbords)
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
			
				if(Camera* pkCam = static_cast<Camera*>(pkModelWnd->GetRenderTarget()))
				{
					Entity* pkEnt = g_kMistClient.m_pkEntityManager->GetEntityByID(pkCam->GetRootEntityID());

					string szMadFile;
					
					if(string(g_kMistClient.GetSelItem((char*)strController.c_str())) == "human_female")
						szMadFile = string("player.mad");
					else if(string(g_kMistClient.GetSelItem((char*)strController.c_str())) == "human_male")
						szMadFile = string("player2.mad");
					else if(string(g_kMistClient.GetSelItem((char*)strController.c_str())) == "goblin_male")
						szMadFile = string("goblin.mad");

					P_Mad* pkMad = ((P_Mad*)pkEnt->GetProperty("P_Mad"));

					pkMad->SetBase(szMadFile.c_str());
					pkEnt->SetWorldPosV(Vector3(0,-1.0f,-pkMad->GetRadius()));
					pkEnt->SetWorldRotV(Vector3(0,0,0));
				}
			}
		}
	}

	if(msg == ZGM_MOUSEMOVE)
	{
		if(strMainWnd == "CharGen_CreateCharWnd")
		{
			ZGuiWnd* pkModelWnd = g_kMistClient.GetWnd("CharGen_ModelPreviewLabel");			

			static Point press_pos;
			static bool pressed = false;
			static float prev_x = 0, prev_y = 0, prev_z = 0;
			static float fObjRotX=0, fObjRotY=0, fObjRotZ=0;
			static float m_fObjRotX=0, m_fObjRotY=0, m_fObjRotZ=0;

			if(g_kMistClient.m_pkInputHandle->Pressed(MOUSELEFT))
			{
				Point curr_pos(g_kMistClient.m_pkGui->m_iMouseX, g_kMistClient.m_pkGui->m_iMouseY);

				if(pressed == false)
				{
					press_pos = curr_pos;
					pressed = true;
				}

				m_fObjRotX = curr_pos.y - press_pos.y + prev_x;
				m_fObjRotY = press_pos.x - curr_pos.x + prev_y;

				if(pkModelWnd->GetScreenRect().Inside(curr_pos.x, curr_pos.y	))
				{
					m_fObjRotX = press_pos.x - curr_pos.x  + prev_x;

					if(Camera* pkCam = static_cast<Camera*>(pkModelWnd->GetRenderTarget()))
					{
						Entity* pkEnt = g_kMistClient.m_pkEntityManager->GetEntityByID(pkCam->GetRootEntityID());
						pkEnt->SetWorldRotV(Vector3(fObjRotX,m_fObjRotX,fObjRotZ));
					}
				}
			}
			else
			{
				pressed = false;
				prev_x = m_fObjRotX;
				prev_y = m_fObjRotY;
				prev_z = m_fObjRotZ;
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
		printf("address from gui = %s\n", strIP.c_str());
		return true;
	}

	return false;
}

void MistClient::LoadStartScreenGui(bool bShowSplashImage)
{
	SetGuiCapture(true);

	ShowWnd("SkillBar",	false);
	ShowWnd("ActionBar",	false);
	ShowWnd("ChatDlgMainWnd",	false);

	ShowWnd("ConnectWnd", false);
	ShowWnd("AddNewServerWnd", false);
	ShowWnd("LoginWnd", false);
	ShowWnd("OptionsWnd", false);
	ShowWnd("CharGen_CreateCharWnd", false);
	ShowWnd("CharGen_SelectCharWnd", false);

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
		
		//set menu music
		m_pkAudioSys->SetMusic(m_strMenuMusic);

	}
}
