// spelldlg.cpp: implementation of the SpellDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "spelldlg.h"
#include "quickboard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SpellDlg::SpellDlg(ZGuiApp* pkApp, QuickBoard* pkQuickBoard)
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));

	m_bAutoCloseWnd = false;
	m_pkQuickBoard = pkQuickBoard;
	m_pkApp = pkApp;
	Init();
}

SpellDlg::~SpellDlg()
{

}

void SpellDlg::Update()
{
	if(m_pkDialog->IsVisible() == false)
	{
		return;
	}
}

void SpellDlg::Init()
{
	int screen_w = m_pkApp->GetWidth(); 
	int screen_h = m_pkApp->GetHeight();

	//
	// Create main window
	//

	m_pkApp->CreateWnd(Wnd, "SpellBookMainWnd", "MainWnd", "", screen_w-400, 0, 400, 484, 0);
	m_pkDialog = m_pkApp->GetWnd("SpellBookMainWnd");
	m_pkDialog->SetSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spellbookwnd.bmp", 0),0));

	//
	// Create auto close button
	//

	m_pkApp->CreateWnd(Checkbox, "AutoCloseSpellWnd", "SpellBookMainWnd", "", 358, 450, 16, 16, 0);
	m_pkAutoCloseSpellCheckBox = (ZGuiCheckbox*) m_pkApp->GetWnd("AutoCloseSpellWnd");


	//
	// Create spellbuttons
	//

	ZGuiSkin* pkButtonSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_u.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);

	ZGuiSkin* pkButtonSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/quickbn_d.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/quickbn_a.bmp", 0),0);

	char szName[50];

	for(int y=0; y<SPELL_ROWS; y++)
		for(int x=0; x<SPELL_COLS; x++)
		{
			char szName[50];
			sprintf(szName, "SpellBookSpellSlot%i", y*10+x);
			m_pkApp->CreateWnd(Button, szName, "SpellBookMainWnd", "", 32+x*64, 100+y*64, 64, 64, 0);

			ZGuiButton* pkButton = (ZGuiButton*)m_pkApp->GetWnd(szName);
			pkButton->SetButtonUpSkin(pkButtonSkinUp);
			pkButton->SetButtonHighLightSkin(pkButtonSkinUp);
			pkButton->SetButtonDownSkin(pkButtonSkinDown);
			pkButton->m_bUseAlhpaTest = false;

			m_pkSpellButtons[y][x] = pkButton;
		}

	//
	// Create a textbox to print spell desctription.
	//

	m_pkApp->CreateWnd(Textbox, "SpellDescTextbox", "SpellBookMainWnd", 
		"", 32, 370, 300, 60, EB_IS_MULTILINE | READ_ONLY);

	//
	// Label that displayes selected spell school and selected spell
	//

	m_pkApp->CreateWnd(Label, "SpellBookLabel", "SpellBookMainWnd", "Spellbook School 1", 32, 71, 350, 16, 0);

	//
	// Create buttons for selection spell school
	//

	ZGuiSkin* pkFireSchoolSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spell_category_fire_u.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/spell_category_a.bmp", 0),0);

	ZGuiSkin* pkFireSchoolSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spell_category_fire_d.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/spell_category_a.bmp", 0),0);

	ZGuiSkin* pkNecromancerSchoolSkinUp = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spell_category_necromancer_u.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/spell_category_necromancer_a.bmp", 0),0);

	ZGuiSkin* pkNecromancerSchoolSkinDown = new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spell_category_necromancer_d.bmp", 0),
		m_pkTexMan->Load("/data/textures/gui/spell_category_necromancer_a.bmp", 0),0);

	int i;

	for(i=0; i<NUM_SCHOOLS; i++)
	{
		sprintf(szName, "SpellSchoolBn%i", i);
		m_pkApp->CreateWnd(Checkbox, szName, "SpellBookMainWnd", "", 32+i*32, 10, 32, 32, 0);

		ZGuiCheckbox* pkButton = (ZGuiCheckbox*) m_pkApp->GetWnd(szName);
		m_pkSchoolButtons[i] = pkButton;

		pkButton->m_bUseAlhpaTest = true;
		pkButton->SetButtonUncheckedSkin(pkFireSchoolSkinUp);
		pkButton->SetButtonCheckedSkin(pkFireSchoolSkinDown);
	}

	m_pkSchoolButtons[0]->SetButtonUncheckedSkin(pkFireSchoolSkinUp);
	m_pkSchoolButtons[0]->SetButtonCheckedSkin(pkFireSchoolSkinDown);

	m_pkSchoolButtons[1]->SetButtonUncheckedSkin(pkNecromancerSchoolSkinUp);
	m_pkSchoolButtons[1]->SetButtonCheckedSkin(pkNecromancerSchoolSkinDown);

	m_pkSchoolButtons[0]->CheckButton();

	//
	// Create buttons for changing page in spellbook
	//

	char* szLabel[NUM_PAGES] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "10" };

	for(i=0; i<NUM_PAGES; i++)
	{
		sprintf(szName, "Level1SpellBn%i", i);
		m_pkApp->CreateWnd(Button, szName, "SpellBookMainWnd", szLabel[i], 32+i*28, 450, 24, 16, 0);

		m_pkPageButton[i] = (ZGuiButton*) m_pkApp->GetWnd(szName);

		m_pkPageButton[i]->SetButtonUpSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/spellbook_level_page_u.bmp", 0),0));
		m_pkPageButton[i]->SetButtonHighLightSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/spellbook_level_page_u.bmp", 0),0));
		m_pkPageButton[i]->SetButtonDownSkin(new ZGuiSkin(
			m_pkTexMan->Load("/data/textures/gui/spellbook_level_page_d.bmp", 0),0));
	}

	m_pkApp->CreateWnd(Label, "TestSpell", "SpellBookMainWnd", "", 32, 100, 64, 64, 0);
	m_pkApp->GetWnd("TestSpell")->SetSkin(new ZGuiSkin(
		m_pkTexMan->Load("/data/textures/gui/spells/lightingball.bmp", 0),0));

	SpellSlot* kNewSlot = new SpellSlot();
	kNewSlot->pkLabel = (ZGuiLabel*) m_pkApp->GetWnd("TestSpell");
	kNewSlot->x = 0;
	kNewSlot->y = 0;

	m_vkSpells.push_back(kNewSlot);

	//
	// Hide dialog from start
	//

	m_pkDialog->Hide();
}

void SpellDlg::OnCommand(ZGuiWnd* pkWndClicked)
{
	if(pkWndClicked == m_pkAutoCloseSpellCheckBox)
	{
		printf(pkWndClicked->GetName());
		m_bAutoCloseWnd = !m_bAutoCloseWnd;
		return;
	}

	for(int i=0; i<NUM_SCHOOLS; i++)
	{
		if(pkWndClicked == m_pkSchoolButtons[i])
		{
			for(int j=0; j<NUM_SCHOOLS; j++)
				m_pkSchoolButtons[j]->UncheckButton();

			char szLabel[25];
			sprintf(szLabel, "Spellbook School %i", i+1);
			m_pkApp->GetWnd("SpellBookLabel")->SetText(szLabel);

			m_pkSchoolButtons[i]->CheckButton();
		}
	}

	for(int y=0; y<SPELL_ROWS; y++)
		for(int x=0; x<SPELL_COLS; x++)
		{
			if(m_pkSpellButtons[y][x] == pkWndClicked)
			{
				SpellSlot* pkSlot = FindSlot(x,y);

				if(pkSlot)
				{
					int tex_id = FindSlot(x,y)->pkLabel->GetSkin()->m_iBkTexID;

					const char* szID = m_pkTexMan->GetFileName(tex_id);

					m_pkQuickBoard->AddSlot( QuickBoard::Spell, "lightingball" );

					m_pkAudioSys->StartSound( "/data/sound/turn_page.wav",
							m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);

					if(m_bAutoCloseWnd == true)
						ToogleOpen();
				}

				break;
			}
		}
}

void SpellDlg::ToogleOpen()
{
	if(m_pkDialog->IsVisible())
	{
		m_pkAudioSys->StartSound( "/data/sound/close_window.wav",
				m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);
		m_pkDialog->Hide();
		m_pkGui->SetFocus(m_pkApp->GetWnd("PanelBkWnd") );
	}
	else
	{
		m_pkAudioSys->StartSound( "/data/sound/open_window.wav",
				m_pkAudioSys->GetListnerPos(),m_pkAudioSys->GetListnerDir(),false);
		m_pkDialog->Show();

		m_pkGui->SetFocus(m_pkDialog);
	}
}

SpellDlg::SpellSlot* SpellDlg::FindSlot(int x, int y)
{
	vector<SpellSlot*>::iterator it = m_vkSpells.begin();

	for( ; it != m_vkSpells.end(); it++)
	{
		SpellSlot* pkCurrent = (*it);

		if(pkCurrent->x == x && pkCurrent->y == y)
		{
			return pkCurrent;
		}
	}

	return NULL;
}