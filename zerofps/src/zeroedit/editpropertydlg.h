// editpropertydlg.h: interface for the EditPropertyDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITPROPERTYDLG_H__F0F656FE_74C5_4E19_93D9_61437FDA743A__INCLUDED_)
#define AFX_EDITPROPERTYDLG_H__F0F656FE_74C5_4E19_93D9_61437FDA743A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gui.h"
//#include <string>
//using namespace std;

class PropertyFactory;
class Property;

class EditPropertyDlg  
{
public:

	typedef bool (*callback)(ZGuiWnd* pkWnd, unsigned int uiMessage, int iNumParams, void *pParams);

	EditPropertyDlg(Gui* pkGui, PropertyFactory* pk, ObjectManager* om, 
		Object* pkCurrentChild, callback cb);
	virtual ~EditPropertyDlg();

	bool DlgProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );

private:
	void RemoveProperty();
	ZGuiWnd* Create(int x, int y, int w, int h);

	ZGuiWnd* CreateAddPropertyDlg(int x, int y, int w, int h);
	bool OnCloseAddProperty(bool bSave);
	void OnOpenAddProperty();
	bool OnCloseEditProperty(bool bSave);
	void OnOpenEditProperty();

	Gui* m_pkGui;
	ZGui* m_pkZGui;
	callback m_oGuiCallback;
	ZGuiWnd* m_pkWindow;

	Object* m_pkCurrentChild;
	ObjectManager* m_pkObjectManager;
	PropertyFactory* m_pkPropFactory;
	Property* m_pkSelProperty;
	string m_kSelPropValue;

	bool m_bAdd;
};

#endif // !defined(AFX_EDITPROPERTYDLG_H__F0F656FE_74C5_4E19_93D9_61437FDA743A__INCLUDED_)
