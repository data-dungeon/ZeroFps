#ifndef _GUI_INVENTORY_H_
#define _GUI_INVENTORY_H_

#include "../zerofpsv2/basic/vector2.h"
#include "../zerofpsv2/basic/zguiskin.h"
#include "../mcommon/mlcontainer.h"

class TextureManager;
class ZGuiWnd;
class ZGuiLabel;
class Entity;

#include <vector>
#include <string>
using namespace std;

class InventoryDlg
{
public:
	InventoryDlg();
	~InventoryDlg();

	void Update(vector<MLContainerInfo>& vkContainerList);

	void Open();
	void Close();
	void OnCommand(string strController);
	void OnMouseMove(bool bLeftButtonPressed, int x, int y);
	bool IsVisible() { if(m_pkMainWnd == NULL) return false; return m_pkMainWnd && m_pkMainWnd->IsVisible(); }

private:
	TextureManager* m_pkTexMan;

	ZGuiWnd* m_pkMainWnd;
	
	bool m_bGuiCaptureBeforOpen;

	const int ICON_WIDTH;
	const int ICON_HEIGHT;

	struct ITEM_SLOT
	{
		string strWndName;
	};

	vector<ITEM_SLOT> m_vkItemList;
};

#endif
