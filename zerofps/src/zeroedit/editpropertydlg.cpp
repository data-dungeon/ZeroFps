// editpropertydlg.cpp: implementation of the EditPropertyDlg class.
//
//////////////////////////////////////////////////////////////////////


#include "editpropertydlg.h"
#include "../zerofps/engine/propertyfactory.h"
#include "../zerofps/engine/objectmanager.h"
#include "resource_id.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Window proc wrappers
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern ZeroEdit Editor;

static bool PROPERTYPROC( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams ) {
	return Editor.m_pkGui->m_pkEditPropDlgBox->DlgProc(pkWindow, uiMessage, iNumberOfParams, pkParams); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EditPropertyDlg::EditPropertyDlg(Gui* pkGui, PropertyFactory* pf, ObjectManager* om, 
								 Object* pkCurrentChild, callback cb)
{
	m_szSelProperty.erase();
	m_szSelPropValue.erase();

	m_pkGui = pkGui;
	m_pkZGui = m_pkGui->GetGUI(); 
	m_pkCurrentChild = pkCurrentChild;
	m_pkPropFactory = pf;
	m_pkObjectManager = om;
	m_oGuiCallback = cb;
	m_pkWindow = Create(200,200,500,300);
	m_pkSelProperty = NULL;
	OnOpenEditProperty();
}

EditPropertyDlg::~EditPropertyDlg()
{

}

ZGuiWnd* EditPropertyDlg::Create(int x, int y, int w, int h)
{
	if( m_pkZGui->GetMainWindow(ID_PROPERTY_WND_MAIN))
	{
		m_pkZGui->ShowMainWindow(ID_PROPERTY_WND_MAIN, true);
		return m_pkZGui->GetMainWindow(ID_PROPERTY_WND_MAIN);
	}

	ZGuiWnd* pkMainWindow = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_PROPERTY_WND);
	pkMainWindow->SetSkin(m_pkGui->GetSkin("main"), -1, m_pkGui->GetTexture("border_corner_a"));
	pkMainWindow->SetMoveArea(Rect(0,0,m_pkGui->m_pkEdit->m_iWidth,m_pkGui->m_pkEdit->m_iHeight));
	pkMainWindow->SetWindowFlag(WF_CLOSEABLE);

	int y_pos;

	m_pkGui->CreateButton(pkMainWindow, ID_PROPERTY_WND_CLOSE, w-20, 0, 20, 20, "x")->SetWindowFlag(WF_CENTER_TEXT);
	m_pkGui->CreateLabel(pkMainWindow, 0, 20, 20, 16*5, 20, "Name:");
	m_pkGui->Register( m_pkGui->CreateTextbox(pkMainWindow, ID_NAME_TEXTBOX, 16*6, 20, 
		16*6+16*7*2, 20), "ObjectNameEB" );

	m_pkGui->CreateLabel(pkMainWindow, 0, 20, 50, 16*4, 20, "Pos:");
	m_pkGui->Register( m_pkGui->CreateTextbox(pkMainWindow, ID_POSX_TEXTBOX, 16*6, 50, 16*6, 20), "ObjectPosXEB");
	m_pkGui->Register( m_pkGui->CreateTextbox(pkMainWindow, ID_POSY_TEXTBOX, 16*6+16*7, 50, 16*6, 20), "ObjectPosYEB"); 
	m_pkGui->Register( m_pkGui->CreateTextbox(pkMainWindow, ID_POSZ_TEXTBOX, 16*6+16*7*2, 50, 16*6, 20), "ObjectPosZEB");

	y_pos = 80;

	m_pkGui->CreateLabel(pkMainWindow, 0, 20, y_pos, 16*9-5, 20, "Props:");

	ZGuiCombobox* cb;
	m_pkGui->Register(cb = m_pkGui->CreateCombobox(pkMainWindow, ID_PROPERTIES_CB, 16*6, 
		y_pos, 16*6+16*7*2, h-(y_pos+60), false), "PropertyCB");
	cb->SetNumVisibleRows(20);

	ZGuiButton* bn;
	m_pkGui->Register(bn = m_pkGui->CreateButton(pkMainWindow, ID_ADDPROPERTY_BN, 16*6+16*6+16*7*2+10, 
		y_pos, 70, 20, "Add"), "AddPropertyBN");
	bn->SetWindowFlag(WF_CENTER_TEXT);

	m_pkGui->Register(bn = m_pkGui->CreateButton(pkMainWindow, ID_REMOVEPROPERTY_BN, 16*6+16*6+16*7*2+10, 
		y_pos+30, 70, 20, "Remove"), "AddPropertyBN");
	bn->SetWindowFlag(WF_CENTER_TEXT);

	y_pos += 30;

	m_pkGui->CreateLabel(pkMainWindow, 0, 20, y_pos, 16*9-5, 20, "Values:");

	m_pkGui->Register(cb = m_pkGui->CreateCombobox(pkMainWindow, ID_PROPERTY_VALUES_CB, 16*6, 
		y_pos, 16*6+16*7*2, 500, false), "PropertyValuesCB");
	cb->SetNumVisibleRows(20);

	y_pos += 30;

	m_pkGui->CreateLabel(pkMainWindow, 0, 20, y_pos, 16*9-5, 20, "Change:");
	
	ZGuiWnd* pkPropValSetEB = m_pkGui->CreateTextbox(pkMainWindow, ID_PROPERTY_NEW_VALUE_EB, 16*6, y_pos, 16*6+16*7*2, 20);
	m_pkGui->Register(pkPropValSetEB, "PropertyValueSetEB" );
	ZGuiWnd* pkSetNewValueBN = m_pkGui->CreateButton(pkMainWindow, ID_PROPERTY_SET_NEW_VALUE_BN, 
		16*6+16*6+16*7*2+10, y_pos, 70, 20, "OK");
	pkSetNewValueBN->SetWindowFlag(WF_CENTER_TEXT);

	ZGuiWnd* pkPropOKBn = m_pkGui->CreateButton(pkMainWindow, ID_PROPERTY_OK, 16*6+16*6+16*7*2+10, h-50, 70, 20, "OK");
	pkPropOKBn->SetWindowFlag(WF_CENTER_TEXT);
	ZGuiWnd* pkPropCancelBn = m_pkGui->CreateButton(pkMainWindow, ID_PROPERTY_CANCEL, 16*6+16*6+16*7*2+10, h-25, 70, 20, "Cancel");
	pkPropCancelBn->SetWindowFlag(WF_CENTER_TEXT);

	y_pos += 30;

	m_pkGui->CreateLabel(pkMainWindow, 0, 20, y_pos, 100, 20, "ObjectType");

	vector<string> vkObjectTypeNames;
	vkObjectTypeNames.push_back("OBJECT_TYPE_DYNAMIC");
	vkObjectTypeNames.push_back("OBJECT_TYPE_STATIC");
	vkObjectTypeNames.push_back("OBJECT_TYPE_PLAYER");
	vkObjectTypeNames.push_back("OBJECT_TYPE_STATDYN");

	m_pkGui->CreateRadiobuttons(pkMainWindow, vkObjectTypeNames, "ObjecttypeRadioGroup", 
		ID_OBJECTTYPE_RADIOGROUP, 10, 200, 16); 
	
	m_pkZGui->AddMainWindow(ID_PROPERTY_WND_MAIN, pkMainWindow, PROPERTYPROC, true);

	m_pkZGui->AddKeyCommand(KEY_RETURN, pkMainWindow, pkPropOKBn);
	m_pkZGui->AddKeyCommand(KEY_ESCAPE, pkMainWindow, pkPropCancelBn);
	m_pkZGui->AddKeyCommand(KEY_RETURN, pkPropValSetEB, pkSetNewValueBN); 

	return pkMainWindow;
}

void EditPropertyDlg::OnOpenEditProperty()
{
	ZGuiWnd* pkPropertysCB = m_pkGui->Get("PropertyCB");
	ZGuiWnd* pkPropertyValuesCB = m_pkGui->Get("PropertyValuesCB");

	ZGuiWnd* pkNameEB = m_pkGui->Get("ObjectNameEB");
	ZGuiWnd* pkPosXEB = m_pkGui->Get("ObjectPosXEB");
	ZGuiWnd* pkPosYEB = m_pkGui->Get("ObjectPosYEB");
	ZGuiWnd* pkPosZEB = m_pkGui->Get("ObjectPosZEB");

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
		char text[50];
		Vector3 p = m_pkCurrentChild->GetPos();
		
		((ZGuiTextbox*) pkNameEB)->SetText((char*)m_pkCurrentChild->GetName().c_str());

		sprintf(text, "%0.3f", p.x); ((ZGuiTextbox*) pkPosXEB)->SetText(text);
		sprintf(text, "%0.3f", p.y); ((ZGuiTextbox*) pkPosYEB)->SetText(text);
		sprintf(text, "%0.3f", p.z); ((ZGuiTextbox*) pkPosZEB)->SetText(text);

		list<Property*> akPropertys;
		vector<string> akPropertyNames;

		m_pkCurrentChild->GetPropertys(&akPropertys, PROPERTY_TYPE_ALL, PROPERTY_SIDE_ALL);

		list<Property*>::iterator s = akPropertys.begin();
		list<Property*>::iterator e = akPropertys.end();

		int antal = akPropertys.size();
 
		for(int i=0 ; s != e; s++, i++ )
		{
			m_pkGui->AddItemToList(pkPropertysCB, true, (*s)->m_acName, i, (i == 0));
		}	

		// Send a message to the main winproc...
		if(((ZGuiCombobox*)pkPropertysCB)->GetListbox()->GetSelItem())
		{
			int* piParams = new int[2];
			piParams[0] = ID_PROPERTIES_CB; // Listbox ID
			piParams[1] = ((ZGuiCombobox*)pkPropertysCB)->GetListbox()->GetSelItem()->GetIndex(); // list item ID
			DlgProc(pkPropertysCB->GetParent(), ZGM_CBN_SELENDOK, 2, piParams);
			delete[] piParams;
		}

		int apa = m_pkCurrentChild->GetObjectType();

		ZGuiWnd* pkWndGroup = m_pkGui->Get("ObjecttypeRadioGroup");

		if(pkWndGroup)
		{
			ZGuiRadiobutton* curr = ((ZGuiRadiobutton*)pkWndGroup);

			while(1)
			{
				int index = curr->GetButton()->GetID() - ID_OBJECTTYPE_RADIOGROUP;

				if(index == apa)
				{
					curr->GetButton()->CheckButton();
				}
				else
				{
					curr->GetButton()->UncheckButton();
				}

				curr = curr->GetNext();
				if(curr == NULL)
					break;
			}
		}
	}
}

bool EditPropertyDlg::OnCloseEditProperty(bool bSave)
{
	if(!m_pkCurrentChild)
		return true;

	ZGuiWnd* pkNameEB = m_pkGui->Get("ObjectNameEB");
	ZGuiWnd* pkXPosEB = m_pkGui->Get("ObjectPosXEB");
	ZGuiWnd* pkYPosEB = m_pkGui->Get("ObjectPosYEB");
	ZGuiWnd* pkZPosEB = m_pkGui->Get("ObjectPosZEB");

	if(pkNameEB)
	{
		char* strName = pkNameEB->GetText();
		char* strXPos = pkXPosEB->GetText();
		char* strYPos = pkYPosEB->GetText();
		char* strZPos = pkZPosEB->GetText();

		if( strName != NULL && strXPos != NULL && 
			strYPos != NULL && strZPos != NULL)
		{
			float x,y,z;

			x = atof(strXPos);
			y = atof(strYPos);
			z = atof(strZPos);

			char strText[512];
			strcpy(strText, pkNameEB->GetText());
		
			if(m_pkCurrentChild)
			{
				m_pkCurrentChild->GetName() = string(strText);
				m_pkCurrentChild->GetPos() = Vector3(x,y,z);
			}
		}
	}

	int index = -1;
	ZGuiWnd* pkWndGroup = m_pkGui->Get("ObjecttypeRadioGroup");

	if(pkWndGroup)
	{
		ZGuiRadiobutton* curr = ((ZGuiRadiobutton*)pkWndGroup);

		while(1)
		{
			if(curr->GetButton()->IsChecked())
			{
				index = curr->GetButton()->GetID();
				break;
			}

			curr = curr->GetNext();
			if(curr == NULL)
				break;
		}
	}
	else
		return false;

	if(index != -1 && bSave)
	{
		int type_offset = index-ID_OBJECTTYPE_RADIOGROUP;
		if(type_offset >= 0 && type_offset < 4)
			m_pkCurrentChild->GetObjectType() = ObjectType(index-ID_OBJECTTYPE_RADIOGROUP);
		else
		{
			printf("Failed to set object type\n");
		}
	}

	// Send a message to the main winproc...
	int* piParams = new int[1];
	piParams[0] = ID_PROPERTY_SET_NEW_VALUE_BN; // Listbox ID
	DlgProc(m_pkWindow, ZGM_COMMAND, 1, piParams);
	delete[] piParams;

	return true;
}

ZGuiWnd* EditPropertyDlg::CreateAddPropertyDlg(int x, int y, int w, int h)
{
	if( m_pkZGui->GetMainWindow(ID_ADDPROPERTY_WND_MAIN))
	{
		m_pkZGui->ShowMainWindow(ID_ADDPROPERTY_WND_MAIN, true);
		OnOpenAddProperty();
		return NULL;
	}

//	ZGuiWnd* mamma = m_pkZGui->GetMainWindow(ID_PROPERTY_WND_MAIN);

	ZGuiWnd* pkDlg = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_ADDPROPERTY_WND);
	pkDlg->SetSkin(m_pkGui->GetSkin("red"));
	pkDlg->SetMoveArea(m_pkGui->GetScreenRect());
	pkDlg->SetWindowFlag(WF_CLOSEABLE);

//	m_pkGui->Register( pkDlg, "AddPropWnd" );

	ZGuiWnd* pkAddPropCloseBn = m_pkGui->CreateButton(pkDlg, ID_ADDPROPERTY_CLOSE, w-20, 0, 20, 20, "x");
	pkAddPropCloseBn->SetWindowFlag(WF_CENTER_TEXT);
	ZGuiWnd* pkAddPropOKBn = m_pkGui->CreateButton(pkDlg, ID_ADDPROPERTY_OK, w-40, h-30, 40, 20, "OK");
	pkAddPropOKBn->SetWindowFlag(WF_CENTER_TEXT);

	m_pkZGui->AddKeyCommand(KEY_RETURN, pkDlg, pkAddPropOKBn);
	m_pkZGui->AddKeyCommand(KEY_ESCAPE, pkDlg, pkAddPropCloseBn);

//	m_pkZGui->RegisterWindow(pkDlg);
//	pkDlg->SetZValue(434343);
//	mamma->SortChilds();

	vector<string> vkPropNames;
	m_pkPropFactory->GetAllProperties(vkPropNames);

	m_pkGui->CreateRadiobuttons(pkDlg, vkPropNames, "PropRadioGroup", ID_PROPERTY_RADIOGROUP, 0, 0, 16);

	m_pkZGui->AddMainWindow(ID_ADDPROPERTY_WND_MAIN, pkDlg, PROPERTYPROC, true); 

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
		if(m_bAdd)
		{
			m_pkCurrentChild->AddProperty(strPropText);
			ZGuiCombobox* pkPropertysCB = ((ZGuiCombobox*)m_pkGui->Get("PropertyCB"));
			pkPropertysCB->AddItem(strPropText, -1, true);

			// Send a message to the main winproc...
			int* piParams = new int[2];
			piParams[0] = ID_PROPERTIES_CB; // Listbox ID
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
		case ID_PROPERTY_CANCEL:
		case ID_PROPERTY_WND_CLOSE:
			if(OnCloseEditProperty(false))
				m_pkZGui->ShowMainWindow(ID_PROPERTY_WND_MAIN, false);
			break;
		case ID_PROPERTY_OK:
			if(OnCloseEditProperty(true))
				m_pkZGui->ShowMainWindow(ID_PROPERTY_WND_MAIN, false);
			break;
		case ID_ADDPROPERTY_BN:
			CreateAddPropertyDlg(0,0,300,500);
			break;
		case ID_ADDPROPERTY_CLOSE:
			m_bAdd = false;
			if(OnCloseAddProperty(true))
				m_pkZGui->ShowMainWindow(ID_ADDPROPERTY_WND_MAIN, false);
			break;
		case ID_ADDPROPERTY_OK:
			m_bAdd = true;
			if(OnCloseAddProperty(true))
				m_pkZGui->ShowMainWindow(ID_ADDPROPERTY_WND_MAIN, false);
			break;
		case ID_REMOVEPROPERTY_BN:
			RemoveProperty();
			break;
		case ID_PROPERTY_SET_NEW_VALUE_BN:
			{
				if(m_pkSelProperty)
				{
					string kPrevValue = m_pkSelProperty->GetValue(m_szSelPropValue);
					char* text = m_pkGui->Get("PropertyValueSetEB")->GetText();
					m_pkSelProperty->SetValue(m_szSelPropValue, string(text));
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
			case ID_PROPERTIES_CB:

				pkSelItem = ((ZGuiCombobox*)m_pkGui->Get("PropertyCB"))->GetListbox()->GetSelItem();

				if(pkSelItem)
					m_szSelProperty = pkSelItem->GetText();
				else
					m_szSelProperty.erase();

				UpdateStats(iID);
				break;

			case ID_PROPERTY_VALUES_CB:

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

		if(pkItem)
		{
			m_pkCurrentChild->DeleteProperty(pkItem->GetText());
			((ZGuiCombobox*)wnd)->RemoveSelItem(true); // select prev

			// Uppdatatera comboxarna om ett alternativ är markerat
			if(((ZGuiCombobox*)wnd)->GetListbox()->GetSelItem())
			{
				// Send a message to the main winproc...
				int* piParams = new int[2];
				piParams[0] = ID_PROPERTIES_CB; // Listbox ID
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
	if(ComboBoxID == ID_PROPERTIES_CB)
	{
		if(!m_szSelProperty.empty())
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
	if(ComboBoxID == ID_PROPERTY_VALUES_CB)
	{
		string strPropValue = "";
		strPropValue = m_pkSelProperty->GetValue(m_szSelPropValue);

		// Set selected property value in textbox
		m_pkGui->Get("PropertyValueSetEB")->SetText((char*) strPropValue.c_str());
	}
}


