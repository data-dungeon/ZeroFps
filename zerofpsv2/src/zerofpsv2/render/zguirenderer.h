// GuiRender.h: interface for the GuiRender class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _RENDER_ZGUIRENDER_H_
#define _RENDER_ZGUIRENDER_H_

#include "render_x.h"
#include "../basic/rect.h"
#include "../basic/zguifont.h"
#include "../basic/image.h"
#include "../basic/zfobjectmanger.h"

class ZGuiSkin;

const int ZG_CENTER_TEXT_HORZ = -2;

typedef pair<Point,color_rgb> tRGBPoint;
typedef pair<Rect,color_rgb> tRGBRect;

class RENDER_API ZGuiRender : public ZFSubSystem 
{
public:
	ZGuiRender();
	virtual ~ZGuiRender();

	virtual bool StartRender() = 0;
	virtual bool EndRender() = 0;
	
	virtual bool RenderRects(vector<tRGBRect>& akLines) = 0;
	virtual bool RenderPoints(vector<tRGBPoint>& akLines) = 0;
	virtual bool RenderQuad(Rect kScreenRect) = 0;
	virtual bool RenderBorder(Rect kScreenRect) = 0;
	virtual bool SetSkin(ZGuiSkin* pkSkin) = 0;
	virtual bool SetFont(ZGuiFont* pkFont) = 0;
	virtual void RenderText( char *stText, Rect kScreenRect, int iCursorPos, int iRenderDistFromTop,
		bool bMultiLine, int& rCharsPrinted, int& rRowsPrinted) = 0;
	virtual bool SetDisplay(int w, int h) = 0;
	virtual void GetScreenSize(int& x, int& y) = 0;
	virtual bool RenderLines(vector<Point>& akLines,
		unsigned char r,unsigned char g,unsigned char b,
		float width=1.0f) = 0;

	bool StartUp();
	bool ShutDown();
	bool IsValid();
};

#endif 

//Kan man säga till OpenGL att den skall rendera ens punkter i 2D-fönster koordinater?





