// zguiscrollbar.h: interface for the ZGuiScrollbar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZGUISCROLLBAR_H__88D34F0A_573C_468B_8090_219A2AD24315__INCLUDED_)
#define AFX_ZGUISCROLLBAR_H__88D34F0A_573C_468B_8090_219A2AD24315__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../engine_x.h"
#include "../../basic/rect.h"
#include "zguicontrol.h"

class ZGuiButton;

class ENGINE_API ZGuiScrollbar : public ZGuiControl
{
public:
	void SetScrollInfo(unsigned int min, unsigned int max, float page_size, unsigned int pos); // page size är i procent och skall vara ett värde mellan 0-1
	ZGuiScrollbar(Rect kRectangle, ZGuiWnd* pkParent, bool bVisible, int iID);
	virtual ~ZGuiScrollbar();

	void CreateInternalControls();
	bool Render( ZGuiRender* renderer );
	void SetThumbButtonSkins(ZGuiSkin* pkSkinNormal, ZGuiSkin* pkSkinHighLight);
	bool Notify(ZGuiWnd* pkWnd, int iCode);
	int GetPos() { return m_nPos; }
	
	int m_iScrollChange;

	ZGuiButton* GetButton() { return m_pkThumbButton; }
private:
	ZGuiButton* m_pkThumbButton;
	bool m_bHorzintal;
	unsigned int m_nMax, m_nMin, m_nPos;
	
};

#endif // !defined(AFX_ZGUISCROLLBAR_H__88D34F0A_573C_468B_8090_219A2AD24315__INCLUDED_)



