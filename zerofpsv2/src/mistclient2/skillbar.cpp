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
		temp.m_bInUse = false;
		temp.m_strSkillName = "";
		temp.m_strSkillIcon = "emptyskill.bmp";
		temp.m_fReloadTimeLeft = -1;
		
		
		string name = "SkillButton"+IntToString(i);
		temp.m_pkButton = (ZGuiButton*)g_kMistClient.CreateWnd(Label,(char*)name.c_str(), "SkillBar", "", 96 + i*32, 0, 32, 32, 0);
		temp.m_pkButton->SetButtonUpSkin(new ZGuiSkin());
		temp.m_pkButton->SetButtonDownSkin(new ZGuiSkin());
		temp.m_pkButton->SetFocus(true);
		temp.m_pkButton->Enable();
//  		temp.m_pkButton->SetButtonHighLightSkin(new ZGuiSkin());
		
		ZGuiSkin* pkSkin;
		pkSkin = temp.m_pkButton->GetButtonUpSkin();
			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;

		pkSkin = temp.m_pkButton->GetButtonDownSkin();
			pkSkin->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/empty.tga") ;
			pkSkin->m_afBkColor[0] = 0.5;
			pkSkin->m_afBkColor[1] = 0.5;
			pkSkin->m_afBkColor[2] = 0.5;
			
			pkSkin->m_afBorderColor[0] = 0;
			pkSkin->m_afBorderColor[1] = 0;
			pkSkin->m_afBorderColor[2] = 0;
			pkSkin->m_unBorderSize = 2;	
			
			
		m_kSkills.push_back(temp);	
	}
	
		
	bHaveBeenInitialized = true;
}

void SkillBar::Update()
{
	if(m_kSkills.empty())
		return;


	m_kSkills[0].m_pkButton->GetButtonUpSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes("skills/basic_attack.tga");
}











