// statsdlg.h: interface for the StatsDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATSDLG_H__74161501_9DE3_4C43_9347_D9588967C920__INCLUDED_)
#define AFX_STATSDLG_H__74161501_9DE3_4C43_9347_D9588967C920__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/texturemanager.h"
#include "../zerofpsv2/engine_systems/audio/zfaudiosystem.h"
#include "../mcommon/rulesystem/character/characterstats.h"
#include "../mcommon/p_charstats.h"
#include "../mcommon/rulesystem/character/characterstats.h"

class StatsDlg  
{
public:
	StatsDlg(ZGuiApp* pkApp);
	~StatsDlg();

	void Update();
	void Init();
	void OnCommand(ZGuiWnd* pkWndClicked);
	void ToogleOpen();
	void SetCharacterProperty(CharacterProperty* pkCharProp);

private:
	ZGuiApp* m_pkApp;
	ZGui* m_pkGui;
	ZGuiResourceManager* m_pkResMan;
	TextureManager* m_pkTexMan;
	ZFAudioSystem* m_pkAudioSys;
	ZGuiWnd* m_pkDialog;
	CharacterProperty* m_pkCharProperty;

};

#endif // !defined(AFX_STATSDLG_H__74161501_9DE3_4C43_9347_D9588967C920__INCLUDED_)
