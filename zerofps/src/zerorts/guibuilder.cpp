// guibuilder.cpp: implementation of the GuiBuilder class.
//
//////////////////////////////////////////////////////////////////////

#include "guibuilder.h"
#include "../zerofps/render/texturemanager.h"
#include "../zerofps/engine/input.h"
#include "../zerofps/gui/zguiresourcemanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GuiBuilder::GuiBuilder(TextureManager* pkTexMan, ZGui* pkGui)
{
	m_pkTexMan = pkTexMan;
	m_pkGui = pkGui;
	InitSkins();
}	

GuiBuilder::~GuiBuilder()
{

}


bool GuiBuilder::InitSkins()
{
	int piss = m_pkTexMan->Load("file:../data/textures/piss.bmp", 0); // första misslyckas, I don't know...
	int bn_up = m_pkTexMan->Load("file:../data/textures/button_up.bmp", 0);
	int bn_down = m_pkTexMan->Load("file:../data/textures/button_down.bmp", 0);
	int bn_focus = m_pkTexMan->Load("file:../data/textures/button_focus.bmp", 0);

	int attack_bnu = m_pkTexMan->Load("file:../data/textures/attack_bnu.bmp", 0);
	int attack_bnd = m_pkTexMan->Load("file:../data/textures/attack_bnd.bmp", 0);
	int attack_bnf = m_pkTexMan->Load("file:../data/textures/attack_bnf.bmp", 0);

	int build_bnu = m_pkTexMan->Load("file:../data/textures/build_bnu.bmp", 0);
	int build_bnd = m_pkTexMan->Load("file:../data/textures/build_bnd.bmp", 0);
	int build_bnf = m_pkTexMan->Load("file:../data/textures/build_bnf.bmp", 0);

	int move_bnu = m_pkTexMan->Load("file:../data/textures/move_bnu.bmp", 0);
	int move_bnd = m_pkTexMan->Load("file:../data/textures/move_bnd.bmp", 0);
	int move_bnf = m_pkTexMan->Load("file:../data/textures/move_bnf.bmp", 0);

	int stop_bnu = m_pkTexMan->Load("file:../data/textures/stop_bnu.bmp", 0);
	int stop_bnd = m_pkTexMan->Load("file:../data/textures/stop_bnd.bmp", 0);
	int stop_bnf = m_pkTexMan->Load("file:../data/textures/stop_bnf.bmp", 0);

	int repair_bnu = m_pkTexMan->Load("file:../data/textures/repair_bnu.bmp", 0);
	int repair_bnd = m_pkTexMan->Load("file:../data/textures/repair_bnd.bmp", 0);
	int repair_bnf = m_pkTexMan->Load("file:../data/textures/repair_bnf.bmp", 0);

	int guard_bnu = m_pkTexMan->Load("file:../data/textures/guard_bnu.bmp", 0);
	int guard_bnd = m_pkTexMan->Load("file:../data/textures/guard_bnd.bmp", 0);
	int guard_bnf = m_pkTexMan->Load("file:../data/textures/guard_bnf.bmp", 0);

	int minimap = m_pkTexMan->Load("file:../data/textures/minimap.bmp", 0);

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("attack_bnu"), 
		new ZGuiSkin(attack_bnu, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("attack_bnd"), 
		new ZGuiSkin(attack_bnd, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("attack_bnf"), 
		new ZGuiSkin(attack_bnf, false) ) ); 

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("build_bnu"), 
		new ZGuiSkin(build_bnu, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("build_bnd"), 
		new ZGuiSkin(build_bnd, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("build_bnf"), 
		new ZGuiSkin(build_bnf, false) ) ); 

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("move_bnu"), 
		new ZGuiSkin(move_bnu, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("move_bnd"), 
		new ZGuiSkin(move_bnd, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("move_bnf"), 
		new ZGuiSkin(move_bnf, false) ) ); 

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("stop_bnu"), 
		new ZGuiSkin(stop_bnu, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("stop_bnd"), 
		new ZGuiSkin(stop_bnd, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("stop_bnf"), 
		new ZGuiSkin(stop_bnf, false) ) ); 

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("repair_bnu"), 
		new ZGuiSkin(repair_bnu, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("repair_bnd"), 
		new ZGuiSkin(repair_bnd, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("repair_bnf"), 
		new ZGuiSkin(repair_bnf, false) ) ); 

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("guard_bnu"), 
		new ZGuiSkin(guard_bnu, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("guard_bnd"), 
		new ZGuiSkin(guard_bnd, false) ) ); 
	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("guard_bnf"), 
		new ZGuiSkin(guard_bnf, false) ) ); 

	m_kSkinMap.insert( map<string, ZGuiSkin*>::value_type(string("minimap"), 
		new ZGuiSkin(minimap, false) ) ); 

	return true;
}

ZGuiSkin* GuiBuilder::GetSkin(char* strName)
{
	static ZGuiSkin* fail_skin;
	static bool init = false;

	if(init == false)
	{
		int tex = m_pkTexMan->Load("file:../data/textures/notex.bmp", 0);
		fail_skin = new ZGuiSkin(tex, -1, -1, -1, 255, 255, 255, 255, 255, 255);
		init = true;
	}

	map<string, ZGuiSkin*>::iterator ret = m_kSkinMap.find(string(strName));

	if(ret != m_kSkinMap.end())
		return ret->second;
	
	return fail_skin;
}

ZGuiWnd* GuiBuilder::Get(char* strName)
{
	return m_pkGui->GetResMan()->Wnd(string(strName));
}

ZGuiLabel* GuiBuilder::CreateLabel(ZGuiWnd* pkParent, int iID, int x, int y, int w, 
								   int h, char* strText, char* szRegName)
{
	ZGuiLabel* pkLabel = new ZGuiLabel(Rect(x,y,x+w,y+h), pkParent, true, iID);
	
	if(strText)
		pkLabel->SetText(strText);

	if(szRegName != NULL)
		m_pkGui->RegisterWindow(pkLabel, szRegName);

	pkLabel->SetGUI(m_pkGui);

	return pkLabel;
}