#include "zeroed.h"

bool ZeroEd::UpdatePropertyList(int iID)
{
	ZGuiListbox* pkProperyList = (ZGuiListbox*) GetWnd("PropertyList");
	if(pkProperyList == NULL)
		return false;
	pkProperyList->RemoveAllItems();

   ZGuiListbox* pkProperyValList = (ZGuiListbox*) GetWnd("PropertyValList");
	if(pkProperyValList == NULL)
		return false;
	pkProperyValList->RemoveAllItems();

   SetText("PropertyValEb", "");

	vector<string> vkProperties;
	PropertyFactory* pkPropFuck = 
		static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));

	list<string> temp;
	pkPropFuck->GetAllProperties(vkProperties);
	for(int i=0; i<vkProperties.size(); i++)
		temp.push_back(vkProperties[i]);

	temp.sort(); 

	int j=0;

	Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(iID);
   if(pkEnt)
   {
	   list<string>::iterator it = temp.begin();
	   for( ; it!=temp.end(); it++) 
	   {
		   if(pkEnt->GetProperty((*it).c_str()) != NULL)
			   pkProperyList->AddItem((char*)(*it).c_str(), j++, false);
	   }
   }

	return true;
}

bool ZeroEd::FillPropertyList()
{
	ZGuiListbox* pkProperyList = (ZGuiListbox*) GetWnd("AllPropertyList");
	if(pkProperyList == NULL)
		return false;

	pkProperyList->RemoveAllItems();

	vector<string> vkProperties;
	PropertyFactory* pkPropFuck = 
		static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));

	list<string> temp;
	pkPropFuck->GetAllProperties(vkProperties);
	for(int i=0; i<vkProperties.size(); i++)
		temp.push_back(vkProperties[i]);

	temp.sort(); 

	int j=0;
	list<string>::iterator it = temp.begin();
	for( ; it!=temp.end(); it++) 
		pkProperyList->AddItem((char*)(*it).c_str(), j++, false);

	return true;
}

void ZeroEd::FillPropertyValList()
{
	char* item;
	Entity* pkEnt;
	Property* pkProp;

   ZGuiListbox* pkProperyValList = (ZGuiListbox*) GetWnd("PropertyValList");
	if(pkProperyValList == NULL)
		return;

	pkProperyValList->RemoveAllItems();

	if((pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject)))
		if((item = GetSelItem("PropertyList")))
			if((pkProp = pkEnt->GetProperty(item)))
			{
				vector<string> apa;
				apa = pkProp->GetValueNames();
				
				list<string> temp;
				for(int i=0; i<apa.size(); i++)
					temp.push_back(apa[i]);

				temp.sort(); 

				int j=0;
				list<string>::iterator it = temp.begin();
				for( ; it!=temp.end(); it++) 
					pkProperyValList->AddItem((char*)(*it).c_str(), j++, false);
			}
}

void ZeroEd::AddPropertyVal()
{
	char* item;
	Entity* pkEnt;
	Property* pkProp;

	if((pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject)))
		if((item = GetSelItem("PropertyList")))
			if((pkProp = pkEnt->GetProperty(item)))
			{
				char* val = GetSelItem("PropertyValList");
				char* res = GetText("PropertyValEb");
				
				if(val != NULL && res != NULL)
					pkProp->SetValue(val, res);
			}

	m_pkGui->SetFocus(GetWnd("EditPropertyWnd"));
}

void ZeroEd::RemoveSelProperty()
{
	char* item;
	Entity* pkEnt;
	Property* pkProp;

	if((pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject)))
		if((item = GetSelItem("PropertyList")))
			if((pkProp = pkEnt->GetProperty(item)))
			{
				pkEnt->RemoveProperty(pkProp);
				UpdatePropertyList(m_iCurrentObject);
			}
}