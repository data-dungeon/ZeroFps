// itemboxbox.h: interface for the itembox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMBOX_H__CBE34F86_D2E8_4A84_8078_6E89373F86A7__INCLUDED_)
#define AFX_ITEMBOX_H__CBE34F86_D2E8_4A84_8078_6E89373F86A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dialogbox.h"

class Container;
class TextureManager;
class Object;
struct GuiData;

class ItemBox : public DlgBox 
{
public:
	
	void SetContainer(Container* pkContainer);
	void Update();
	ItemBox(ZGui* pkGui, ZGuiWndProc oMainWndProc, TextureManager* pkTexMan,
		int iCols=8, int iRows=8, int iSlotSize=48, int iTopX=5, int iTopY=5);
	virtual ~ItemBox();

	bool DlgProc( ZGuiWnd* pkWnd,unsigned int uiMessage,
		int iNumberOfParams,void *pkParams );

	bool Create(int x, int y, int w, int h, ZGuiWndProc pkWndProc);
	bool OnClose(bool bSave);
	bool OnOpen(int x, int y);

private:
	void PaintStaticSlots(int container_size_x, int container_size_y);

	typedef pair<int,int > slot_pos;

	
	bool ButtonSlotExist(int grid_x, int grid_y);
	void RemoveSlot(int grid_x, int grid_y);
	void AddSlot(GuiData* pkData);
	char* GetSlotName(int id);

	/// number of rows and colls
	const int m_ciRows, m_ciCols;

	/// same pos as the top (x,y) of the first static 
	/// grid picture in the resource file.
	const int m_ciTopX, m_ciTopY;
	
	/// size of the slots in pixels
	const int m_ciSlotSize;

	TextureManager* m_pkTexMan;
	Container* m_pkContainer;

	typedef pair<ZGuiButton*, slot_pos> slot;

	vector< slot > m_akSlots;

	slot* m_pkMoveItem;

	slot_pos GetSlot(int x, int y);

};

#endif // !defined(AFX_ITEMBOX_H__CBE34F86_D2E8_4A84_8078_6E89373F86A7__INCLUDED_)
