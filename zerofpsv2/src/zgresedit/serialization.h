// serialization.h: interface for the Serialization class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALIZATION_H__B6B5F30F_CC16_4E88_9C62_1FE103B767B7__INCLUDED_)
#define AFX_SERIALIZATION_H__B6B5F30F_CC16_4E88_9C62_1FE103B767B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
using namespace std;

#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"

class ZFBasicFS;
class ZGuiResourceManager;
class TextureManager;
class Scene;

class Serialization  
{
public:
	bool SaveGUI(char* szFileName, Scene* pkScene);
	bool LoadGUI(const char* szFileName, Scene* pkScene);
	void TempSave(Scene* pkScene);
	ZGuiWnd* TempLoad(Scene* pkScene); // returns the new mainwnd/focus wnd

	Serialization();
	virtual ~Serialization();

private:

	vector<ZGuiWnd*> m_kSavedWnds;

	bool WndHaveBeenSaved(ZGuiWnd* pkWnd);
	bool PrintWnd(ZGuiWnd* pkWnd);
	void PrintSkins(ZGuiWnd* pkWnd);

	string ChangeExtension(char* szName);
	ZFBasicFS* m_pkBasicFS;
	ZGuiResourceManager* m_pkGuiResMan;
	TextureManager* m_pkTexMan;
	ZGui* m_pkGui;
	
	//-- Temporära pekare som sätts i Load/Save
	FILE* m_pkFile;
	Scene* m_pkScene;
	//--

	vector< pair<ZGuiSkin, string> > m_kSkinTable;

	struct RANDOM_SORT: public binary_function<ZGuiWnd*, ZGuiWnd*, bool> { 
		bool operator() (ZGuiWnd* x, ZGuiWnd* y); // { return (rand()%10 > 5) ? true : false; }
	} RandomSort;

};

#endif // !defined(AFX_SERIALIZATION_H__B6B5F30F_CC16_4E88_9C62_1FE103B767B7__INCLUDED_)
