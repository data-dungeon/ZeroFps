
#include "mistclient.h"
#include "gui_optionsdlg.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/engine_systems/common/zshadow.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"

extern MistClient	g_kMistClient;

OptionsDlg::OptionsDlg(MistClient* pkMistClient)
{ 
	m_pkMC = pkMistClient;

	m_pkZShaderSystem = m_pkMC->m_pkZShaderSystem;
	m_pkZShadow = m_pkMC->m_pkZShadow;
	m_pkAudioSys = m_pkMC->m_pkAudioSys;
	m_pkRender = m_pkMC->m_pkRender;
	m_pkInput = m_pkMC->m_pkInput;
	m_pkZeroFps = m_pkMC->m_pkZeroFps;

	m_kOptionsValues.m_bLockGammaColors = false;
	m_kOptionsValues.m_iCurrentShadowGroup = 1;
	m_kOptionsValues.m_iCurrentPage = 0;
}

OptionsDlg::~OptionsDlg()
{

}

void OptionsDlg::Open()
{
	ZGuiTabCtrl* pkTabCtrl = (ZGuiTabCtrl*)m_pkMC->GetWnd("OptionsTabCtlr");
	pkTabCtrl->SetFont(m_pkMC->m_pkGuiMan->Font("defguifont"));

	((ZGuiSlider*)m_pkMC->GetWnd("AudioVolumeSlider"))->AddBuddyWindow(m_pkMC->GetWnd("AudioVolume"));
	
	float unslColor[] = {0.7f,0.7f,0.7f};
	float selColor[]  = {1,1,1};
	
	pkTabCtrl->SetTabColor(unslColor,false);
	pkTabCtrl->SetTabColor(selColor,true);
	
	for(int i=0; i<pkTabCtrl->GetNumPages(); i++)
	{
		list<ZGuiWnd*> kChilds;
		pkTabCtrl->GetPage(i)->GetChildrens(kChilds);
		for(list<ZGuiWnd*>::iterator it = kChilds.begin(); it!=kChilds.end(); it++) 
		{
			(*it)->SetTextColor(255,255,255); 
		}
	}

	char szText[50];

	((ZGuiSlider*)m_pkMC->GetWnd("GammaSliderRed"))->SetRange(0,100);
	((ZGuiSlider*)m_pkMC->GetWnd("GammaSliderGreen"))->SetRange(0,100);
	((ZGuiSlider*)m_pkMC->GetWnd("GammaSliderBlue"))->SetRange(0,100);

	m_pkZShaderSystem->GetGamma(m_kOptionsValues.m_fGammaColors[0],
		m_kOptionsValues.m_fGammaColors[1],m_kOptionsValues.m_fGammaColors[2]);
	
	((ZGuiSlider*)m_pkMC->GetWnd("GammaSliderRed"))->SetPos((int)(
		(m_kOptionsValues.m_fGammaColors[0] / 5.0f) * 100.0f), false);
	((ZGuiSlider*)m_pkMC->GetWnd("GammaSliderGreen"))->SetPos((int)(
		(m_kOptionsValues.m_fGammaColors[1] / 5.0f) * 100.0f), false);
	((ZGuiSlider*)m_pkMC->GetWnd("GammaSliderBlue"))->SetPos((int)(
		(m_kOptionsValues.m_fGammaColors[2] / 5.0f) * 100.0f), false);

	sprintf(szText, "%.2f", m_kOptionsValues.m_fGammaColors[0]); 
	m_pkMC->SetText("GammaRedLabel", szText);
	sprintf(szText, "%.2f", m_kOptionsValues.m_fGammaColors[1]); 
	m_pkMC->SetText("GammaGreenLabel", szText);
	sprintf(szText, "%.2f", m_kOptionsValues.m_fGammaColors[2]); 
	m_pkMC->SetText("GammaBlueLabel", szText);

	m_pkMC->CheckButton("LockGammaColorsCheckbox", m_kOptionsValues.m_bLockGammaColors);

	m_pkMC->CheckButton("ScreenSize800x600Rb", 
		(m_pkMC->m_iWidth == 800 && m_pkMC->m_iHeight == 600) );
	m_pkMC->CheckButton("ScreenSize1024x768Rb", 
		(m_pkMC->m_iWidth == 1024 && m_pkMC->m_iHeight == 768) );
	m_pkMC->CheckButton("ScreenSize1280x1024Rb", 
		(m_pkMC->m_iWidth == 1280 && m_pkMC->m_iHeight == 1024) );
	m_pkMC->CheckButton("ScreenSize1600x1200Rb", 
		(m_pkMC->m_iWidth == 1600 && m_pkMC->m_iHeight == 1200) );

	m_pkMC->CheckButton("ColorDepth16Checkbox", (m_pkRender->GetDepth() == 16) );
	m_pkMC->CheckButton("ColorDepth32Checkbox", (m_pkRender->GetDepth() == 32) );

	m_pkMC->CheckButton("FullscreenCheckbox", (m_pkRender->GetFullscreen() == true) );
	m_pkMC->CheckButton("WindowCheckbox", (m_pkRender->GetFullscreen() == false) );

	m_pkMC->CheckButton("NumShadows0Rb", (m_pkZShadow->GetNumShadowsPerModel() == 0));	
	m_pkMC->CheckButton("NumShadows1Rb", (m_pkZShadow->GetNumShadowsPerModel() == 1));
	m_pkMC->CheckButton("NumShadows2Rb", (m_pkZShadow->GetNumShadowsPerModel() == 2));
	m_pkMC->CheckButton("NumShadows3Rb", (m_pkZShadow->GetNumShadowsPerModel() == 3));
	m_pkMC->CheckButton("NumShadows4Rb", (m_pkZShadow->GetNumShadowsPerModel() == 4));

	m_pkMC->ClearListbox("ShadowGroupsComboBox");
	((ZGuiCombobox*)m_pkMC->GetWnd("ShadowGroupsComboBox"))->SetNumVisibleRows(5); 
	m_pkMC->AddListItem("ShadowGroupsComboBox", "Big enviroment objects (zones ,trees)", 
		m_kOptionsValues.m_iCurrentShadowGroup == 1);
	m_pkMC->AddListItem("ShadowGroupsComboBox", "Small enviroment objects (rocks,tables)", 
		m_kOptionsValues.m_iCurrentShadowGroup == 2);
	m_pkMC->AddListItem("ShadowGroupsComboBox", "Players (player characters)", 
		m_kOptionsValues.m_iCurrentShadowGroup == 3);
	m_pkMC->AddListItem("ShadowGroupsComboBox", "Game objects (inventory objects)", 
		m_kOptionsValues.m_iCurrentShadowGroup == 4);
	m_pkMC->AddListItem("ShadowGroupsComboBox", "Creatures (non playable monsters)", 
		m_kOptionsValues.m_iCurrentShadowGroup == 5);
	
	m_kOptionsValues.m_abEnabledShadowGroups[1] = m_pkZShadow->IsShadowGroupEnabled(1);
	m_kOptionsValues.m_abEnabledShadowGroups[2] = m_pkZShadow->IsShadowGroupEnabled(2);
	m_kOptionsValues.m_abEnabledShadowGroups[3] = m_pkZShadow->IsShadowGroupEnabled(3);
	m_kOptionsValues.m_abEnabledShadowGroups[4] = m_pkZShadow->IsShadowGroupEnabled(4);
	m_kOptionsValues.m_abEnabledShadowGroups[5] = m_pkZShadow->IsShadowGroupEnabled(5);

	m_pkMC->CheckButton("ShadowmapCheckbox", m_pkMC->m_pkZeroFps->GetShadowMap() );
	m_kOptionsValues.m_bPrevShadowMapState = m_pkMC->m_pkZeroFps->GetShadowMap(); 

	m_pkMC->CheckButton("VegetationCheckbox", m_pkMC->m_pkZeroFps->GetVegetation() );
	m_kOptionsValues.m_bPrevVegetationState = m_pkMC->m_pkZeroFps->GetVegetation(); 	
	
	m_pkMC->CheckButton("EnableShadowGroup", m_kOptionsValues.m_abEnabledShadowGroups[
		m_kOptionsValues.m_iCurrentShadowGroup]);	

	m_kOptionsValues.m_iPrevNetSpeed = m_pkMC->m_pkZeroFps->GetConnectionSpeed(); 

	//m_pkMC->CheckButton("NetSpeed1Rb", (m_kOptionsValues.m_iPrevNetSpeed == 3300)  );
	//m_pkMC->CheckButton("NetSpeed2Rb", (m_kOptionsValues.m_iPrevNetSpeed == 10000)  );
	//m_pkMC->CheckButton("NetSpeed3Rb", (m_kOptionsValues.m_iPrevNetSpeed == 50000) );
	//
	for(int i=0; i<pkTabCtrl->GetNumPages(); i++)
	{
		if(i!=m_kOptionsValues.m_iCurrentPage)
			pkTabCtrl->GetPage(i)->Hide();
		else
			pkTabCtrl->GetPage(i)->Show();
	}

	float fPos;
	//fPos = m_pkAudioSys->GetMusicVolume() * 100.0f; 
	//((ZGuiSlider*)m_pkMC->GetWnd("MusicVolumeSlider"))->SetRange(0,100);
	//((ZGuiSlider*)m_pkMC->GetWnd("MusicVolumeSlider"))->SetPos(fPos, true); 

	fPos = m_pkAudioSys->GetMainVolume() * 100.0f;
	((ZGuiSlider*)m_pkMC->GetWnd("AudioVolumeSlider"))->SetRange(0,100);
	((ZGuiSlider*)m_pkMC->GetWnd("AudioVolumeSlider"))->SetPos((int)fPos, true);

	fPos = (m_pkInput->GetMouseSens() / 5.0f) * 100.0f ;
	((ZGuiSlider*)m_pkMC->GetWnd("MouseSensSlider"))->SetRange(0,100);
	((ZGuiSlider*)m_pkMC->GetWnd("MouseSensSlider"))->SetPos((int)fPos, true);

	fPos = m_pkZeroFps->GetConnectionSpeed();
	((ZGuiSlider*)m_pkMC->GetWnd("NetSpeedSlider"))->SetRange(3000,50000);
	((ZGuiSlider*)m_pkMC->GetWnd("NetSpeedSlider"))->SetPos((int)fPos, true);




	m_kOptionsValues.m_fPrevVolume = m_pkAudioSys->GetMainVolume();
	//m_kOptionsValues.m_fPrevMusicVolume = m_pkAudioSys->GetMusicVolume();
	m_kOptionsValues.m_fPrevMouseSens = m_pkInput->GetMouseSens();

	char* szWnds[] =
	{
		"ScreenSize1024x768Rb", "ScreenSize1280x1024Rb",
		"ScreenSize1600x1200Rb", "ScreenSize800x600Rb",
	};

	for(int i=0; i<4; i++)
	{
		m_pkMC->GetWnd(szWnds[i])->SetTextColor(255,255,255); 
	}

	if(g_kMistClient.m_pkGui->m_iScaleMode == AUTO_SCALE)
		m_pkMC->CheckButton("ScaleGUICheckbox", true);

	if(g_kMistClient.m_pkGui->m_iScaleMode == MANUALLY_SCALE)
		m_pkMC->CheckButton("ScaleGUICheckbox", false);
	
}

void OptionsDlg::Close(bool bSave)
{
	m_pkMC->ShowWnd("OptionsWnd",	false);
	m_kOptionsValues.m_iCurrentPage = ((ZGuiTabCtrl*)m_pkMC->GetWnd("OptionsTabCtlr"))->GetCurrentPage(); 

	if(bSave == false) // restore old values
	{
		m_pkZShaderSystem->SetGamma(m_kOptionsValues.m_fGammaColors[0],
			m_kOptionsValues.m_fGammaColors[1], m_kOptionsValues.m_fGammaColors[2]);

		m_pkAudioSys->SetMainVolume(m_kOptionsValues.m_fPrevVolume);
//		m_pkAudioSys->SetMusicVolume(m_kOptionsValues.m_fPrevMusicVolume);
		
		//char cmd[25];
		//sprintf(cmd, "i_mousesens %.3f", m_kOptionsValues.m_fPrevMouseSens);
		//m_pkZeroFps->m_pkConsole->Execute(cmd);

		//sprintf(cmd, "r_shadowmap %i", m_kOptionsValues.m_bPrevShadowMapState);
		//g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(cmd);		

		//sprintf(cmd, "r_vegetation %i", m_kOptionsValues.m_bPrevVegetationState);
		//g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(cmd);				
		//
		//sprintf(cmd, "n_netspeed %i", m_kOptionsValues.m_iPrevNetSpeed);
		//g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(cmd);		

		m_kOptionsValues.m_iNewNetSpeed = 
		m_kOptionsValues.m_iPrevNetSpeed = m_pkMC->m_pkZeroFps->GetConnectionSpeed();

		
	}
	else // change options
	{		

		char cmd[25];
		sprintf(cmd, "n_netspeed %i", m_kOptionsValues.m_iNewNetSpeed);
		g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(cmd);	

		m_pkZShaderSystem->SetGamma((float)atof(m_pkMC->GetText("GammaRedLabel")),
			(float)atof(m_pkMC->GetText("GammaGreenLabel")), (float)atof(m_pkMC->GetText("GammaBlueLabel")));

		m_kOptionsValues.m_bLockGammaColors = m_pkMC->IsButtonChecked("LockGammaColorsCheckbox");

		if( (m_pkMC->IsButtonChecked("ScreenSize800x600Rb") &&
			!(m_pkMC->m_iWidth == 800 && m_pkMC->m_iHeight == 600)) || 
			(m_pkMC->IsButtonChecked("ScreenSize1024x768Rb") &&
			!(m_pkMC->m_iWidth == 1024 && m_pkMC->m_iHeight == 768)) ||
			(m_pkMC->IsButtonChecked("ScreenSize1280x1024Rb") &&
			!(m_pkMC->m_iWidth == 1280 && m_pkMC->m_iHeight == 1024)) ||
			(m_pkMC->IsButtonChecked("ScreenSize1600x1200Rb") &&
			!(m_pkMC->m_iWidth == 1600 && m_pkMC->m_iHeight == 1200)) ||
			(m_pkMC->IsButtonChecked("ColorDepth16Checkbox") && m_pkRender->GetDepth() != 16) ||
			(m_pkMC->IsButtonChecked("ColorDepth32Checkbox") && m_pkRender->GetDepth() != 32) ||
			(m_pkMC->IsButtonChecked("FullscreenCheckbox") && !m_pkRender->GetFullscreen()) ||
			(m_pkMC->IsButtonChecked("WindowCheckbox") && m_pkRender->GetFullscreen()))
		{
			m_pkMC->SetText("RestartMsgLabel", "You must restart before the changes take effect.");
			m_pkMC->ShowWnd("RestartMsgBox", true, true, true);
		}

		if(m_pkMC->m_iScaleMode == AUTO_SCALE && !m_pkMC->IsButtonChecked("ScaleGUICheckbox"))
		{
			m_pkMC->SetText("RestartMsgLabel", "You must restart before the changes take effect.");
			m_pkMC->ShowWnd("RestartMsgBox", true, true, true);
		}

		if(m_pkMC->m_iScaleMode == MANUALLY_SCALE && m_pkMC->IsButtonChecked("ScaleGUICheckbox"))
		{
			m_pkMC->SetText("RestartMsgLabel", "You must restart before the changes take effect.");
			m_pkMC->ShowWnd("RestartMsgBox", true, true, true);
		}

		if(m_pkMC->IsButtonChecked("NumShadows0Rb")) m_pkZeroFps->m_pkConsole->Execute("r_shadows 0");
		if(m_pkMC->IsButtonChecked("NumShadows1Rb")) m_pkZeroFps->m_pkConsole->Execute("r_shadows 1");
		if(m_pkMC->IsButtonChecked("NumShadows2Rb")) m_pkZeroFps->m_pkConsole->Execute("r_shadows 2");
		if(m_pkMC->IsButtonChecked("NumShadows3Rb")) m_pkZeroFps->m_pkConsole->Execute("r_shadows 3");
		if(m_pkMC->IsButtonChecked("NumShadows4Rb")) m_pkZeroFps->m_pkConsole->Execute("r_shadows 4");

		for(int i=1; i<6; i++)
			if( m_kOptionsValues.m_abEnabledShadowGroups[i] ) 
				m_pkZShadow->EnableShadowGroup(i);
			else
				m_pkZShadow->DisableShadowGroup(i);
	}
}

void GuiMsgOptionsDlg( string strMainWnd, string strController,
								unsigned int msg, int numparms, void *params )
{
	static bool s_bChangeServerName = false;

	////////////////////////////////////////////////////////////////////////////
	// ZGM_COMMAND (hantera meddelanden för Klicka på knappar och annat)
	////////////////////////////////////////////////////////////////////////////
	if(msg == ZGM_COMMAND)
	{
		if(strMainWnd == "OptionsWnd")
		{
			if(strController == "OptionsWndOKBn")
			{
				g_kMistClient.m_pkOptionsDlg->Close(true);
			}
			else
			if(strController == "OptionsWndCancelBn")
			{
				g_kMistClient.m_pkOptionsDlg->Close(false);
			}
		}
		else
		if(strMainWnd == "OptionsPageVideo")
		{
			if(strController == "FullscreenCheckbox")
			{
				g_kMistClient.CheckButton("WindowCheckbox", 
					!g_kMistClient.IsButtonChecked("FullscreenCheckbox"));
			}
			else
			if(strController == "WindowCheckbox")
			{
				g_kMistClient.CheckButton("FullscreenCheckbox", 
					!g_kMistClient.IsButtonChecked("WindowCheckbox"));
			}
			if(strController == "ColorDepth16Checkbox")
			{
				g_kMistClient.CheckButton("ColorDepth32Checkbox", 
					!g_kMistClient.IsButtonChecked("ColorDepth16Checkbox"));
			}
			else
			if(strController == "ColorDepth32Checkbox")
			{
				g_kMistClient.CheckButton("ColorDepth16Checkbox", 
					!g_kMistClient.IsButtonChecked("ColorDepth32Checkbox"));
			}
		}
		else
		if(strMainWnd == "OptionsPageGraphic")
		{
			if(strController == "EnableShadowGroup")
			{
				g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_abEnabledShadowGroups[
					g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_iCurrentShadowGroup] =
					g_kMistClient.IsButtonChecked("EnableShadowGroup");
			}
			else
			if(strController == "ShadowmapCheckbox")
			{
				char cmd[50];
				sprintf(cmd, "r_shadowmap %i", (int) g_kMistClient.IsButtonChecked("ShadowmapCheckbox"));
				g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(cmd);								
			}
			else
			if(strController == "VegetationCheckbox")
			{
				char cmd[50];
				sprintf(cmd, "r_vegetation %i", (int) g_kMistClient.IsButtonChecked("VegetationCheckbox"));
				g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(cmd);								
			}
		}
		else
		if(strMainWnd == "OptionsPageAudio")
		{
			if(strController == "AudioOnOffCheckbox")
			{
				if(((ZGuiCheckbox*)g_kMistClient.GetWnd("AudioOnOffCheckbox"))->IsChecked())
					g_kMistClient.m_pkAudioSys->SetMainVolume(0.0f);
				else
					g_kMistClient.m_pkAudioSys->SetMainVolume(
						g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_fPrevVolume);

			}
		}
		else
		if(strMainWnd == "OptionsPageController")
		{
			if(strController == "MouseSensSetDefaultBn")
			{
				float fPos = (1.0f / 5.0f) * 100.0f ;
				((ZGuiSlider*)g_kMistClient.GetWnd("MouseSensSlider"))->SetPos((int)fPos, true);
			}
		}
		else 
		if(strMainWnd == "OptionsPageGame")
		{

			if(strController == "NetSpeedSetDefaultBn")
			{
				((ZGuiSlider*)g_kMistClient.GetWnd("NetSpeedSlider"))->SetPos(3000, true);
			}

			//char cmd[25];

			//if(strController == "NetSpeed1Rb")
			//{
			//	sprintf(cmd, "n_netspeed %i", 3000);
			//	g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(cmd);		
			//}
			//else
			//if(strController == "NetSpeed2Rb")
			//{
			//	sprintf(cmd, "n_netspeed %i", 10000);
			//	g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(cmd);		
			//}
			//else
			//if(strController == "NetSpeed3Rb")
			//{
			//	sprintf(cmd, "n_netspeed %i", 50000);
			//	g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(cmd);		
			//}

		}
		else
		if(strMainWnd == "RestartMsgBox")
		{
			if(strController == "RestartMsgBoxOKBn")
			{
				int d = g_kMistClient.IsButtonChecked("ColorDepth16Checkbox") ? 16 : 32;
				bool fullscreen = g_kMistClient.IsButtonChecked("FullscreenCheckbox") ? 1 : 0;

				if(g_kMistClient.IsButtonChecked("ScreenSize800x600Rb")) 
					g_kMistClient.m_pkRender->SetDisplay(800,600,d,fullscreen);
				if(g_kMistClient.IsButtonChecked("ScreenSize1024x768Rb")) 
					g_kMistClient.m_pkRender->SetDisplay(1024,768,d,fullscreen);
				if(g_kMistClient.IsButtonChecked("ScreenSize1280x1024Rb")) 
					g_kMistClient.m_pkRender->SetDisplay(1280,1024,d,fullscreen);
				if(g_kMistClient.IsButtonChecked("ScreenSize1600x1200Rb")) 
					g_kMistClient.m_pkRender->SetDisplay(1600,1200,d,fullscreen);

				if(g_kMistClient.IsButtonChecked("ScaleGUICheckbox"))
					g_kMistClient.m_pkGui->m_iScaleMode = 0;
				else
					//g_kMistClient.m_pkGui->m_iScaleMode = 1;

				g_kMistClient.ShowWnd("RestartMsgBox",	false);
				g_kMistClient.m_pkZeroFps->QuitEngine();
			}
			else
			if(strController == "RestartMsgBoxCancelBn")
			{
				g_kMistClient.ShowWnd("RestartMsgBox",	false);
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
			//if( strController	==	"NewServerNameEB"	)
			//	g_kMistClient.m_pkGui->SetFocus(	g_kMistClient.GetWnd("NewServerIPName") ); 
		}
	}

	////////////////////////////////////////////////////////////////////////////
	//	ZGM_SCN_SETPOS (hantera meddelanden för en slider)
	////////////////////////////////////////////////////////////////////////////
	if(msg == ZGM_SCN_SETPOS)
	{
		if(strMainWnd == "OptionsPageVideo")
		{
			if(strController == "GammaSliderBlue" || 
				strController == "GammaSliderGreen" ||
				strController == "GammaSliderRed")
			{
				const char* szGammaSliders[3] = {
					"GammaSliderRed","GammaSliderGreen","GammaSliderBlue"
				};

				char* szGammaLabels[3] = {
					"GammaRedLabel","GammaGreenLabel","GammaBlueLabel"
				};

				char szText[10];
				float pos = (float)((ZGuiSlider*)
					g_kMistClient.GetWnd(strController))->ZGuiSlider::GetPos();

				for(int i=0; i<3; i++)
				{
					if( strController	==	szGammaSliders[i]	)
					{
						float fGamma = 5.0f * (pos / 100.0f);
						sprintf(szText, "%.2f", fGamma);
						
						g_kMistClient.SetText(szGammaLabels[i], szText);
					}
				}

				if(g_kMistClient.IsButtonChecked("LockGammaColorsCheckbox"))
				{
					for(int i=0; i<3; i++)
					{
						if( strController	!=	szGammaSliders[i] )
						{
							((ZGuiSlider*)g_kMistClient.GetWnd(
								szGammaSliders[i]))->SetPos((int)pos,false,true);
							g_kMistClient.SetText(szGammaLabels[i], szText);
						}
					}
				}

				g_kMistClient.m_pkZShaderSystem->SetGamma(
					(float)atof(g_kMistClient.GetText("GammaRedLabel")),
					(float)atof(g_kMistClient.GetText("GammaGreenLabel")), 
					(float)atof(g_kMistClient.GetText("GammaBlueLabel")));
			}

		}
		else
		if(strMainWnd == "OptionsPageAudio")
		{
			//if(strController == "MusicVolumeSlider")
			//{
			//	float pos;
			//	pos = ((ZGuiSlider*)g_kMistClient.GetWnd("MusicVolumeSlider"))->ZGuiSlider::GetPos();
			//	g_kMistClient.m_pkAudioSys->SetMusicVolume(pos / 100.0f);
			//}
			//else
			if(strController == "AudioVolumeSlider")
			{
				if(((ZGuiCheckbox*)g_kMistClient.GetWnd("AudioOnOffCheckbox"))->IsChecked() == false)
				{
					float pos;
					pos = ((ZGuiSlider*)g_kMistClient.GetWnd("AudioVolumeSlider"))->ZGuiSlider::GetPos();
					g_kMistClient.m_pkAudioSys->SetMainVolume(pos / 100.0f);					
				}
			}			
		}
		else
		if(strMainWnd == "OptionsPageController")
		{
			if(strController == "MouseSensSlider")
			{
				float pos = ((ZGuiSlider*)g_kMistClient.GetWnd("MouseSensSlider"))->ZGuiSlider::GetPos();

				pos = 5.0f * (pos / 100.0f);
				if(pos < 0.01f) pos = 0.01f;
				
				char szCmd[25];
				sprintf(szCmd, "i_mousesens %.3f", pos);
				g_kMistClient.m_pkZeroFps->m_pkConsole->Execute(szCmd);
			}
		}
		else
		if(strMainWnd == "OptionsPageGame")
		{
			if(strController == "NetSpeedSlider")
			{
				int pos = ((ZGuiSlider*)g_kMistClient.GetWnd("NetSpeedSlider"))->ZGuiSlider::GetPos();
	
				char szText[16];
				sprintf(szText, "%i", pos);				
				g_kMistClient.SetText("NetSpeedRes", szText);

				g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_iNewNetSpeed = pos;
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////
	//	ZGM_CBN_SELENDOK (hantera meddelanden för en combobox)
	////////////////////////////////////////////////////////////////////////////
	if(msg == ZGM_CBN_SELENDOK)
	{
		if(strMainWnd == "OptionsPageGraphic")
		{
			if(strController == "ShadowGroupsComboBox")
			{
				string item = g_kMistClient.GetSelItem("ShadowGroupsComboBox");

				if(item == "Big enviroment objects (zones ,trees)")
					g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_iCurrentShadowGroup = 1;
				else
				if(item == "Small enviroment objects (rocks,tables)")
					g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_iCurrentShadowGroup = 2;
				else
				if(item == "Players (player characters)")
					g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_iCurrentShadowGroup = 3;
				else
				if(item == "Game objects (inventory objects)")
					g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_iCurrentShadowGroup = 4;
				else
				if(item == "Creatures (non playable monsters)")
					g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_iCurrentShadowGroup = 5;

				g_kMistClient.CheckButton("EnableShadowGroup", 
					g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_abEnabledShadowGroups[
						g_kMistClient.m_pkOptionsDlg->m_kOptionsValues.m_iCurrentShadowGroup]);
			}
		}
	}		
}







