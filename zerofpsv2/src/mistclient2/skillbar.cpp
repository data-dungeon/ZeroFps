#include "skillbar.h"
#include "mistclient.h"
#include <iostream>

extern MistClient	g_kMistClient;

SkillBar::SkillBar()
{
}

void SkillBar::Update()
{
	ZGuiWnd* pkSkillbar = (ZGuiWnd*)g_kMistClient.GetWnd("SkillBar");

	static bool s_bOnes = false;

 	if(pkSkillbar && s_bOnes == false)
 	{
		s_bOnes = true;

 		ZGuiLabel* m_pkIconSelection = (ZGuiLabel*)g_kMistClient.CreateWnd(Label,
			"testtest", "SkillBar", "SkillBar", 0, 0, 32, 32, 0);
 	
 	 	m_pkIconSelection->SetSkin(new ZGuiSkin());
 	 	m_pkIconSelection->GetSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes(
 	 		"actions/action_selection_border.tga") ;
 	}
}