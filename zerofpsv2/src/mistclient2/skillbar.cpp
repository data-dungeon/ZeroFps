#include "skillbar.h"
#include "mistclient.h"
#include <iostream>
#include "../mcommon/p_characterproperty.h"

extern MistClient	g_kMistClient;

SkillBar::SkillBar()
{

}

void SkillBar::Init()
{
	static bool bHaveBeenInitialized = false;
	
	if(bHaveBeenInitialized)
		return;


	SkillInfo temp;
	for(int i =0;i<19;i++)
	{
		temp.m_strSkillName = "";
		temp.m_fReloadTimeLeft = -1;
		temp.m_fReloadTimeTotal = -1;
		
		
		string name = "SkillButton"+IntToString(i);
		temp.m_pkButton = (ZGuiButton*)g_kMistClient.CreateWnd(Button,(char*)name.c_str(), "SkillBar", "", 96 + i*32, 0, 32, 32, 0);
		temp.m_pkButton->SetButtonUpSkin(new ZGuiSkin());
		temp.m_pkButton->SetButtonDownSkin(new ZGuiSkin());
		temp.m_pkButton->SetButtonHighLightSkin(temp.m_pkButton->GetButtonUpSkin());		
//   		temp.m_pkButton->SetButtonHighLightSkin(new ZGuiSkin());		
		temp.m_pkButton->SetFocus(true);
		
		
		ZGuiSkin* pkSkin;
		pkSkin = temp.m_pkButton->GetButtonUpSkin();
			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;

		pkSkin = temp.m_pkButton->GetButtonDownSkin();
			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;
			
//  		pkSkin = temp.m_pkButton->GetButtonHighLightSkin();
//  			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;
			
			
		m_kSkills.push_back(temp);	
	}
	
		
	bHaveBeenInitialized = true;
}

void SkillBar::UpdateList(vector<SkillNetInfo> kSkillInfo)
{
	static string strIconDir = "skills/";


	int iPos;
	for(int i = 0;i<kSkillInfo.size();i++)
	{
		iPos = kSkillInfo[i].m_cPos;
	
		m_kSkills[iPos].m_strSkillName = 		kSkillInfo[i].m_strSkillName;
		m_kSkills[iPos].m_strSkillScreenName = kSkillInfo[i].m_strSkillScreenName;
		m_kSkills[iPos].m_fReloadTimeLeft = 	kSkillInfo[i].m_fReloadTimeLeft;
		m_kSkills[iPos].m_fReloadTimeTotal = 	kSkillInfo[i].m_fReloadTimeTotal;
		m_kSkills[iPos].m_iSkillType = 			kSkillInfo[i].m_cSkillType;
		
		if(m_kSkills[iPos].m_strSkillName.empty())
		{
			m_kSkills[iPos].m_pkButton->GetButtonUpSkin()->m_iBkTexID = 		g_kMistClient.LoadGuiTextureByRes("skills/empty.tga");
			m_kSkills[iPos].m_pkButton->GetButtonDownSkin()->m_iBkTexID = 		g_kMistClient.LoadGuiTextureByRes("skills/empty.tga");
		}
		else
		{
//				av nån anledning så försvinner knapparna när man stänger inventory fönstret =/ om detta är på		
// 			if(m_kSkills[iPos].m_pkButton)
// 				g_kMistClient.m_pkGui->GetToolTip()->RemoveToolTip(m_kSkills[iPos].m_pkButton);
 			g_kMistClient.m_pkGui->GetToolTip()->AddToolTip(m_kSkills[iPos].m_pkButton, 
				m_kSkills[iPos].m_strSkillScreenName, 50.0f);

			m_kSkills[iPos].m_pkButton->GetButtonUpSkin()->m_iBkTexID = 		g_kMistClient.LoadGuiTextureByRes((strIconDir+kSkillInfo[i].m_strSkillIcon).c_str());
			m_kSkills[iPos].m_pkButton->GetButtonDownSkin()->m_iBkTexID = 		g_kMistClient.LoadGuiTextureByRes((strIconDir+kSkillInfo[i].m_strSkillIcon).c_str());
		}
	}
}


void SkillBar::HandleCommand(const string& strCommand)
{
	int iButton = atoi(&strCommand.c_str()[11]);

	if(!m_kSkills[iButton].m_strSkillName.empty())
		g_kMistClient.SendAddSkillToQueue(m_kSkills[iButton].m_strSkillName,g_kMistClient.GetCurrentTargetID());
}

void SkillBar::Update(float fTimeDiff)
{
	ZGuiSkin* pkSkin;
	bool bCombatMode = g_kMistClient.GetCombatMode();

	for(int i = 0;i<m_kSkills.size();i++)
	{
		if(!m_kSkills[i].m_strSkillName.empty())
		{
			//still reloading
			if( m_kSkills[i].m_fReloadTimeLeft > 0)
			{		
				m_kSkills[i].m_fReloadTimeLeft -= fTimeDiff;				
				float fT = 1 - m_kSkills[i].m_fReloadTimeLeft / m_kSkills[i].m_fReloadTimeTotal;
			
				//disable unused skilles
				if(!bCombatMode && m_kSkills[i].m_iSkillType == eOFFENSIVE)
				{
					pkSkin = m_kSkills[i].m_pkButton->GetButtonUpSkin();							
					pkSkin->m_afBkColor[0] = fT;
					pkSkin->m_afBkColor[1] = 0;
					pkSkin->m_afBkColor[2] = 0;							
							
					pkSkin = m_kSkills[i].m_pkButton->GetButtonDownSkin();							
					pkSkin->m_afBkColor[0] = fT;
					pkSkin->m_afBkColor[1] = 0;
					pkSkin->m_afBkColor[2] = 0;					
				}				
				else
				{
					pkSkin = m_kSkills[i].m_pkButton->GetButtonUpSkin();							
					pkSkin->m_afBkColor[0] = fT;
					pkSkin->m_afBkColor[1] = fT;
					pkSkin->m_afBkColor[2] = fT;							
							
					pkSkin = m_kSkills[i].m_pkButton->GetButtonDownSkin();							
					pkSkin->m_afBkColor[0] = fT;
					pkSkin->m_afBkColor[1] = fT;
					pkSkin->m_afBkColor[2] = fT;											
				}
			}
			else
			{
			
				if(!bCombatMode && m_kSkills[i].m_iSkillType == eOFFENSIVE)
				{			
					pkSkin = m_kSkills[i].m_pkButton->GetButtonUpSkin();							
					pkSkin->m_afBkColor[0] = 1.0;
					pkSkin->m_afBkColor[1] = 0.0;
					pkSkin->m_afBkColor[2] = 0.0;				
					
					pkSkin = m_kSkills[i].m_pkButton->GetButtonDownSkin();							
					pkSkin->m_afBkColor[0] = 0.5;
					pkSkin->m_afBkColor[1] = 0.0;
					pkSkin->m_afBkColor[2] = 0.0;										
				}
				else
				{			
					pkSkin = m_kSkills[i].m_pkButton->GetButtonUpSkin();							
					pkSkin->m_afBkColor[0] = 1.0;
					pkSkin->m_afBkColor[1] = 1.0;
					pkSkin->m_afBkColor[2] = 1.0;				
					
					pkSkin = m_kSkills[i].m_pkButton->GetButtonDownSkin();							
					pkSkin->m_afBkColor[0] = 0.5;
					pkSkin->m_afBkColor[1] = 0.5;
					pkSkin->m_afBkColor[2] = 0.5;							
				}			
			}			
		}
		else
		{
			pkSkin = m_kSkills[i].m_pkButton->GetButtonUpSkin();							
			pkSkin->m_afBkColor[0] = 1;
			pkSkin->m_afBkColor[1] = 1;
			pkSkin->m_afBkColor[2] = 1;							
					
			pkSkin = m_kSkills[i].m_pkButton->GetButtonDownSkin();							
			pkSkin->m_afBkColor[0] = 1;
			pkSkin->m_afBkColor[1] = 1;
			pkSkin->m_afBkColor[2] = 1;						
		}
	}
}








