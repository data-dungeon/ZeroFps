// statsdlg.cpp: implementation of the StatsDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "statsdlg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StatsDlg::StatsDlg(ZGuiApp* pkApp)
{
	m_pkGui = static_cast<ZGui*>(g_ZFObjSys.GetObjectPtr("Gui"));
	m_pkResMan = static_cast<ZGuiResourceManager*>(g_ZFObjSys.GetObjectPtr("ZGuiResourceManager"));
	m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));
	m_pkAudioSys = static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));
	m_pkApp = pkApp;
	m_pkCharProperty = NULL;
	Init();
}

StatsDlg::~StatsDlg()
{

}

void StatsDlg::Update()
{
	if(m_pkDialog->IsVisible() == false)
	{
		return;
	} 

	if(m_pkCharProperty)
	{
		CharacterStats* pkCharStats = m_pkCharProperty->GetCharStats();
		
		map<string,string> kDataList;
		pkCharStats->GetData(kDataList); 

		map<string,string>::iterator itData = kDataList.begin();

		for( ; itData != kDataList.end(); itData++)
		{
			printf("itData %s = %s\n", itData->first, itData->second);
		}

	}
}

void StatsDlg::Init()
{
	int screen_w = m_pkApp->GetWidth(); 
	int screen_h = m_pkApp->GetHeight();
}

void StatsDlg::OnCommand(ZGuiWnd* pkWndClicked)
{

}

void StatsDlg::ToogleOpen()
{
	if(m_pkCharProperty)
		m_pkCharProperty->RequestUpdateFromServer("data");
}

void StatsDlg::SetCharacterProperty(CharacterProperty* pkCharProp)
{
	m_pkCharProperty = pkCharProp;
}