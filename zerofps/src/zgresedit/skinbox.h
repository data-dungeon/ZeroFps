#ifndef _SKINBOX_H
#define _SKINBOX_H

#include "dlgbox.h"
class ZFBasicFS;
class TextureManager;

class SkinBox : public DlgBox  
{
public:
	SkinBox(GuiBuilder* pkGuiBuilder, ZFBasicFS* pkBasicFS, TextureManager* pkTexMan);
	virtual ~SkinBox();

	void UpdatePreviewImage(char* strFileName);
	bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc);
	bool OnOpen(int x=-1, int y=-1);
	bool OnClose(bool bSave);

	void SetSkinBkColor(int r, int g, int b);
	void SetSkinBorderColor(int r, int g, int b);

	ZGuiSkin* m_pkPreviewSkin;

private:
	void FillTextureList();
	ZFBasicFS* m_pkBasicFS;
	TextureManager* m_pkTexMan;
};

#endif // #ifndef _SKINBOX_H
