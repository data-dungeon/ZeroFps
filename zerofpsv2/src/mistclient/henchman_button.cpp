#include "henchman_button.h"

int HenchmanButton::s_iHenchemanAlphaTex = 0;

HenchmanButton::HenchmanButton(ZGuiApp* pkApp, int iIconID, int iPos)
{
	char szName[25];
	sprintf(szName, "HenchManBn0%i", iPos);
	pkApp->CreateWnd(Checkbox, szName, "MainWnd", "", pkApp->GetWidth()-51, 4+iPos*56, 48, 48, 0);

	m_pkClickIcon = static_cast<ZGuiCheckbox*>(pkApp->GetWnd(szName));

	ZGuiSkin* pkSkin;
	pkSkin = new ZGuiSkin(255,255,255, 0,0,0, 0);
	pkSkin->m_iBkTexID = iIconID;
	pkSkin->m_iBkTexAlphaID = s_iHenchemanAlphaTex;
	m_pkClickIcon->SetButtonUncheckedSkin(pkSkin);

	pkSkin = new ZGuiSkin(255,255,255, 128,255,0, 2);
	pkSkin->m_iBkTexID = iIconID;
	pkSkin->m_iBkTexAlphaID = s_iHenchemanAlphaTex;
	m_pkClickIcon->SetButtonCheckedSkin(pkSkin);

	sprintf(szName, "HenchManLifeBar0%i", iPos);
	pkApp->CreateWnd(Label, szName, "MainWnd", "", pkApp->GetWidth()-51, 52+iPos*56, 48, 4, 0);
	pkApp->GetWnd(szName)->SetSkin(new ZGuiSkin(0,0,255, 0,0,0, 0));
	m_pkLifeBar = (ZGuiLabel*) pkApp->GetWnd(szName);

	sprintf(szName, "HenchManLifeBarBk0%i", iPos);
	pkApp->CreateWnd(Label, szName, "MainWnd", "", pkApp->GetWidth()-51, 52+iPos*56, 48, 4, 0);
	pkApp->GetWnd(szName)->SetSkin(new ZGuiSkin(255,0,0, 0,0,0, 0));

	m_iListPos = iPos;
}

string HenchmanButton::GetName()
{
	return string(m_pkClickIcon->GetName());
}

void HenchmanButton::Check(bool bCheck)
{
	if(bCheck)
		m_pkClickIcon->CheckButton();
	else
		m_pkClickIcon->UncheckButton();	
}