// GuiRender.h: interface for the GuiRender class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _RENDER_ZGUIRENDER_H_
#define _RENDER_ZGUIRENDER_H_

#include "render_x.h"
#include "../basic/rect.h"
#include "../basic/zguifont.h"
#include "../basic/image.h"
#include "../basic/zfsystem.h"

class ZGuiSkin;

const int ZG_CENTER_TEXT_HORZ = -2;

typedef pair<Point,color_rgb> tRGBPoint;
typedef pair<Rect,color_rgb> tRGBRect;

enum GUIScaleMode
{
	GUIScaleProjMatBeforeRendering,
	GUIScaleManually,
};

//struct MULTI_LINE_TEXT_INFO
//{
//	int m_iRenderDistFromTop; // [in]
//	unsigned short** m_ppRowOffsets; // [out]
//	unsigned short* m_pNumRows;
//};

/**	\brief	Render for GUI.
		\ingroup Render
*/
class RENDER_API ZGuiRender : public ZFSubSystem 
{
public:
	ZGuiRender();
	virtual ~ZGuiRender();
	virtual bool StartRender() = 0;
	virtual bool EndRender() = 0;
	virtual void EnableClipper(bool bEnable) = 0;

	virtual void SetClipperArea(Rect rc) = 0;
	virtual bool RenderRects(vector<tRGBRect>& akLines) = 0;
	virtual bool RenderPoints(vector<tRGBPoint>& akLines) = 0;
	virtual bool RenderQuad(Rect kScreenRect) = 0;
	virtual bool RenderBorder(Rect kScreenRect) = 0;
	virtual bool SetSkin(ZGuiSkin* pkSkin) = 0;
	virtual bool SetFont(ZGuiFont* pkFont) = 0;
	//virtual void RenderText( char *stText, Rect kScreenRect, int iCursorPos, int iRenderDistFromTop,
	//	bool bMultiLine, int& rCharsPrinted, int& rRowsPrinted, float afTextcolor[3]) = 0;

	virtual void RenderText( char *stText, Rect kScreenRect, int iCursorPos, 
		float afTextcolor[3], int iRenderDistFromLeft=0) = 0;
	
	virtual int StartDrawText(bool bCreateDisplayList=false) = 0;
	virtual void DrawString(const char* text, const int length, int x, int y, 
		const float color[3], const ZGuiFont* font) = 0;
	virtual void DrawStringDisplayList(int iDisplayListID) = 0;
	virtual void EndDrawText() = 0;
	virtual void DeleteStringDisplayList(int iDisplayListID) = 0;

	virtual pair<int,int> GetWordLength(char *text, int offset, int max_width) = 0;
	virtual bool SetDisplay(int w, int h) = 0;
	virtual void GetScreenSize(int& x, int& y) = 0;
	virtual bool RenderLines(vector<Point>& akLines,
		unsigned char r,unsigned char g,unsigned char b,
		float width=1.0f) = 0;
	virtual void GetScaleMode(GUIScaleMode& eGUIScaleMode) = 0; 
	virtual void SetScaleMode(GUIScaleMode eGUIScaleMode) = 0;
};

#endif 





