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
	GLGuiRender(/*int w=0, int h=0, TextureManager* pkTextureManger=NULL*/);
	virtual ~GLGuiRender();

	bool EndRender();
	bool StartRender();
	bool RenderQuad(Rect kScreenRect, bool bMask=false);
	bool RenderBorder(Rect kScreenRect, bool bMask=false);
	bool SetSkin(ZGuiSkin* pkSkin);
	bool SetMaskTexture(int iID) { m_iMaskTexture = iID; return true; }
	bool RenderText( char *stText, Rect kScreenRect, int iFontSize, int iCursorPos, 
					 bool bMask=false, bool bCenterTextVertically=true);
	bool SetDisplay(int w, int h);

private:
	GLvoid glPrint(GLint x, GLint y, char *string, int set, int bind_texture_id);	// Where The Printing Happens;
	void BuildFont();
	void PrintRows(char* text, unsigned int iXPos, unsigned int iYPos, 
					unsigned int nFontSize, unsigned int iCharsOneRow, 
					unsigned int nMaxRows, int iCursorPos);
	ZGuiSkin* m_pkSkin; // current skin
	int m_iMaskTexture;
	int m_iScreenWidth, m_iScreenHeight;
	void PrintChar(char cChar, int xoffset, int yoffset, int iFontSize);
	TextureManager* m_pkTextureManger;
	GLuint m_iFontDisplaylistID;

};

#endif // !defined(AFX_GLRENDER_H__943EECE7_8487_4E17_B41E_3B4D94D1683F__INCLUDED_)
