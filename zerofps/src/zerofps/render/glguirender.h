// GLRender.h: interface for the GLRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLRENDER_H__943EECE7_8487_4E17_B41E_3B4D94D1683F__INCLUDED_)
#define AFX_GLRENDER_H__943EECE7_8487_4E17_B41E_3B4D94D1683F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "render_x.h"
#include "zguirenderer.h"

class TextureManager;

class RENDER_API GLGuiRender : public ZGuiRender  
{
public:
	GLGuiRender();
	virtual ~GLGuiRender();

	bool EndRender();
	bool StartRender();
	bool RenderQuad(Rect kScreenRect/*, bool bMask=false*/);
	bool RenderBorder(Rect kScreenRect/*, bool bMask=false*/);
	bool SetSkin(ZGuiSkin* pkSkin);
	//bool SetMaskTexture(int iID) { m_iMaskTexture = iID; return true; }
	bool RenderText( char *stText, Rect kScreenRect, int iCursorPos, bool bCenterTextVertically=true);
	bool SetDisplay(int w, int h);
	bool SetFont(ZGuiFont* pkFont);

private:

	ZGuiSkin* m_pkSkin; // current skin
	ZGuiFont* m_pkFont; // current font
	//int m_iMaskTexture;
	int m_iScreenWidth, m_iScreenHeight;
	bool PrintRows(char* text, Rect rc, int iCursorPos);
	TextureManager* m_pkTextureManger;


};

#endif // !defined(AFX_GLRENDER_H__943EECE7_8487_4E17_B41E_3B4D94D1683F__INCLUDED_)
