#ifndef _GUI_EQUIPMENTDLG_H_
#define _GUI_EQUIPMENTDLG_H_

#include "../zerofpsv2/basic/vector2.h"
#include "../zerofpsv2/basic/zguiskin.h"
#include "../mcommon/p_container.h"

class TextureManager;
class ZGuiWnd;
class ZGuiLabel;
class Entity;

#include <vector>
#include <string>
using namespace std;

class EquipmentDlg
{
public:
	EquipmentDlg();
	~EquipmentDlg();

	void Open();
	void Close();
	void OnCommand(string strController);
	void OnMouseMove(bool bLeftButtonPressed, int x, int y);
	bool IsVisible() 
	{ 
		if(m_pkEquipmentWnd == NULL) 
			return false; 
		return m_pkEquipmentWnd->IsVisible(); 
	}
		
private:
	
	TextureManager* m_pkTexMan;
	
	ZGuiWnd* m_pkEquipmentWnd;	
};

#endif // _GUI_EQUIPMENTDLG_H_
