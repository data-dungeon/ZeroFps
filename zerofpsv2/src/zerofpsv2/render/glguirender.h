// GLRender.h: interface for the GLRender class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _RENDER_GLGUIRENDER_H_
#define _RENDER_GLGUIRENDER_H_

#include "render_x.h"
#include "zguirenderer.h"
#include "../engine/fh.h"
#include "zshadersystem.h"
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
	bool StartRender(bool bClear);
	bool RenderQuad(Rect kScreenRect);
	bool RenderPoints(vector<tRGBPoint>& akLines);
	bool RenderLines(vector<Point>& akLines, 
		unsigned char r,unsigned char g,unsigned char b, float width=1.0f);
	bool RenderRects(vector<tRGBRect>& akLines);
	bool RenderBorder(Rect kScreenRect);
	bool SetSkin(ZGuiSkin* pkSkin);
	bool SetDisplay(int w, int h);
	void GetScreenSize(int& cx, int& cy);
	void SetClipperArea(Rect rc);

	bool SetFont(ZGuiFont* pkFont);
	//void RenderText( char *stText, Rect kScreenRect, 
	//	int iCursorPos, int iRenderDistFromTop, bool bMultiLine,
	//	int& rCharsPrinted, int& rRowsPrinted, float afTextcolor[3]);

	void RenderText( char *stText, Rect kScreenRect, int iCursorPos, 
		float afTextcolor[3], int iRenderDistFromLeft);

	int StartDrawText(bool bCreateDisplayList=false);
	void DrawString(const char* text, const int length, int x, int y, 
		const float color[3], const ZGuiFont* font);
	void DrawStringDisplayList(int iDisplayListID);
	void EndDrawText();
	void DeleteStringDisplayList(int iDisplayListID);
	
private:

	void UpdateAnimation();

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
	
	TextureManager*	m_pkTextureManger;
	ZShaderSystem*		m_pkShader;
	
	int m_iCursorPos;
	unsigned short** m_ppRowOffsets;

	Rect m_rcTextBox;
	Rect m_rcClipperArea;
	bool m_bClipperEnabled;

	int m_iScreenWidth;
	int m_iScreenHeight;

	float m_afTextColor[3]; // [0-1] RGB

	bool m_bSearchForSytax;
	string m_strSyntax;

	bool m_bDrawList;
};

#endif
