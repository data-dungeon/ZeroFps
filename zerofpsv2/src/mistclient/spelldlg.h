// spelldlg.h: interface for the SpellDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPELLDLG_H__0C1929AC_DAE0_4FC3_8711_E568BCADA733__INCLUDED_)
#define AFX_SPELLDLG_H__0C1929AC_DAE0_4FC3_8711_E568BCADA733__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/texturemanager.h"

class SpellDlg  
{
public:
	SpellDlg(ZGuiApp* pkApp);
	~SpellDlg();

	void Update();
	void Init();
	void OnCommand(ZGuiWnd* pkWndClicked);
	void ToogleOpen();

private:
	ZGuiApp* m_pkApp;
	ZGui* m_pkGui;
	ZGuiResourceManager* m_pkResMan;
	TextureManager* m_pkTexMan;
	ZGuiWnd* m_pkDialog;
	ZGuiWnd* m_pkLevelPages[10];
};

#endif // !defined(AFX_SPELLDLG_H__0C1929AC_DAE0_4FC3_8711_E568BCADA733__INCLUDED_)
