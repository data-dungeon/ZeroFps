// GuiRender.h: interface for the GuiRender class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIRENDER_H__689588FE_E416_482A_9654_617B1E478E48__INCLUDED_)
#define AFX_GUIRENDER_H__689588FE_E416_482A_9654_617B1E478E48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "render_x.h"
#include "../basic/rect.h"
#include "../basic/basic.pkg"

class ZGuiSkin;

class RENDER_API ZGuiRender : public ZFObject 
{
public:
	ZGuiRender();
	virtual ~ZGuiRender();

	virtual bool StartRender() = 0;
	virtual bool EndRender() = 0;
	virtual bool RenderQuad(Rect kScreenRect/*, bool bMask=false*/) = 0;
	virtual bool RenderBorder(Rect kScreenRect/*, bool bMask=false*/) = 0;
	virtual bool SetSkin(ZGuiSkin* pkSkin) = 0;
	virtual bool SetFont(ZGuiFont* pkFont) = 0;
	virtual bool RenderText( char *stText, Rect kScreenRect, int iCursorPos, bool bCenterTextVertically=true) = 0;
	//virtual bool SetMaskTexture(int iID) = 0;
	virtual bool SetDisplay(int w, int h) = 0;
};

#endif // !defined(AFX_GUIRENDER_H__689588FE_E416_482A_9654_617B1E478E48__INCLUDED_)

//Kan man säga till OpenGL att den skall rendera ens punkter i 2D-fönster koordinater?





