// inventorydlg.h: interface for the InventoryDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_)
#define AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofpsv2/basic/zfsubsystem.h"
#include "../zerofpsv2/gui/zguilabel.h"
#include "../zerofpsv2/gui/zgui.h"
#include "../zerofpsv2/gui/zguiresourcemanager.h"
#include "../zerofpsv2/render/texturemanager.h"

#include <vector>
using namespace std;

class InventoryDlg// : public ZFSubSystem  
{
public:
	void OnScroll(int iID, int iPos);
	void OnCommand(int iID);
	void OnMouseMove(int x, int y, bool bMouseDown);
	void RemoveSlot(Point sqr, bool bUnregister, bool bDragItem=false);

	class Slot
	{
	public:
		Slot() {};
		~Slot() {};

		ZGuiLabel* pkLabel;
		Point sqr;

		char szPic[2][50];
	};

	Slot* FindSlot(Point sq);
	void OnClick(int x, int y, bool bMouseDown);
	void AddSlot(char *szPic, char *szPicA, Point sqr, bool bDragItem=false);
	InventoryDlg();
	~InventoryDlg();

	static int GetID(Point sqr);

private:
	void ScrollItems(int iPos);
	
	void RegisterSlot(Slot slot);
	Point MousePosToSqr(int x, int y);
	vector<Slot> m_kSlots;
	vector<Slot> m_kDragSlots;

	typedef vector<Slot>::iterator itSlot;

	ZGui* m_pkGui;
	ZGuiResourceManager* m_pkResMan;
	TextureManager* m_pkTexMan;

	static int s_iSlotZCounter;

	Point m_kClickOffset;



};

#endif // !defined(AFX_INVENTORYDLG_H__E2EC300C_10CF_4D5C_B948_2FD7FCAB2686__INCLUDED_)
