// GLRender.h: interface for the GLRender class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _RENDER_GLGUIRENDER_H_
#define _RENDER_GLGUIRENDER_H_

#include "render_x.h"
#include "zguirenderer.h"

class TextureManager;

/// Handles rendering of ZGUI.
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
	void RenderText( char *stText, Rect kScreenRect, int iCursorPos, 
		int& rCharsPrinted, int& rRowsPrinted);
	bool SetDisplay(int w, int h);
	bool SetFont(ZGuiFont* pkFont);
	void GetScreenSize(int& cx, int& cy);

private:
	pair<int,int> GetWordLength(char* text, int offset);

	ZGuiSkin* m_pkSkin; // current skin
	ZGuiFont* m_pkFont; // current font
	//int m_iMaskTexture;
	int m_iScreenWidth, m_iScreenHeight;
	bool PrintRows(char* text, Rect rc, int iCursorPos, int& chars_printed, int& rows_printed);
	TextureManager* m_pkTextureManger;
};

#endif
