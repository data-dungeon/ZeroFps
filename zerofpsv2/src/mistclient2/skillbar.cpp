#include "skillbar.h"
#include "mistclient.h"
#include <iostream>

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
		temp.m_strSkillIcon = "empty.tga";
		temp.m_fReloadTimeLeft = -1;
		
		
		string name = "SkillButton"+IntToString(i);
		temp.m_pkButton = (ZGuiButton*)g_kMistClient.CreateWnd(Button,(char*)name.c_str(), "SkillBar", "", 96 + i*32, 0, 32, 32, 0);
		temp.m_pkButton->SetButtonUpSkin(new ZGuiSkin());
		temp.m_pkButton->SetButtonDownSkin(new ZGuiSkin());
  		temp.m_pkButton->SetButtonHighLightSkin(new ZGuiSkin());		
		temp.m_pkButton->SetFocus(true);
// 		temp.m_pkButton->Enable();
		
		
		ZGuiSkin* pkSkin;
		pkSkin = temp.m_pkButton->GetButtonUpSkin();
			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;

		pkSkin = temp.m_pkButton->GetButtonDownSkin();
			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;
			pkSkin->m_afBkColor[0] = 0.5;
			pkSkin->m_afBkColor[1] = 0.5;
			pkSkin->m_afBkColor[2] = 0.5;
			
		pkSkin = temp.m_pkButton->GetButtonHighLightSkin();
			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;
/*			pkSkin->m_afBkColor[0] = 1;
			pkSkin->m_afBkColor[1] = 0.8;
			pkSkin->m_afBkColor[2] = 0.8;	*/		
// 			pkSkin->m_afBorderColor[0] = 0;
// 			pkSkin->m_afBorderColor[1] = 0;
// 			pkSkin->m_afBorderColor[2] = 0;
// 			pkSkin->m_unBorderSize = 2;	
			
			
		m_kSkills.push_back(temp);	
	}
	
		
	bHaveBeenInitialized = true;
}

void SkillBar::UpdateList(vector<SkillInfo> kSkillInfo)
{
	static string strIconDir = "skills/";

	if(kSkillInfo.size() != m_kSkills.size())
	{
		cout<<"ERROR: bad size on skilllist"<<endl;
		return;
	}
	
	for(int i = 0;i<kSkillInfo.size();i++)
	{
		m_kSkills[i].m_strSkillName = kSkillInfo[i].m_strSkillName;
		m_kSkills[i].m_strSkillIcon = kSkillInfo[i].m_strSkillIcon;
		m_kSkills[i].m_fReloadTimeLeft = kSkillInfo[i].m_fReloadTimeLeft;
		
		
		if(m_kSkills[i].m_strSkillName.empty())
		{
			m_kSkills[i].m_pkButton->GetButtonUpSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga");
			m_kSkills[i].m_pkButton->GetButtonDownSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga");
			m_kSkills[i].m_pkButton->GetButtonHighLightSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga");
		}
		else
		{
			m_kSkills[i].m_pkButton->GetButtonUpSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes((strIconDir+m_kSkills[i].m_strSkillIcon).c_str());
			m_kSkills[i].m_pkButton->GetButtonDownSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes((strIconDir+m_kSkills[i].m_strSkillIcon).c_str());
			m_kSkills[i].m_pkButton->GetButtonHighLightSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes((strIconDir+m_kSkills[i].m_strSkillIcon).c_str());
		}
	}
}


void SkillBar::HandleCommand(const string& strCommand)
{
	int iButton = atoi(&strCommand.c_str()[11]);

	if(!m_kSkills[iButton].m_strSkillName.empty())
		g_kMistClient.SendAddSkillToQueue(m_kSkills[iButton].m_strSkillName,g_kMistClient.GetCurrentTargetID());
}








