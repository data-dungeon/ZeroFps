#include "skillbar.h"
#include "mistclient.h"
#include <iostream>

extern MistClient	g_kMistClient;

SkillBar::SkillBar()
{
	cout<<"skillbar created"<<endl;


/*
	ZGuiLabel* m_pkIconSelection = (ZGuiLabel*)g_kMistClient.CreateWnd(Label,"test", "SkillBar", "", 0, 0, 32, 32, 0);

// 	m_pkIconSelection->SetSkin(new ZGuiSkin());
// 	m_pkIconSelection->GetSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes(
// 		"actions/action_selection_border.tga") ;	

	m_pkIconSelection->SetText("hej hopp",true);
	m_pkIconSelection->Show();*/

}

void SkillBar::Update()
{
// 	if(ZGuiWnd* pkWnd= (ZGuiWnd*)g_kMistClient.GetWnd("SkillBar"))
// 	{
// 		pkWnd->SetPos(0,50);	
// // 		pkWnd->	
// 	}
// 	else
// 	{
// 		cout<<"ERROR------------------ FUCK"<<endl;
// 	}
// 	
// 	if(!g_kMistClient.GetWnd("testtest"))
// 	{
// 		ZGuiLabel* m_pkIconSelection = (ZGuiLabel*)g_kMistClient.CreateWnd(Label,"testtest", "SkillBar", "", 0, 0, 32, 32, 0);
// 	
// 	// 	m_pkIconSelection->SetSkin(new ZGuiSkin());
// 	// 	m_pkIconSelection->GetSkin()->m_iBkTexID = g_kMistClient.LoadGuiTextureByRes(
// 	// 		"actions/action_selection_border.tga") ;	
// 	
// 		m_pkIconSelection->SetText("hej hopp",true);
// 		m_pkIconSelection->Show();
// 	}
}