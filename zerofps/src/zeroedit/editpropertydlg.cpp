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
	m_pkGui = pkGui;
	m_pkZGui = m_pkGui->GetGUI(); 
	m_pkCurrentChild = pkCurrentChild;
	m_pkPropFactory = pf;
	m_pkObjectManager = om;
	m_oGuiCallback = cb;
	m_pkWindow = Create(0,0,500,500);
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
		return NULL;
	}

	ZGuiWnd* pkMainWindow = new ZGuiWnd(Rect(x,y,x+w,y+h),NULL,true,ID_PROPERTY_WND);
	pkMainWindow->SetSkin(m_pkGui->GetSkin("blue"));
	pkMainWindow->SetMoveArea(Rect(0,0,m_pkGui->m_pkEdit->m_iWidth,m_pkGui->m_pkEdit->m_iHeight));
	pkMainWindow->SetWindowFlag(WF_CLOSEABLE);

	int y_pos;

	m_pkGui->CreateButton(pkMainWindow, ID_PROPERTY_WND_CLOSE, w-20, 0, 20, 20, "x")->SetWindowFlag(WF_CENTER_TEXT);
	m_pkGui->CreateLabel(pkMainWindow, 0, 20, 20, 16*5, 20, "Name:");
	m_pkGui->Register( m_pkGui->CreateTextbox(pkMainWindow, ID_NAME_TEXTBOX, 16*6, 20, 200, 20), "ObjectNameEB" );

	m_pkGui->CreateLabel(pkMainWindow, 0, 20, 60, 16*4, 20, "Pos:");
	m_pkGui->Register( m_pkGui->CreateTextbox(pkMainWindow, ID_POSX_TEXTBOX, 16*6, 60, 16*6, 20), "ObjectPosXEB");
	m_pkGui->Register( m_pkGui->CreateTextbox(pkMainWindow, ID_POSY_TEXTBOX, 16*6+16*7, 60, 16*6, 20), "ObjectPosYEB"); 
	m_pkGui->Register( m_pkGui->CreateTextbox(pkMainWindow, ID_POSZ_TEXTBOX, 16*6+16*7*2, 60, 16*6, 20), "ObjectPosZEB");

	y_pos = 100;

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
		y_pos, 16*6+16*7*2, h-(y_pos+60), false), "PropertyValuesCB");
	cb->SetNumVisibleRows(20);

	y_pos += 30;

	m_pkGui->CreateLabel(pkMainWindow, 0, 20, y_pos, 16*9-5, 20, "Change:");
	m_pkGui->Register( m_pkGui->CreateTextbox(pkMainWindow, ID_PROPERTY_NEW_VALUE_EB, 
		16*6, y_pos, 100, 20), "PropertyValueSetEB" );
	m_pkGui->CreateButton(pkMainWindow, ID_PROPERTY_SET_NEW_VALUE_BN, 16*6+120, y_pos, 80, 20, "OK")->SetWindowFlag(WF_CENTER_TEXT);

	m_pkGui->CreateButton(pkMainWindow, ID_PROPERTY_OK, w-100, h-50, 80, 20, "OK")->SetWindowFlag(WF_CENTER_TEXT);
	m_pkGui->CreateButton(pkMainWindow, ID_PROPERTY_CANCEL, w-100, h-25, 80, 20, "Cancel")->SetWindowFlag(WF_CENTER_TEXT);
	
	m_pkZGui->AddMainWindow(ID_PROPERTY_WND_MAIN, pkMainWindow, PROPERTYPROC, true);

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

		((ZGuiCombobox*) pkPropertysCB)->RemoveAllItems();
 
		for(int i ; s != e; s++, i++ )
		{
			m_pkGui->AddItemToList(pkPropertysCB, true, (*s)->m_acName, i++);

			akPropertyNames.clear(); 
			akPropertyNames = (*s)->GetValueNames();
/*
			if(!akPropertyNames.empty())
			{
				vector<string>::iterator s2 = akPropertyNames.begin();
				vector<string>::iterator e2 = akPropertyNames.end();

				for(int j ; s2 != e2; s2++, j++ )
					m_pkGui->AddItemToList(pkPropertyValuesCB, true, (char*)(*s2).c_str(), j++);
			}
			else
			{
				printf("No property values for property %s\n", (*s)->m_acName);
			}*/
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
	return true;
}

ZGuiWnd* EditPropertyDlg::CreateAddPropertyDlg(int x, int y, int w, int h)
{
	if( m_pkGui->Get("AddPropWnd") )
	{
		m_pkGui->Get("AddPropWnd")->Show();
		OnOpenAddProperty();
		return NULL;
	}

	ZGuiWnd* mamma = m_pkZGui->GetMainWindow(ID_PROPERTY_WND_MAIN);

	ZGuiWnd* pkDlg = new ZGuiWnd(Rect(x,y,x+w,y+h),mamma,true,ID_ADDPROPERTY_WND);
	pkDlg->SetSkin(m_pkGui->GetSkin("red"));
	pkDlg->SetMoveArea(m_pkGui->GetScreenRect());
	pkDlg->SetWindowFlag(WF_CLOSEABLE);

	m_pkGui->Register( pkDlg, "AddPropWnd" );

	m_pkGui->CreateButton(pkDlg, ID_ADDPROPERTY_CLOSE, w-20, 0, 20, 20, "x")->SetWindowFlag(WF_CENTER_TEXT);
	m_pkGui->CreateButton(pkDlg, ID_ADDPROPERTY_OK, w-40, h-30, 40, 20, "OK")->SetWindowFlag(WF_CENTER_TEXT);
	
	m_pkZGui->RegisterWindow(pkDlg);
	pkDlg->SetZValue(434343);
	mamma->SortChilds();

	vector<string> vkPropNames;
	m_pkPropFactory->GetAllProperties(vkPropNames);

	m_pkGui->CreateRadiobuttons(pkDlg, vkPropNames, "PropRadioGroup", ID_PROPERTY_RADIOGROUP, 0, 0, 16);

	return pkDlg;
}

bool EditPropertyDlg::OnCloseAddProperty(bool bSave)
{
	char* strPropText;
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

	if(!m_pkCurrentChild)
		return true;

	if(bSave)
	{
		// Add new property.
		if(m_bAdd)
		{
			m_pkCurrentChild->AddProperty(strPropText);
			printf("Adding property %s\n", strPropText);

			ZGuiListbox* pkPropertysCB = ((ZGuiCombobox*)m_pkGui->Get("PropertyCB"))->GetListbox();
			pkPropertysCB->AddItem(strPropText, pkPropertysCB->GetItemCount());
		}
	}

	return true;
}

void EditPropertyDlg::OnOpenAddProperty()
{

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
				m_pkGui->Get("AddPropWnd")->Hide();
			break;
		case ID_ADDPROPERTY_OK:
			m_bAdd = true;
			if(OnCloseAddProperty(true))
				m_pkGui->Get("AddPropWnd")->Hide();
			break;
		case ID_REMOVEPROPERTY_BN:
			RemoveProperty();
			break;
		case ID_PROPERTY_SET_NEW_VALUE_BN:
			{
				if(m_pkSelProperty)
				{
					string kPrevValue = m_pkSelProperty->GetValue(m_kSelPropValue);
					char* text = m_pkGui->Get("PropertyValueSetEB")->GetText();
					m_pkSelProperty->SetValue(m_kSelPropValue, string(text));
				}
			}
			break;
		}
		break;

	case ZGM_CBN_SELENDOK:
		{
			int iID = ((int*)pkParams)[0];
			ZGuiWnd *win = ((ZGuiWnd*)m_pkZGui->GetWindow(iID));

			switch(iID)
			{
			case ID_PROPERTIES_CB:
				{
					ZGuiListitem* pkSelItem = ((ZGuiCombobox*)(win))->GetListbox()->GetSelItem();

					if(pkSelItem)
					{
						m_pkSelProperty = m_pkCurrentChild->GetProperty(pkSelItem->GetText());
						vector<string> akPropertyNames = m_pkSelProperty->GetValueNames();

						ZGuiWnd* pkPValueCB = m_pkGui->Get("PropertyValuesCB");
						
						if(!akPropertyNames.empty())
						{
							vector<string>::iterator s = akPropertyNames.begin();
							vector<string>::iterator e = akPropertyNames.end();
							int counter=0;

							for( ; s != e; s++ )
							{
								m_pkGui->AddItemToList(pkPValueCB, true, (char*)(*s).c_str(), counter++);
							}
						}
					}
				}
				break;

			case ID_PROPERTY_VALUES_CB:
				ZGuiListitem* pkSelItem = ((ZGuiCombobox*)(win))->GetListbox()->GetSelItem();

				if(pkSelItem && m_pkSelProperty)
				{
					m_kSelPropValue = pkSelItem->GetText();
					m_pkGui->Get("PropertyValueSetEB")->SetText((char*)
						m_pkSelProperty->GetValue(m_kSelPropValue).c_str());
				}
				break;

/*			case 324124124:
				ZGuiListitem* pkSelItem = ((ZGuiCombobox*)(win))->GetListbox()->GetSelItem();

				if(pkSelItem)
				{
					Object* pkObject = m_pkObjectManager->GetObject(pkSelItem->GetText());

					if(pkObject)
					{
						ZGuiWnd* pkWnd = m_pkZGui->GetWindow(ID_PROPERTIES_CB);

						if(pkWnd)
						{
							ZGuiCombobox* pkCB = (ZGuiCombobox*) pkWnd;
							pkCB->RemoveAllItems();

							list<Property*> pkList;
							pkObject->GetPropertys(&pkList, PROPERTY_TYPE_ALL, PROPERTY_SIDE_ALL);

							list<Property*>::iterator s = pkList.begin();
							list<Property*>::iterator e = pkList.end();
							int counter=0;

							for( ; s != e; s++ )
							{
								pkCB->SetLabelText((*s)->m_acName);
								m_pkGui->AddItemToList(pkCB, true, (*s)->m_acName, counter++);
							}
						}
					}
				}
				break;*/

/*			case ID_PROPERTY_VALUES_CB:
				ZGuiTextbox* pkPropValEB = (ZGuiTextbox*) (m_pkGui->Get( "PropertyValueSetEB"));
				ZGuiCombobox* pkPropValCB = (ZGuiCombobox*) (m_pkGui->Get( "PropertyValuesCB"));

				if(pkPropValEB && pkPropValLB)
				{
					char* prop_key = pkPropValCB->GetListbox()->GetSelItem()->GetText();
					//char* value = pkPropValEB->SetText();

					Property* prop = m_pkCurrentChild->GetProperty(prop_key):

					prop->GetValue(prop_key);
				}

				break;*/
			}
		}
		break;
	}
	return true;
}

void EditPropertyDlg::RemoveProperty()
{
	ZGuiListbox* pkPropertyList = ((ZGuiCombobox*)m_pkGui->Get("PropertyCB"))->GetListbox();

	if(pkPropertyList)
	{
		ZGuiListitem* pkItem = pkPropertyList->GetSelItem();

		if(pkItem)
		{
			m_pkCurrentChild->DeleteProperty(pkItem->GetText());
			printf("Removing property %s\n", pkItem->GetText());
			
			int iID = pkItem->GetID();
			pkPropertyList->RemoveItem(iID);

			if(pkPropertyList->GetItemCount() == 0)
			{
				((ZGuiCombobox*)m_pkGui->Get("PropertyCB"))->SetLabelText("");
			}
		}
	}
}
