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

	// Add for the entity variables display.
	pkProperyList->AddItem("Variables", j++, false);

	Entity* pkEnt = m_pkEntityManager->GetEntityByID(iID);
   if(pkEnt)
   {
	   list<string>::iterator it = temp.begin();
	   for( ; it!=temp.end(); it++) 
	   {
		   if(pkEnt->GetProperty((*it).c_str()) != NULL)
			   pkProperyList->AddItem((char*)(*it).c_str(), j++, false);
	   }
   }
   else
   {
      ShowWnd("AddNewProperyWnd",false);
      ShowWnd("EditPropertyWnd",false);
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

	if((pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject)))
		if((item = GetSelItem("PropertyList"))) 
		{

			if(string("Variables") == string(item))
			{
				cout << "Would like to look at entity vars";
				vector<string> kValNames;
				pkEnt->GetAllVarNames(kValNames);
				
				list<string> temp;
				for(int i=0; i<kValNames.size(); i++)
					temp.push_back(kValNames[i]);

				temp.sort(); 

				int j=0;
				list<string>::iterator it = temp.begin();
				for( ; it!=temp.end(); it++) 
					pkProperyValList->AddItem((char*)(*it).c_str(), j++, false);
			}

			else if((pkProp = pkEnt->GetProperty(item)))
			{
				vector<string> kValNames;
				kValNames = pkProp->GetValueNames();
				
				list<string> temp;
				for(int i=0; i<kValNames.size(); i++)
					temp.push_back(kValNames[i]);

				temp.sort(); 

				int j=0;
				list<string>::iterator it = temp.begin();
				for( ; it!=temp.end(); it++) 
					pkProperyValList->AddItem((char*)(*it).c_str(), j++, false);
			}
		}
}
void ZeroEd::AddProperty(int iEntityID,const string& strProperty)
{
	NetPacket kNp;
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("addproperty");
	kNp.Write(iEntityID);
	kNp.Write_Str(strProperty);
	
	m_pkZeroFps->RouteEditCommand(&kNp);	
	
	UpdatePropertyList(m_iCurrentObject);
}

void ZeroEd::AddPropertyVal()
{
	char* item;

	if(item = GetSelItem("PropertyList"))
	{
		char* val = GetSelItem("PropertyValList");
		char* res = GetText("PropertyValEb");

		NetPacket kNp;
		kNp.Write((char) ZFGP_EDIT);
		kNp.Write_Str("setvariable");
		kNp.Write(m_iCurrentObject);
		kNp.Write_Str(string(item));
		kNp.Write_Str(string(val));
		kNp.Write_Str(string(res));			
		m_pkZeroFps->RouteEditCommand(&kNp);								
	}

	m_pkGui->SetFocus(GetWnd("EditPropertyWnd"));
}

void ZeroEd::RemoveSelProperty()
{
	char* item;

	item = GetSelItem("PropertyList");
	
	NetPacket kNp;
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("removeproperty");
	kNp.Write(m_iCurrentObject);
	kNp.Write_Str(string(item));
	
	m_pkZeroFps->RouteEditCommand(&kNp);	
	
	UpdatePropertyList(m_iCurrentObject);
}

bool ZeroEd::SaveCurrentToScript()
{
   Entity* pkEnt;
   Property* pkProp;
   ZFBasicFS* pkBFPS;	
   PropertyFactory* pkPropFuck;
   string strFileName = GetText("SaveScriptFileNameEb");

   bool bUpdate = false;

   string CREATED_SCRIPTS_DIR = "data/script/objects/";
   
	if((pkBFPS = static_cast<ZFBasicFS*>(g_ZFObjSys.GetObjectPtr("ZFBasicFS"))) &&
      (pkPropFuck = static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"))) &&
      (pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject)) && !strFileName.empty())
   {
      if( strFileName.find(".lua") == string::npos )
         strFileName += string(".lua");

      //if(!pkBFPS->DirExist(CREATED_SCRIPTS_DIR.c_str()))
	     // pkBFPS->CreateDir(CREATED_SCRIPTS_DIR.c_str());

      BuildFileTree("ObjectTree", (char*) CREATED_SCRIPTS_DIR.c_str(), ".lua");

      ZFVFile kFile;
      if(kFile.Open(CREATED_SCRIPTS_DIR + strFileName, 0,true))
      {
         fprintf(kFile.m_pkFilePointer, "function Create()\n\n");
         fprintf(kFile.m_pkFilePointer, "\tInitObject()\n");

         vector<string> vkProperties;
	      pkPropFuck->GetAllProperties(vkProperties);
       
         for(int i=0; i<vkProperties.size(); i++)
         {
            if((pkProp=pkEnt->GetProperty(vkProperties[i].c_str())) != NULL)
            {
		         fprintf(kFile.m_pkFilePointer, "\t\tInitProperty(\"%s\")\n", vkProperties[i].c_str());

               vector<string> kValNames = pkProp->GetValueNames();
               for(int j=0; j<kValNames.size(); j++)
                  fprintf(kFile.m_pkFilePointer, "\t\t\tInitParameter(\"%s\",\"%s\")\n", 
                     kValNames[j].c_str(), pkProp->GetValue( kValNames[j].c_str() ) );
            }
         }
         
         fprintf(kFile.m_pkFilePointer, "\tSetParentObject()\n");
         fprintf(kFile.m_pkFilePointer, "\tSetReturnObject()\n");
         fprintf(kFile.m_pkFilePointer, "end\n");
         kFile.Close();
         bUpdate = true;
      }
   }

   ShowWnd("SaveScriptFileNameEb",false);
   GetWnd("ObjectTree")->Resize(200, 200, false);
   ((ZGuiTextbox*) GetWnd("SaveScriptFileNameEb"))->KillFocus();

   if(bUpdate)
   {
      BuildFileTree("ObjectTree", (char*) CREATED_SCRIPTS_DIR.c_str(), ".lua");

      ShowWnd("SaveScriptFileNameEb",false);
      GetWnd("ObjectTree")->Resize(200, 200, false);
      ((ZGuiTextbox*) GetWnd("SaveScriptFileNameEb"))->KillFocus();
   }
 
   return true;
}












