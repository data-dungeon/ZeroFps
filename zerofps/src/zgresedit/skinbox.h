#ifndef _SKINBOX_H
#define _SKINBOX_H

#include "dialogbox.h"
class ZFBasicFS;
class TextureManager;

const int MAX_SKIN_TYPES = 16;

class SkinBox : public DlgBox  
{
	enum TextureType
	{
		BACKGROUND,
		BORDERHORZ,
		BORDERVERT,
		BORDERCORNER,
		BACKGROUNDALPHA,
		BORDERHORZALPHA,
		BORDERVERTALPHA,
		BORDERCORNERALPHA,
		NONE
	};	
	
	bool OnClose(bool bSave);
	void SetSkinBkColor(int r, int g, int b);
	void SetSkinBorderColor(int r, int g, int b);
	void SetSkinBorderSize(int iSize);

	bool GetSelTexInfo(ZGuiSkin* pkSkin, long* &pType, char* szFileName);
	void UpdatePreviewImage(char* szFileName);
	void SetupDefaultSkin();
	void FillTextureList();
	ZFBasicFS* m_pkBasicFS;
	TextureManager* m_pkTexMan;
	vector<ZGuiSkin> m_vkPreviewSkin;
	unsigned short m_usSkinTypeIndex;
	unsigned short m_unRealBorderSz;
	TextureType m_eSelTextureType;

public:
	SkinBox(GuiBuilder* pkGuiBuilder, ZGuiWndProc oMainWndProc, 
		ZFBasicFS* pkBasicFS, TextureManager* pkTexMan);
	virtual ~SkinBox();

	bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc);
	bool OnOpen(int x=-1, int y=-1);
	
	bool DlgProc(ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams );

};

#endif // #ifndef _SKINBOX_H




