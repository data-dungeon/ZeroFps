// editpropertydlg.cpp: implementation of the EditPropertyDlg class.
//
//////////////////////////////////////////////////////////////////////

 
#include "editpropertydlg.h"
#include "../zerofps/engine/propertyfactory.h"
#include "../zerofps/engine/objectmanager.h"
#include "resource_id.h"
#include "../../data/gui_resource_files/zgresource_id.h"

///////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////

// Window proc wrappers
///////////////////////////////////////////////////////////////////////////////

extern ZeroEdit Editor;

static bool PROPERTYPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, 
						 int iNumberOfParams, void *pkParams ) 
{
	DlgBox* pkDlgBox = Editor.m_pkGui->GetDlg("PropertyDlg");
	if(pkDlgBox)
		return pkDlgBox->DlgProc(pkWindow, uiMessage, iNumberOfParams, pkParams);
	else
		return Editor.m_pkGui->WndProc(pkWindow, uiMessage, iNumberOfParams, pkParams);

}

///////////////////////////////////////////////////////////////////////////////

EditPropertyDlg::EditPropertyDlg(Gui* pkGui, PropertyFactory* pf, 
								 ObjectManager* om, Object* pkCurrentChild, 
								 Input* pkInput,callback cb) 
								 : DlgBox(pkGui->GetGUI(), pkInput, cb)
{
	m_szSelProperty.erase();
	m_szSelPropValue.erase();

	m_pkGui = pkGui;
	m_pkZGui = m_pkGui->GetGUI(); 
	m_pkCurrentChild = pkCurrentChild;
	m_pkPropFactory = pf;
	m_pkObjectManager = om;
	m_oGuiCallback = cb;
	Create(200,200, "../data/gui_resource_files/zgresource_rc.txt", "PropertyDlg");
	m_pkSelProperty = NULL;
}

EditPropertyDlg::~EditPropertyDlg()
{
	OnCloseAddProperty(false);
	OnCloseEditProperty(false);
}

bool EditPropertyDlg::Create(int x, int y, char* szResourceFile, char* szDlgName)
{
	m_pkGui->CaptureInput(true);

	ZGuiWnd* test = m_pkGui->Get("PropertyDlg");
	if(test)
	{
		ClearAllFileds();
		m_pkZGui->ShowMainWindow(test, true);
		return true;
	}

	m_pkZGui->LoadDialog(szResourceFile, szDlgName, PROPERTYPROC);

	// Initialize key commands to update values by pushing return while
	// textbox have focus and closing window by pushing Return or Escape,
	ZGuiWnd* pkDialog			= m_pkGui->Get("PropertyDlg");
	ZGuiWnd* pkPropOKBn			= m_pkGui->Get("ObjectPropOK");
	ZGuiWnd* pkPropCancelBn		= m_pkGui->Get("ObjectPropertiesCancel");
	ZGuiWnd* pkSetNewValueBN	= m_pkGui->Get("ChangeValueOKBn");
	ZGuiWnd* pkPropValSetEB		= m_pkGui->Get("PropertyValueSetEB");

	m_pkZGui->AddKeyCommand(KEY_RETURN, pkDialog, pkPropOKBn);
	m_pkZGui->AddKeyCommand(KEY_ESCAPE, pkDialog, pkPropCancelBn);
	m_pkDlgBox = pkDialog;

	m_pkZGui->ShowMainWindow(m_pkDlgBox, false);

	return true;
}

void EditPropertyDlg::OnOpenEditProperty()
{
	ZGuiWnd* pkPropertysCB = m_pkGui->Get("PropertyCB");
	ZGuiWnd* pkPropertyValuesCB = m_pkGui->Get("PropertyValuesCB");

	ZGuiWnd* pkNameEB = m_pkGui->Get("ObjectNameEB");
	ZGuiWnd* pkPosXEB = m_pkGui->Get("ObjectPosXEB");
	ZGuiWnd* pkPosYEB = m_pkGui->Get("ObjectPosYEB");
	ZGuiWnd* pkPosZEB = m_pkGui->Get("ObjectPosZEB");

	ZGuiWnd* pkRotXEB = m_pkGui->Get("ObjectRotX");
	ZGuiWnd* pkRotYEB = m_pkGui->Get("ObjectRotY");
	ZGuiWnd* pkRotZEB = m_pkGui->Get("ObjectRotZ");

	if(pkPropertysCB == NULL)
	{
		cout << "Failed to find combox named [PropertyCB]" << endl;
		return;
	}

	if(pkPropertyValuesCB == NULL)
	{
		cout << "Failed to find combox named [PropertyValuesCB]" << endl;
		return;
	}

	if(pkNameEB == NULL)
	{
		cout << "Failed to find editbox named [ObjectNameEB]" << endl;
		return;
	}

	((ZGuiCombobox*) pkPropertysCB)->RemoveAllItems();
	((ZGuiCombobox*) pkPropertyValuesCB)->RemoveAllItems();
	((ZGuiTextbox*) m_pkGui->Get("PropertyValueSetEB"))->SetText("");

	if(m_pkCurrentChild)
	{
		char szID[50];
		sprintf(szID, "%i", m_pkCurrentChild->iNetWorkID);
		m_pkGui->Get("ObjectID")->SetText(szID);

		char text[50];
		Vector3 p = m_pkCurrentChild->GetPos();

		((ZGuiTextbox*) pkNameEB)->SetText((char*)m_pkCurrentChild->GetName().c_str());

		sprintf(text, "%0.3f", p.x); ((ZGuiTextbox*) pkPosXEB)->SetText(text);
		sprintf(text, "%0.3f", p.y); ((ZGuiTextbox*) pkPosYEB)->SetText(text);
		sprintf(text, "%0.3f", p.z); ((ZGuiTextbox*) pkPosZEB)->SetText(text);

		p = m_pkCurrentChild->GetRot();

		sprintf(text, "%0.3f", p.x); ((ZGuiTextbox*) pkRotXEB)->SetText(text);
		sprintf(text, "%0.3f", p.y); ((ZGuiTextbox*) pkRotYEB)->SetText(text);
		sprintf(text, "%0.3f", p.z); ((ZGuiTextbox*) pkRotZEB)->SetText(text);

		vector<Property*> akPropertys;
		vector<string> akPropertyNames;

		m_pkCurrentChild->GetPropertys(&akPropertys, PROPERTY_TYPE_ALL, PROPERTY_SIDE_ALL);

		vector<Property*>::iterator s = akPropertys.begin();
		vector<Property*>::iterator e = akPropertys.end();

		int antal = akPropertys.size();
 
		for(int i=0 ; s != e; s++, i++ )
		{
			m_pkGui->AddItemToList(pkPropertysCB, true, (*s)->m_acName, i, (i == 0));
		}	

		// Send a message to the main winproc...
		if(((ZGuiCombobox*)pkPropertysCB)->GetListbox()->GetSelItem())
		{
			int* piParams = new int[2];
			piParams[0] = PropertyCB; 
			piParams[1] = ((ZGuiCombobox*)pkPropertysCB)->GetListbox()->GetSelItem()->GetIndex(); // list item ID
			DlgProc(pkPropertysCB->GetParent(), ZGM_CBN_SELENDOK, 2, piParams);
			delete[] piParams;
		}

		switch(m_pkCurrentChild->GetObjectType())
		{
		case OBJECT_TYPE_DYNAMIC:
			m_pkGui->Get("ObTypeDynamicRb")->Notify(NULL, NCODE_CLICK_UP);
			break;
		case OBJECT_TYPE_STATIC:
			m_pkGui->Get("ObTypeStaticRb")->Notify(NULL, NCODE_CLICK_UP);
			break;
		case OBJECT_TYPE_PLAYER:
			m_pkGui->Get("ObTypePlayerRb")->Notify(NULL, NCODE_CLICK_UP);
			break;
		case OBJECT_TYPE_STATDYN:
			m_pkGui->Get("ObTypeStaticDynamicRb")->Notify(NULL, NCODE_CLICK_UP);
			break;
		case OBJECT_TYPE_DECORATION:
			m_pkGui->Get("ObTypeDecorationRb")->Notify(NULL, NCODE_CLICK_UP);
			break;
		}
	}
}

bool EditPropertyDlg::OnCloseEditProperty(bool bSave)
{
	if(!m_pkCurrentChild)
	{
		m_pkGui->CaptureInput(false);
		return true;
	}

	ZGuiWnd* pkNameEB = m_pkGui->Get("ObjectNameEB");
	ZGuiWnd* pkXPosEB = m_pkGui->Get("ObjectPosXEB");
	ZGuiWnd* pkYPosEB = m_pkGui->Get("ObjectPosYEB");
	ZGuiWnd* pkZPosEB = m_pkGui->Get("ObjectPosZEB");

	ZGuiWnd* pkXRotEB = m_pkGui->Get("ObjectRotX");
	ZGuiWnd* pkYRotEB = m_pkGui->Get("ObjectRotY");
	ZGuiWnd* pkZRotEB = m_pkGui->Get("ObjectRotZ");

	if(bSave)
	{
		char* strName = pkNameEB->GetText();
		char* strXPos = pkXPosEB->GetText();
		char* strYPos = pkYPosEB->GetText();
		char* strZPos = pkZPosEB->GetText();

		char* strXRot = pkXRotEB->GetText();
		char* strYRot = pkYRotEB->GetText();
		char* strZRot = pkZRotEB->GetText();

		if( strName != NULL && strXPos != NULL && 
			strYPos != NULL && strZPos != NULL)
		{
			float x,y,z,rx,ry,rz;

			x = atof(strXPos);
			y = atof(strYPos);
			z = atof(strZPos);

			rx = atof(strXRot);
			ry = atof(strYRot);
			rz = atof(strZRot);

			char strText[512];
			strcpy(strText, pkNameEB->GetText());
		
			if(m_pkCurrentChild)
			{
				m_pkCurrentChild->GetName() = string(strText);
				m_pkCurrentChild->GetPos() = Vector3(x,y,z);
				m_pkCurrentChild->GetRot() = Vector3(rx,ry,rz);
			}
		}
	}

	if(bSave && m_pkCurrentChild != NULL)
	{
		if( ((ZGuiRadiobutton*)m_pkGui->Get("ObTypeDynamicRb"))->GetButton()->IsChecked())
			m_pkCurrentChild->GetObjectType() = OBJECT_TYPE_DYNAMIC;
		else
		if( ((ZGuiRadiobutton*)m_pkGui->Get("ObTypeStaticRb"))->GetButton()->IsChecked())
			m_pkCurrentChild->GetObjectType() = OBJECT_TYPE_STATIC;
		else
		if( ((ZGuiRadiobutton*)m_pkGui->Get("ObTypePlayerRb"))->GetButton()->IsChecked())
			m_pkCurrentChild->GetObjectType() = OBJECT_TYPE_PLAYER;
		else
		if( ((ZGuiRadiobutton*)m_pkGui->Get("ObTypeStaticDynamicRb"))->GetButton()->IsChecked())
			m_pkCurrentChild->GetObjectType() = OBJECT_TYPE_STATDYN;
		else
		if( ((ZGuiRadiobutton*)m_pkGui->Get("ObTypeDecorationRb"))->GetButton()->IsChecked())
			m_pkCurrentChild->GetObjectType() = OBJECT_TYPE_DECORATION;
	}

	if(bSave)
	{
		// Send a message to the main winproc...
		int* piParams = new int[1];
		piParams[0] = ChangeValueOKBn; 
		DlgProc(m_pkDlgBox, ZGM_COMMAND, 1, piParams);
		delete[] piParams;
	}

	m_pkGui->CaptureInput(false);

	return true;
}

ZGuiWnd* EditPropertyDlg::CreateAddPropertyDlg(int x, int y, int w, int h)
{
	ZGuiWnd* test = m_pkGui->Get("EditPropertyDlg");
	if( test)
	{
		m_pkZGui->ShowMainWindow(test, true);
		OnOpenAddProperty();
		return NULL;
	}

	ZGuiWnd* pkDlg = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_ADDPROPERTY_WND);
	pkDlg->SetSkin(m_pkGui->GetSkin("red"));
	pkDlg->SetMoveArea(m_pkGui->GetScreenRect());
	pkDlg->SetWindowFlag(WF_CLOSEABLE);

	vector<string> vkPropNames;
	m_pkPropFactory->GetAllProperties(vkPropNames);

	int iHeight = m_pkGui->CreateRadiobuttons(pkDlg, vkPropNames, 
		"PropRadioGroup", ID_PROPERTY_RADIOGROUP, 0, 0, 16);

	h = iHeight+5;
	pkDlg->Resize(w,h);

	if(y+h > m_pkGui->GetScreenRect().Height())
	{
		pkDlg->Move(0, -((y+h)-m_pkGui->GetScreenRect().Height()) );
	}

	ZGuiWnd* pkAddPropCloseBn = m_pkGui->CreateButton(pkDlg, ID_ADDPROPERTY_CLOSE, 
		w-20, 0, 20, 20, "x");
	pkAddPropCloseBn->SetWindowFlag(WF_CENTER_TEXT);
	ZGuiWnd* pkAddPropOKBn = m_pkGui->CreateButton(pkDlg, ID_ADDPROPERTY_OK, 
		w-40, h-20, 40, 20, "OK");
	pkAddPropOKBn->SetWindowFlag(WF_CENTER_TEXT);

	m_pkZGui->AddKeyCommand(KEY_RETURN, pkDlg, pkAddPropOKBn);
	m_pkZGui->AddKeyCommand(KEY_ESCAPE, pkDlg, pkAddPropCloseBn);

	m_pkZGui->AddMainWindow(ID_ADDPROPERTY_WND_MAIN, pkDlg, "EditPropertyDlg", 
		PROPERTYPROC, true); 

	return pkDlg;
}

bool EditPropertyDlg::OnCloseAddProperty(bool bSave)
{
	char* strPropText = NULL;
	ZGuiWnd* pkWndGroup = m_pkGui->Get("PropRadioGroup");

	if(pkWndGroup)
	{
		ZGuiRadiobutton* curr = ((ZGuiRadiobutton*)pkWndGroup);

		while(1)
		{
			if(curr->GetButton()->IsChecked())
			{
				strPropText = curr->GetButton()->GetText();
				break;
			}

			curr = curr->GetNext();
			if(curr == NULL)
				break;
		}
	}
	else
		return false;

	if(strPropText == NULL)
		return true; // No button selected.

	if(!m_pkCurrentChild)
		return true;

	if(bSave)
	{
		// Add new property.
		if(m_bAdd && m_pkCurrentChild != NULL)
		{
			m_pkCurrentChild->AddProperty(strPropText);
			ZGuiCombobox* pkPropertysCB = ((ZGuiCombobox*)m_pkGui->Get("PropertyCB"));
			pkPropertysCB->AddItem(strPropText, -1, true);

			// Send a message to the main winproc...
			int* piParams = new int[2];
			piParams[0] = /*ID_PROPERTIES_CB*/ PropertyCB; // Listbox ID // COMMENT OUT by: Zeb
			piParams[1] = pkPropertysCB->GetListbox()->GetItemCount(); // list item ID
			DlgProc(pkPropertysCB->GetParent(), ZGM_CBN_SELENDOK, 2, piParams);
			delete[] piParams;
		}
	}

	return true;
}

void EditPropertyDlg::OnOpenAddProperty()
{
//	m_pkZGui->SetFocus(m_pkGui->Get("AddPropWnd"));
}

bool EditPropertyDlg::DlgProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams )
{
	int iControllID;

	switch(uiMessage)
	{
	case ZGM_COMMAND:
		iControllID = ((int*)pkParams)[0];

		switch(iControllID)
		{
		case ObjectPropCloseButton:
		case ObjectPropertiesCancel:
			if(OnCloseEditProperty(false))
			{
				m_pkZGui->ShowMainWindow(m_pkGui->Get("PropertyDlg"), false);
			}
			break;
		case ObjectPropOK:
			if(OnCloseEditProperty(true))
			{
				m_pkZGui->ShowMainWindow(m_pkGui->Get("PropertyDlg"), false);
			}
			break;
		case AddPropertiesBn:
			int w, h; w = 200; h = 520;
			CreateAddPropertyDlg(m_pkGui->GetScreenRect().Width()-w,
				m_pkGui->GetScreenRect().Height()-h,w,h);
			break;
		case ID_ADDPROPERTY_CLOSE:
			m_bAdd = false;
			if(OnCloseAddProperty(true))
				m_pkZGui->ShowMainWindow(m_pkGui->Get("EditPropertyDlg"), false);
			break;
		case ID_ADDPROPERTY_OK:
			m_bAdd = true;
			if(OnCloseAddProperty(true))
				m_pkZGui->ShowMainWindow(m_pkGui->Get("EditPropertyDlg"), false);
			break;
		case RemovePropertyBn:
			RemoveProperty();
			break;
		case ChangeValueOKBn:
			{
				if(m_pkSelProperty)
				{
					vector<string> akPropertyNamesBefore;
					akPropertyNamesBefore = m_pkSelProperty->GetValueNames();

					string kPrevValue = m_pkSelProperty->GetValue(m_szSelPropValue);
					char* text = m_pkGui->Get("PropertyValueSetEB")->GetText();
					m_pkSelProperty->SetValue(m_szSelPropValue, string(text));
					
					vector<string> akPropertyNamesAfter;
					akPropertyNamesAfter = m_pkSelProperty->GetValueNames();

					bool bUpdate = false;

					if(akPropertyNamesBefore.size() == akPropertyNamesAfter.size())
					{
						for(unsigned int i=0; i<akPropertyNamesAfter.size(); i++)
							if(akPropertyNamesBefore[i] != akPropertyNamesAfter[i])
							{
								bUpdate = true;
								break;
							}
					}
					else
						bUpdate = true;

					if(bUpdate)
						OnOpenEditProperty();
				}
			}
			break;
		}
		break;

	case ZGM_CBN_SELENDOK:
		{
			int iID = ((int*)pkParams)[0];
			ZGuiListitem* pkSelItem;

			switch(iID)
			{
			case PropertyCB:

				pkSelItem = ((ZGuiCombobox*)m_pkGui->Get("PropertyCB"))->GetListbox()->GetSelItem();

				if(pkSelItem)
					m_szSelProperty = pkSelItem->GetText();
				else
					m_szSelProperty.erase();

				UpdateStats(iID);
				break;

			case PropertyValuesCB:

				pkSelItem = ((ZGuiCombobox*)m_pkGui->Get("PropertyValuesCB"))->GetListbox()->GetSelItem();

				if(pkSelItem)
					m_szSelPropValue = pkSelItem->GetText();
				else
					m_szSelPropValue.erase();

				UpdateStats(iID);
				break;
			}
		}
		break;
	}
	return true;
}

void EditPropertyDlg::RemoveProperty()
{
	ZGuiWnd* wnd = m_pkGui->Get("PropertyCB");

	if(wnd)
	{
		ZGuiListbox* pkPropList = ((ZGuiCombobox*)wnd)->GetListbox();
		ZGuiListitem* pkItem = pkPropList->GetSelItem();

		if(pkItem && m_pkCurrentChild != NULL)
		{
			m_pkCurrentChild->DeleteProperty(pkItem->GetText());
			((ZGuiCombobox*)wnd)->RemoveSelItem(true); // select prev

			// Uppdatatera comboxarna om ett alternativ är markerat
			if(((ZGuiCombobox*)wnd)->GetListbox()->GetSelItem())
			{
				// Send a message to the main winproc...
				int* piParams = new int[2];
				piParams[0] = PropertyCB; // Listbox ID
				piParams[1] = ((ZGuiCombobox*)wnd)->GetListbox()->GetItemCount(); // list item ID
				DlgProc(((ZGuiCombobox*)wnd)->GetParent(), ZGM_CBN_SELENDOK, 2, piParams);
				delete[] piParams;
			}
			// Töm den andra comboxen sammt editboxen om inget alternativ är markerat
			else
			{
				ZGuiWnd* pkPValueCB = m_pkGui->Get("PropertyValuesCB");

				if(pkPValueCB)	
					((ZGuiCombobox*) pkPValueCB)->RemoveAllItems();

				((ZGuiTextbox*) m_pkGui->Get("PropertyValueSetEB"))->SetText("");
			}
		}
	}
}

void EditPropertyDlg::UpdateStats(int ComboBoxID)
{
	if(ComboBoxID == PropertyCB)
	{
		if(!m_szSelProperty.empty() && m_pkCurrentChild != NULL)
		{
			m_pkSelProperty = m_pkCurrentChild->GetProperty( (char*) m_szSelProperty.c_str() );
		
			// Clear property values combobox
			ZGuiCombobox* pkPValueCB = (ZGuiCombobox*) m_pkGui->Get("PropertyValuesCB");
			pkPValueCB->RemoveAllItems();

			// Fill property values combobox
			vector<string> akPropertyNames = m_pkSelProperty->GetValueNames();
			vector<string>::iterator s = akPropertyNames.begin();
			vector<string>::iterator e = akPropertyNames.end();
			int counter=0;
			int antal = akPropertyNames.size();

			for( ; s != e; s++ )
			{
				m_pkGui->AddItemToList(pkPValueCB, true, (char*)(*s).c_str(), 
					counter, (counter == 0)); // Select the last item in the combox

				counter++;
			}

			string strPropValue = "";
			ZGuiListitem* pkPropValueCBSelItem = pkPValueCB->GetListbox()->GetSelItem();

			if(pkPropValueCBSelItem)
			{
				m_szSelPropValue = pkPropValueCBSelItem->GetText();
				strPropValue = m_pkSelProperty->GetValue(m_szSelPropValue);
			}

			// Set selected property value in textbox
			m_pkGui->Get("PropertyValueSetEB")->SetText((char*) strPropValue.c_str());
		}
		else
		{
			m_pkSelProperty = NULL;

			// Clear property values combobox
			ZGuiCombobox* pkPValueCB = (ZGuiCombobox*) m_pkGui->Get("PropertyValuesCB");
			pkPValueCB->RemoveAllItems();

			// Clear property values textbox
			m_pkGui->Get("PropertyValueSetEB")->SetText("");
		}
	}
	else
	if(ComboBoxID == /*ID_PROPERTY_VALUES_CB*/ PropertyValuesCB) // COMMENT OUT by: Zeb
	{
		string strPropValue = "";
		strPropValue = m_pkSelProperty->GetValue(m_szSelPropValue);

		// Set selected property value in textbox
		m_pkGui->Get("PropertyValueSetEB")->SetText((char*) strPropValue.c_str());
	}
}

bool EditPropertyDlg::IsOpen()
{
	ZGuiWnd* test = m_pkGui->Get("PropertyDlg");
	if(test)
	{
		return test->IsVisible();
	}	

	return false;
}

void EditPropertyDlg::ClearAllFileds()
{
	m_pkGui->Get("PropertyValueSetEB")->SetText("");
	
	m_pkGui->Get("ObjectID")->SetText("");
	m_pkGui->Get("ObjectNameEB")->SetText("");
	m_pkGui->Get("ObjectPosXEB")->SetText("");
	m_pkGui->Get("ObjectPosYEB")->SetText("");
	m_pkGui->Get("ObjectPosZEB")->SetText("");
	m_pkGui->Get("ObjectRotX")->SetText("");
	m_pkGui->Get("ObjectRotY")->SetText("");
	m_pkGui->Get("ObjectRotZ")->SetText("");

	((ZGuiRadiobutton*)m_pkGui->Get("ObTypeDynamicRb"))->GetButton()->UncheckButton();
	((ZGuiRadiobutton*)m_pkGui->Get("ObTypeStaticRb"))->GetButton()->UncheckButton();
	((ZGuiRadiobutton*)m_pkGui->Get("ObTypePlayerRb"))->GetButton()->UncheckButton();
	((ZGuiRadiobutton*)m_pkGui->Get("ObTypeStaticDynamicRb"))->GetButton()->UncheckButton();
	((ZGuiRadiobutton*)m_pkGui->Get("ObTypeDecorationRb"))->GetButton()->UncheckButton();

	((ZGuiCombobox*)m_pkGui->Get("PropertyCB"))->RemoveAllItems();
	((ZGuiCombobox*)m_pkGui->Get("PropertyValuesCB"))->RemoveAllItems();
}

bool EditPropertyDlg::OnOpen(int x, int y)
{
	//ClearAllFileds();
	OnOpenEditProperty();
	return true;
}

bool EditPropertyDlg::OnClose(bool bSave)
{
	OnCloseEditProperty(bSave);
	return true;
}








