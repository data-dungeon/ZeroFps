#ifndef _CONTROLBOX_H
#define _CONTROLBOX_H

#include "dialogbox.h"
class Serialization;
class ZFIni;

class ControlBox : public DlgBox  
{
public:
	class CreateWnd
	{
	public:
		CreateWnd(ZGuiWnd* pkWnd) {m_pkWnd=pkWnd; m_bEnabled=true;}
		~CreateWnd(){}

		ZGuiWnd* m_pkWnd;
		bool m_bEnabled;
	};

	vector<CreateWnd> m_pkCreatedWindows;

private:
	void CreateCopy();
	typedef bool (*ZGuiWndProc) (ZGuiWnd* a, unsigned int b, int c, void* d) ;
	typedef pair<pair<string,string>, ZGuiSkin*> tSkinInf; //window_name, wnd_desc, skin

	bool OnClose(bool bSave);
	void SelectSkins(vector<tSkinInf>& kSkinInfoList, ZGuiWnd* pkWnd);
	bool CreateNewType(CtrlType eType, ZGuiWndProc oWndProc);
	void PrintSkins(Serialization *pkFile, TextureManager* pkTexMan);
	void PrintUniqueWndProperites(Serialization* pkFile, ZGuiWnd* pkWnd);
	
public:
	void ClearAll();
	bool LoadGUI(ZFIni* pkINI, TextureManager* pkTexMan);
	void PrintWindowRC(Serialization* pkFile, TextureManager* pkTexMan);
	void UnregisterNewType(ZGuiWnd* pkWnd);
	void PrintWindowIDs(Serialization* pkFile);
	ControlBox(GuiBuilder* pkGuiBuilder, ZGuiWndProc oMainWndProc);
	virtual ~ControlBox();

	bool OnOpen(int x=-1, int y=-1);
	bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc);
	bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams );
	
};

#endif // #ifndef _CONTROLBOX_H
