// GLRender.h: interface for the GLRender class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _RENDER_GLGUIRENDER_H_
#define _RENDER_GLGUIRENDER_H_

#include "render_x.h"
#include "zguirenderer.h"
#include "../engine/fh.h"
//#include "../engine/zfresource.h"

class TextureManager;

/// Handles rendering of ZGUI.
class RENDER_API GLGuiRender : public ZGuiRender  
{
public:
	GLGuiRender();
	virtual ~GLGuiRender();

	bool EndRender();
	bool StartRender();
	bool RenderQuad(Rect kScreenRect);
	bool RenderPoints(vector<tRGBPoint>& akLines);
	bool RenderLines(vector<Point>& akLines, 
		unsigned char r,unsigned char g,unsigned char b, float width=1.0f);
	bool RenderRects(vector<tRGBRect>& akLines);
	bool RenderBorder(Rect kScreenRect);
	bool SetSkin(ZGuiSkin* pkSkin);
	void RenderText( char *stText, Rect kScreenRect, 
		int iCursorPos, int iRenderDistFromTop, bool bMultiLine,
		int& rCharsPrinted, int& rRowsPrinted);
	bool SetDisplay(int w, int h);
	bool SetFont(ZGuiFont* pkFont);
	void GetScreenSize(int& cx, int& cy);

private:
	void PrintWord(int x, int y, char* szWord, int offset, int length);
	pair<int,int> GetWordLength(char* text, int offset, int max_width);

	ZGuiSkin* m_pkSkin; // current skin
	ZGuiFont* m_pkFont; // current font
	int m_iScreenWidth, m_iScreenHeight;
	bool PrintRows(char* text, Rect rc, int iCursorPos, int iRenderDistFromTop, 
		int& chars_printed, int& rows_printed);
	bool PrintRow(char* text, Rect rc, int iCursorPos, 
		int iRenderDistFromLeft, int& chars_printed);
	TextureManager* m_pkTextureManger;
	int m_iCursorPos;
};

#endif
