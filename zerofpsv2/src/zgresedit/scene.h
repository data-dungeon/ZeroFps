// scene.h: interface for the Scene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENE_H__323A9176_B64C_42F4_95DD_E4F9941A17C4__INCLUDED_)
#define AFX_SCENE_H__323A9176_B64C_42F4_95DD_E4F9941A17C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"

class ZFBasicFS;
class TextureManager;


class Scene  
{
public:
	void Init(ZGuiApp* pkApp);
	Scene();
	virtual ~Scene();

	ZGuiWnd* m_pkPropertyWnd;
	ZGuiWnd* m_pkWorkSpace;
	ZGuiWnd* m_pkViewWindow;
	ZGuiWnd* m_pkSelectFileWnd;
	ZGuiWnd* m_pkDefProp;

	bool BuildFileTree(char* szTreeBoxName, char* szRootPath, char* szExtension);
	bool IsSceneWnd(ZGuiWnd* pkWnd);

	ZGuiApp* GetApp() { return m_pkApp; }

	bool RenameWnd(ZGuiWnd* pkWnd, const char* szName);
	const char* GetAlias(ZGuiWnd* pkWnd);
	ZGuiWnd* GetWnd(const char* szName); // letar först igenom alias sedan bland fönster registrerade i resman.
	void RemoveAlias(ZGuiWnd* pkWnd);
	void ScaleWndToTexSize(ZGuiWnd* pkWnd, char* szSelSkinType);
	ZGuiWnd* CloneWnd(ZGuiWnd* pkWnd, int xpos, int ypos);

private:
	void CreateUI();
	
	ZGuiApp* m_pkApp;
	ZGui* m_pkGui;
	TextureManager* m_pkTexMan;

	struct SORT_FILES : public binary_function<string, string, bool> {
		bool operator()(string x, string y) { 
			return (x.find(".") == string::npos);
		};
	} SortFiles;

	map<ZGuiWnd*, string> m_kNickNameWnds;

};

#endif // !defined(AFX_SCENE_H__323A9176_B64C_42F4_95DD_E4F9941A17C4__INCLUDED_)
