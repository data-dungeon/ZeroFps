#ifndef _GUI_ACTIONMENU_H_
#define _GUI_ACTIONMENU_H_

#include "../zerofpsv2/basic/vector2.h"
#include "../zerofpsv2/basic/zguiskin.h"

class TextureManager;
class ZGuiWnd;
class ZGuiLabel;
class Entity;

#include <vector>
using namespace std;

class ActionMenu
{
public:
	ActionMenu();
	~ActionMenu();

	bool IsOpen();

	void Open();
	void Close();
	void OnCommand(string strController);
	void OnMouseMove(bool bLeftButtonPressed, int x, int y);
	void SetEntity(int iEntityID);

private:
	TextureManager* m_pkTexMan;

	int m_kWidth, m_kHeight;
	ZGuiWnd* m_pkMainWnd;

	int m_iEntityID;

	Vector2 RotateXY(float angle, Vector2 size);
	ZGuiSkin* GetFreeIconSkin();
	void ResetIconSkins();

	vector<string>	m_kActions;
	
	vector< pair<ZGuiSkin*,bool> > m_vkIconSkins;
	ZGuiLabel* m_pkIconSelection;
	bool m_bGuiCaptureBeforOpen;

	const int ICON_WIDTH;
	const int ICON_HEIGHT;
};

#endif
