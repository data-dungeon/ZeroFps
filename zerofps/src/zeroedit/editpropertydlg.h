// editpropertydlg.h: interface for the EditPropertyDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITPROPERTYDLG_H__F0F656FE_74C5_4E19_93D9_61437FDA743A__INCLUDED_)
#define AFX_EDITPROPERTYDLG_H__F0F656FE_74C5_4E19_93D9_61437FDA743A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gui.h"
#include "dialogbox.h"
//#include <string>
//using namespace std;

class PropertyFactory;
class Property;
class Input;

class EditPropertyDlg : public DlgBox
{
public:
	bool IsOpen();

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);

	EditPropertyDlg(Gui* pkGui, PropertyFactory* pk, ObjectManager* om, 
		Object* pkCurrentChild, Input* pkInput, callback cb);
	virtual ~EditPropertyDlg();

	bool DlgProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );

	bool OnCloseEditProperty(bool bSave);
	bool OnCloseAddProperty(bool bSave);

	Object* m_pkCurrentChild;

private:
	void ClearAllFileds();
	void UpdateStats(int iComboBoxID);
	void RemoveProperty();
	bool Create(int x, int y, char* szResourceFile, char* szDlgName);

	ZGuiWnd* CreateAddPropertyDlg(int x, int y, int w, int h);
	
	void OnOpenAddProperty();
	void OnOpenEditProperty();

	Gui* m_pkGui;
	ZGui* m_pkZGui;
	callback m_oGuiCallback;
//	ZGuiWnd* m_pkWindow;	

	ObjectManager* m_pkObjectManager;
	PropertyFactory* m_pkPropFactory;
	Property* m_pkSelProperty;
	
	string m_szSelProperty;
	string m_szSelPropValue;

	bool m_bAdd;

protected:

	bool OnOpen(int x=-1, int y=-1);
	bool OnClose(bool bSave);
};

#endif // !defined(AFX_EDITPROPERTYDLG_H__F0F656FE_74C5_4E19_93D9_61437FDA743A__INCLUDED_)
