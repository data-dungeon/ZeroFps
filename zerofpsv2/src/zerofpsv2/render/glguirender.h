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

/**	\brief	Handles rendering of ZGUI.
		\ingroup Render
*/
class RENDER_API GLGuiRender : public ZGuiRender  
{
public:
	void EnableClipper(bool bEnable);
	GLGuiRender();
	virtual ~GLGuiRender();

	bool StartUp();
	bool ShutDown();
	bool IsValid();

	bool EndRender();
	bool StartRender();
	bool RenderQuad(Rect kScreenRect);
	bool RenderPoints(vector<tRGBPoint>& akLines);
	bool RenderLines(vector<Point>& akLines, 
		unsigned char r,unsigned char g,unsigned char b, float width=1.0f);
	bool RenderRects(vector<tRGBRect>& akLines);
	bool RenderBorder(Rect kScreenRect);
	bool SetSkin(ZGuiSkin* pkSkin);
	bool SetDisplay(int w, int h);
	void GetScreenSize(int& cx, int& cy);
	void GetScaleMode(GUIScaleMode& eGUIScaleMode) { eGUIScaleMode = m_eGUIScaleMode; } 
	void SetScaleMode(GUIScaleMode eGUIScaleMode);
	void SetClipperArea(Rect rc);

	bool SetFont(ZGuiFont* pkFont);
	void RenderText( char *stText, Rect kScreenRect, 
		int iCursorPos, int iRenderDistFromTop, bool bMultiLine,
		int& rCharsPrinted, int& rRowsPrinted, float afTextcolor[3]);
	
private:

	void PrintWord(int x, int y, char* szWord, int offset, int length);
	pair<int,int> GetWordLength(char* text, int offset, int max_width);
	bool PrintRows(char* text, Rect rc, int iCursorPos, int iRenderDistFromTop, 
		int& chars_printed, int& rows_printed);
	bool PrintRow(char* text, Rect rc, int iCursorPos, 
		int iRenderDistFromLeft, int& chars_printed);

	void DoTextTag(); 
	void RotateVertexCoords90deg(float *pfTUs, float *pfTVs, unsigned char ucRotLaps);

	ZGuiSkin* m_pkSkin; // current skin
	ZGuiFont* m_pkFont; // current font
	
	TextureManager* m_pkTextureManger;
	
	int m_iCursorPos;

	Rect m_rcTextBox;
	Rect m_rcClipperArea;
	bool m_bClipperEnabled;

	int m_iScreenWidth;
	int m_iScreenHeight;

	float m_afTextColor[3]; // [0-1] RGB

	bool m_bSearchForSytax;
	string m_strSyntax;

	GUIScaleMode m_eGUIScaleMode;
};

#endif
