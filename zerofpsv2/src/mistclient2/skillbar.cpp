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

	//currect default action marker
	m_pkCurrentPrimarySkill = (ZGuiLabel*) g_kMistClient.CreateWnd(Label, "PrimarySkillMarker", "SkillBar", "", 96 + 0*32, 0, 32, 32, 0);
	m_pkCurrentPrimarySkill->Hide();
	m_pkCurrentPrimarySkill->SetSkin(new ZGuiSkin());
	m_pkCurrentPrimarySkill->GetSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("defaultskill.tga");	
	
	m_pkCurrentSecondarySkill = (ZGuiLabel*) g_kMistClient.CreateWnd(Label, "SecondarySkillMarker", "SkillBar", "", 96 + 0*32, 0, 32, 32, 0);
	m_pkCurrentSecondarySkill->Hide();
	m_pkCurrentSecondarySkill->SetSkin(new ZGuiSkin());
	m_pkCurrentSecondarySkill->GetSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("secondaryskill.tga");	

	if(ZGuiWnd* pkWnd = g_kMistClient.GetWnd("SkillBar"))
		pkWnd->m_bAcceptRightClicks = true;

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
		temp.m_pkButton->SetFocus(true);
		temp.m_pkButton->m_bAcceptRightClicks = true;
		
		ZGuiSkin* pkSkin;
		pkSkin = temp.m_pkButton->GetButtonUpSkin();
			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;

		pkSkin = temp.m_pkButton->GetButtonDownSkin();
			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;
			
			
			
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
		m_kSkills[iPos].m_iTargetType = 			kSkillInfo[i].m_cTargetType;



		if(m_kSkills[iPos].m_strSkillName.empty())
		{
			m_kSkills[iPos].m_pkButton->GetButtonUpSkin()->m_iBkTexID = 		g_kMistClient.LoadGuiTextureByRes("skills/empty.tga");
			m_kSkills[iPos].m_pkButton->GetButtonDownSkin()->m_iBkTexID = 		g_kMistClient.LoadGuiTextureByRes("skills/empty.tga");
		}
		else
		{
			if(m_kSkills[iPos].m_pkButton)
				g_kMistClient.m_pkGui->GetToolTip()->RemoveToolTip(m_kSkills[iPos].m_pkButton->GetName());

 			g_kMistClient.m_pkGui->GetToolTip()->AddToolTip(m_kSkills[iPos].m_pkButton->GetName(), 
				m_kSkills[iPos].m_strSkillScreenName, 50.0f);

			m_kSkills[iPos].m_pkButton->GetButtonUpSkin()->m_iBkTexID = 		g_kMistClient.LoadGuiTextureByRes((strIconDir+kSkillInfo[i].m_strSkillIcon).c_str());
			m_kSkills[iPos].m_pkButton->GetButtonDownSkin()->m_iBkTexID = 		g_kMistClient.LoadGuiTextureByRes((strIconDir+kSkillInfo[i].m_strSkillIcon).c_str());
		}
	}
}


void SkillBar::HandleCommand(int iButton,const string& strCommand)
{
	int iSkill = atoi(&strCommand.c_str()[11]);

	// If skillwindows is up then we set the skills we wish to have on the skillbar.
	ZGuiWnd* pkWndSkill = g_kMistClient.GetWnd("SkillWnd");
	if(pkWndSkill && pkWndSkill->IsVisible())
	{
		string strSkill = g_kMistClient.GetSkillTreeSkill();
		g_kMistClient.SendAddSkillToSkillbar(strSkill,iSkill);
	}

	if(iButton == 0)
	{
		if(m_kSkills[iSkill].m_iTargetType == eSELF)
		{
// 			g_kMistClient.SendAddSkillToQueue(m_kSkills[iSkill].m_strSkillName,-1);
			g_kMistClient.SendUseSkill(m_kSkills[iSkill].m_strSkillName,-1,false);
		
		}
		else
		{
			
	// 		g_kMistClient.SendSetDefaultAttack(m_kSkills[iSkill].m_strSkillName);
			m_strPrimarySkill = m_kSkills[iSkill].m_strSkillName;
	// 		m_strPrimarySkill = m_kSkills[iSkill].m_strSkillName;
	//  		
// 			for(int i = 0;i<m_kSkills.size();i++)
// 				m_kSkills[i].m_bDefaultAttack = false;
// 			
// 			m_kSkills[iSkill].m_bDefaultAttack = true;
		}	
	
		
// 		if(!m_kSkills[iSkill].m_strSkillName.empty())
// 			g_kMistClient.SendAddSkillToQueue(m_kSkills[iSkill].m_strSkillName,g_kMistClient.GetCurrentTargetID());
	}
	else if(iButton == 1)
	{
		if(m_kSkills[iSkill].m_iTargetType == eSELF)
		{
// 			g_kMistClient.SendAddSkillToQueue(m_kSkills[iSkill].m_strSkillName,-1);
			g_kMistClient.SendUseSkill(m_kSkills[iSkill].m_strSkillName,-1,false);
		}
		else
		{	
			m_strSecondarySkill = m_kSkills[iSkill].m_strSkillName;	
		}
	
// 		m_strPrimarySkill  = m_kSkills[iSkill].m_strSkillName;
// 		m_kSkills[iSkill].m_bDefaultAttack = true;
	
// 		if(!m_kSkills[iSkill].m_strSkillName.empty())
// 		{
// 			if(m_kSkills[iSkill].m_bDefaultAttack)			
// 				g_kMistClient.SendSetDefaultAttack("");
// 			else
// 				g_kMistClient.SendSetDefaultAttack(m_kSkills[iSkill].m_strSkillName);
// 		}
	}
	
}

void SkillBar::Update(float fTimeDiff)
{
	bool bHaveSetPrimarySkill = false;
	bool bHaveSetSecondarySkill = false;

	ZGuiSkin* pkSkin;
	bool bCombatMode = g_kMistClient.GetCombatMode();

	for(int i = 0;i<m_kSkills.size();i++)
	{
		if(!m_kSkills[i].m_strSkillName.empty())
		{
			//default attack marker
// 			if(m_kSkills[i].m_bDefaultAttack)
			if(m_strPrimarySkill == m_kSkills[i].m_strSkillName)
			{			
				bHaveSetPrimarySkill = true;
				
				Rect temp = m_kSkills[i].m_pkButton->GetWndRect();				
 				m_pkCurrentPrimarySkill->SetPos(temp.Left, 0, false, true); 
 				m_pkCurrentPrimarySkill->SetZValue(99);
				m_pkCurrentPrimarySkill->Show();
			}		
		
			if(m_strSecondarySkill == m_kSkills[i].m_strSkillName)
			{			
				bHaveSetSecondarySkill = true;
				
				Rect temp = m_kSkills[i].m_pkButton->GetWndRect();				
 				m_pkCurrentSecondarySkill->SetPos(temp.Left, 0, false, true); 
 				m_pkCurrentSecondarySkill->SetZValue(99);
				m_pkCurrentSecondarySkill->Show();
			}				
		
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
	
	
	//if no default attack was set, hide marker
	if(!bHaveSetPrimarySkill)
	   m_pkCurrentPrimarySkill->Hide();

	if(!bHaveSetSecondarySkill)
	   m_pkCurrentSecondarySkill->Hide();

}

		







