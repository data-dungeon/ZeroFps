#include "objectmanager.h"
#include "network.h"
#include "zerofps.h"
#include "netslaveobject.h"

ObjectManager::ObjectManager() 
: ZFObject("ObjectManager") 
{
	iNextObjectID		= 0;
	m_bUpdate			= true;
	
	m_pkWorldObject	=	new Object();	
	m_pkWorldObject->GetName() = "WorldObject";

	m_pkZeroFps=static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));		
	TESTVIM_LoadArcheTypes("zfoh.txt");

	g_ZFObjSys.Register_Cmd("o_logtree",FID_LOGOHTREE,this);	
	g_ZFObjSys.Register_Cmd("o_dumpp",FID_LOGACTIVEPROPERTYS,this);	
	g_ZFObjSys.Register_Cmd("sendmsg",FID_SENDMESSAGE,this, "sendmsg name id",2);	

}

ObjectManager::~ObjectManager() 
{

}

// Add/Remove Objects
void ObjectManager::Add(Object* pkObject) 
{
	pkObject->iNetWorkID = iNextObjectID++;
	m_akObjects.push_back(pkObject);
}

void ObjectManager::Remove(Object* pkObject) 
{	
	m_akObjects.remove(pkObject);
}

void ObjectManager::Delete(Object* pkObject) 
{
	for(vector<Object*>::iterator it=m_akDeleteList.begin();it!=m_akDeleteList.end();it++) 
	{
		if(pkObject == (*it)) {
			cout << "Object already in delete list" << endl;
			return;
		}
	}
	
	m_akDeleteList.push_back(pkObject);
}

void ObjectManager::Clear()
{
	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		if((*it)->CheckLinks(false,0) == false)
			cout << "Error in object manger" << endl;
		else
			cout << "Links ok" << endl;
	} 
	
	m_pkWorldObject->DeleteAllChilds();
}


// Updates
void ObjectManager::Update(int iType,int iSide,bool bSort)
{
	if(!m_bUpdate)
		if(iType!=PROPERTY_TYPE_RENDER)
			return;

	GetPropertys(iType,iSide);
	
	m_iNrOfActivePropertys = m_akPropertys.size();
	
	m_pkZeroFps->DevPrintf("om", "OM::Update(%s, %s,%d) = %d",
		GetPropertyTypeName(iType),GetPropertySideName(iSide),bSort,m_iNrOfActivePropertys);


	if(bSort){
		sort(m_akPropertys.begin(),m_akPropertys.end(),Less_Property);
		//m_akPropertys.sort(Less_Property);	
	}
	
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		(*it)->Update();
	}
	
}

void ObjectManager::UpdateDelete()
{
	int i=0;
	for(vector<Object*>::iterator it2=m_akDeleteList.begin();it2!=m_akDeleteList.end();it2++) 
	{
		Object* pkObject = (*it2);
		//cout << "[" << i << "]:" << pkObject->GetName() << "," << pkObject->iNetWorkID << endl;
		i++;
	}

	int iSize = m_akDeleteList.size();

	if(m_akDeleteList.size()==0)
		return;
	
	for(vector<Object*>::iterator it=m_akDeleteList.begin();it!=m_akDeleteList.end();it++) 
	{
		Object* pkObject = (*it);
		delete (*it);		
	}
	
	m_akDeleteList.clear();
}

void ObjectManager::UpdateGameMessages(void)
{
	// Let Objects/Propertys handle messages
	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		(*it)->HandleMessages();
	}
}


// Create 
Object* ObjectManager::CreateObject(const char* acName)
{	
	ObjectDescriptor *objtemplate = GetTemplate(acName);
	
	//return null if the template does not exist
	if(objtemplate==NULL)
		return NULL;
	
	return CreateObject(objtemplate);	
}

Object* ObjectManager::CreateObject(ObjectDescriptor* pkObjDesc)
{
	Object* tempobject=new Object;

	tempobject->GetName()=pkObjDesc->m_kName;
	tempobject->SetPos(pkObjDesc->m_kPos);		//två gånger för interpolering
	tempobject->SetPos(pkObjDesc->m_kPos);		//två gånger för interpolering
	tempobject->SetRot(pkObjDesc->m_kRot);		//två gånger för interpolering
	tempobject->SetRot(pkObjDesc->m_kRot);		//två gånger för interpolering
	tempobject->GetVel()=pkObjDesc->m_kVel;
	tempobject->GetAcc()=pkObjDesc->m_kAcc;	
	
	tempobject->GetSave()=pkObjDesc->m_bSave;
	tempobject->GetObjectType()=pkObjDesc->m_iObjectType;	
	
	for(list<PropertyDescriptor*>::iterator it=pkObjDesc->m_acPropertyList.begin();it!=pkObjDesc->m_acPropertyList.end();it++) 
	{
		if(tempobject->AddProperty((*it)->m_kName.c_str()))
		{
			//cout<<"Added property "<<(*it)->m_kName.c_str()<<endl;
			(*it)->m_kData.SetPos(0);
			tempobject->GetProperty((*it)->m_kName.c_str())->Load(&(*it)->m_kData);
			//cout<<"Loaded "<<(*it)->m_kName.c_str()<<endl;
		}
	}
	
	return tempobject;
}


Object* ObjectManager::CreateObjectByNetWorkID(int iNetID)
{
	Object *pkNew = new NetSlaveObject;
	//	Add(pkNew);
	pkNew->iNetWorkID = iNetID;
	pkNew->SetParent(m_pkWorldObject);
	g_ZFObjSys.Logf("net", " CreateObjectByNetWorkID( %d ).\n", iNetID);

	return pkNew;
}

Object* ObjectManager::CreateObjectByArchType(const char* acName)
{
	ObjectArcheType* pkAt = GetArcheType(string(acName));
	if(!pkAt)
		return false;

	Object* pkObj =	new Object;
	AddArchPropertys(pkObj, string(acName));

	pkObj->m_strType	= acName;
	pkObj->m_kName		= string("A ") + pkObj->m_strType;

	return pkObj;
}




// Template
void ObjectManager::AddTemplate(ObjectDescriptor* pkNewTemplate)
{
	m_akTemplates.push_back(pkNewTemplate);
}

int ObjectManager::GetNrOfTemplates() 
{	
	return m_akTemplates.size();
}


void ObjectManager::GetTemplateList(vector<string>* paList)
{
	for(list<ObjectDescriptor*>::iterator it=m_akTemplates.begin();it!=m_akTemplates.end();it++) 
	{
		paList->push_back((*it)->m_kName);	
	}
}

bool ObjectManager::MakeTemplate(const char* acName,Object* pkObject, bool bForce)
{
	ObjectDescriptor* tempdesc = GetTemplate(acName);

	if(tempdesc == NULL) {
		tempdesc = new ObjectDescriptor;
		}
	else {
		if(bForce == false)
			return false;
		
		tempdesc->Clear();
		}

/*
	ObjectDescriptor* tempdesc = new ObjectDescriptor;
	
	//set name
	tempdesc->m_kName=acName;
	tempdesc->m_kPos=pkObject->GetPos();
	tempdesc->m_kRot=pkObject->GetRot();
	tempdesc->m_kVel=pkObject->GetVel();
	
	list<Property*> pkPropertys;
	
	pkObject->GetPropertys(&pkPropertys,PROPERTY_TYPE_ALL,PROPERTY_SIDE_ALL);
	
	for(list<Property*>::iterator it=pkPropertys.begin();it!=pkPropertys.end();it++) 
	{
		PropertyDescriptor* pkP=new PropertyDescriptor;
		pkP->m_kName=(*it)->m_acName;
		(*it)->Save(&pkP->m_kData);
		
		tempdesc->m_acPropertyList.push_back(pkP);
	}
	*/
	
	
	pkObject->Save(tempdesc);	
	
	tempdesc->m_kName=acName;
	
	AddTemplate(tempdesc);
	return true;
}




void ObjectManager::ClearTemplates()
{
	for(list<ObjectDescriptor*>::iterator it=m_akTemplates.begin();it!=m_akTemplates.end();it++) 
	{
		delete (*it);
	}
	
	m_akTemplates.clear();
}


ObjectDescriptor* ObjectManager::GetTemplate(const char* acName)
{
	for(list<ObjectDescriptor*>::iterator it=m_akTemplates.begin();it!=m_akTemplates.end();it++) 
	{
		if((*it)->m_kName==acName)
			return (*it);
	}
	
	return NULL;
}

bool ObjectManager::LoadTemplate(const char* acFile)
{
	cout<<"loading template"<<endl;
	
	ZFFile fil;
	if(!fil.Open(acFile,false)){
		return false;
	}
		
	ObjectDescriptor* newtemplate=new ObjectDescriptor;	

	newtemplate->LoadFromFile(&fil);
	
	AddTemplate(newtemplate);
	
	fil.Close();

	return true;
}

bool ObjectManager::SaveTemplate(const char* acName,const char* acFile)
{
	cout<<"saving template "<<acName<<endl;
	
	ObjectDescriptor* objtemplate=GetTemplate(acName);
	if(objtemplate==NULL)
		return false;
	
	ZFFile kFile;
	if(!kFile.Open(acFile,true))
		return false;
	
	objtemplate->SaveToFile(&kFile);	
	
	kFile.Close();
		
	return true;
}

bool ObjectManager::IsA(Object* pkObj, string strStringType)
{
	ObjectArcheType* pkAt = GetArcheType(pkObj->m_strType);
	if(!pkAt)
		return false;	// Object not created from archtype at all :(.
	
	ObjectArcheType* pkParentAt;


	bool bDone = false;
	while(bDone){
		if(pkAt->m_strName == strStringType)
			return true;

		pkParentAt = GetArcheType(pkAt->m_strParentName);
		if(!pkParentAt)
			return false;
		else
			pkAt = pkParentAt;
		}

	return false;
}


// Load/Save Objects
bool ObjectManager::SaveAllObjects(const char* acFile)
{
	ZFFile kFile;
	if(!kFile.Open(acFile,true))
		return false;
	
	ObjectDescriptor kObd;
	
	list<Object*> kObjectlist;
	
	GetAllObjects(&kObjectlist);
	
	for(list<Object*>::iterator it=kObjectlist.begin();it!=kObjectlist.end();it++)
	{
		//the the object dont want to save dont save it
		if(!(*it)->GetSave())
			continue;
	
		(*it)->Save(&kObd);
				
		kObd.SaveToFile(&kFile);
		
		kObd.Clear();
	}

	kFile.Close();

	return true;
}

bool ObjectManager::LoadAllObjects(const char* acFile)
{
	ZFFile kFile;
	if(!kFile.Open(acFile,false))
		return false;
	
	ObjectDescriptor kObd;

	while(kObd.LoadFromFile(&kFile))
	{		
		Object* kObject = CreateObject(&kObd);
		kObject->AttachToClosestZone();
		
		kObd.Clear();
	}
	
	kFile.Close();
	
	return true;
}

// Gets
void ObjectManager::GetAllObjects(list<Object*> *pakObjects)
{
	m_pkWorldObject->GetAllObjects(pakObjects);
}

Object* ObjectManager::GetObject(const char* acName)
{
	list<Object*> kObjects;		
	GetAllObjects(&kObjects);
	
	for(list<Object*>::iterator it=kObjects.begin();it!=kObjects.end();it++) {
		if((*it)->GetName() == acName)
		{
			return (*it);
		}
	}	
	
	return NULL;
}

Object*	ObjectManager::GetObjectByNetWorkID(int iNetID)
{
	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		if((*it)->iNetWorkID == iNetID)
			return (*it);
	}

	return NULL;
}


// NetWork
void ObjectManager::UpdateState(NetPacket* pkNetPacket)
{
	Object* pkNetSlave;
	int iObjectID;
//	int iParentObjectID;
	pkNetPacket->Read(iObjectID);

	while(iObjectID != -1) {
		g_ZFObjSys.Logf("net", "UpdateState: Object %d\n", iObjectID);
//		pkNetPacket->Read(iParentObjectID);

		pkNetSlave = GetObjectByNetWorkID(iObjectID);
		if(pkNetSlave == NULL) {
				g_ZFObjSys.Logf("net", " Object '%d' not found. Trying to create...\n", iObjectID);	
			pkNetSlave = CreateObjectByNetWorkID(iObjectID);
			}
				
		if( pkNetSlave ) {
			g_ZFObjSys.Logf("net", " Refreshing object %d.\n", iObjectID);
			pkNetSlave->PackFrom(pkNetPacket);
			pkNetPacket->Read(iObjectID);
			}
		else {
			g_ZFObjSys.Logf("net", " Object '%d' not found (again) :(.\n", iObjectID);
			return;
			}
		}	
}

void ObjectManager::PackToClients()
{
	NetWork* net = static_cast<NetWork*>(g_ZFObjSys.GetObjectPtr("NetWork"));
	if(net->m_eNetStatus != NET_SERVER)	return;

	NetPacket NP;
	NP.Clear();
	NP.Write((char) ZF_NETTYPE_UNREL);
	NP.Write((char) ZFGP_OBJECTSTATE);

	int iNumOfObjects = m_akObjects.size();
	int iPacketSize = 0;
	int iEndOfObject = -1;


	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		if((*it)->NeedToPack()) {
			NP.Write((*it)->iNetWorkID);
			/*if((*it)->GetParent()) {
				NP.Write((*it)->GetParent()->iNetWorkID);
				}
			else
				NP.Write(int (-1));*/

			(*it)->PackTo(&NP);
	
			iPacketSize++;
			}


		if(iPacketSize >= 10) {
			NP.Write(iEndOfObject);
			NP.Write(ZFGP_ENDOFPACKET);
			net->SendToAllClients(&NP);

			NP.Clear();
			NP.Write((char) ZF_NETTYPE_UNREL);
			NP.Write((char) ZFGP_OBJECTSTATE);

			iPacketSize = 0;
			}
	}

	NP.Write(iEndOfObject);
	NP.Write(ZFGP_ENDOFPACKET);
	net->SendToAllClients(&NP);
}

// Debug / Help Functions		

void ObjectManager::DisplayTree()
{
	g_ZFObjSys.Log_Create("fisklins");
	m_pkWorldObject->PrintTree(0);
}


void ObjectManager::DumpActiverPropertysToLog(char* szMsg)
{
	g_ZFObjSys.Logf("net", "%s : %d\n", szMsg, m_akPropertys.size() );

	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		g_ZFObjSys.Logf("net", "%s", (*it)->m_acName );
		if((*it)->GetObject()->m_pkParent)
			g_ZFObjSys.Logf("net", " Parent Obj: %s\n", (*it)->GetObject()->m_pkParent->m_kName.c_str() );


		//cout << (*it)->m_acName << endl;
	}
	
}

// Message System.
void ObjectManager::SendMsg()
{

}

void ObjectManager::RouteMessage(GameMessage& Msg)
{
	Object*	pkObject = GetObjectByNetWorkID(Msg.m_ToObject);

	if(pkObject == NULL) {
		cout << "No Valid object for message" << endl;		
		return;
		}

	pkObject->AddGameMessage(Msg);
}


// Get Strings.
char* ObjectManager::GetUpdateStatusName(int eStatus)
{
	char* pkName = "";

	switch(eStatus) {
		case UPDATE_NONE: 		pkName = "UPDATE_NONE";	break;
		case UPDATE_ALL: 		pkName = "UPDATE_ALL";	break;
		case UPDATE_STATIC: 	pkName = "UPDATE_STATIC";	break;
		case UPDATE_DYNAMIC: 	pkName = "UPDATE_DYNAMIC";	break;
		case UPDATE_PLAYERS: 	pkName = "UPDATE_PLAYERS";	break;
		case UPDATE_DECORATION: pkName = "UPDATE_DECORATION";	break;
		case UPDATE_LIGHT: 		pkName = "UPDATE_LIGHT";	break;
		}

	return pkName;
}

char* ObjectManager::GetObjectTypeName(int eType)
{
	char* pkName = "";

	switch(eType) {
		case OBJECT_TYPE_DYNAMIC: 		pkName = "OBJECT_TYPE_DYNAMIC";	break;
		case OBJECT_TYPE_STATIC: 		pkName = "OBJECT_TYPE_STATIC";	break;
		case OBJECT_TYPE_PLAYER: 		pkName = "OBJECT_TYPE_PLAYER";	break;
		case OBJECT_TYPE_STATDYN:		pkName = "OBJECT_TYPE_STATDYN";	break;
		case OBJECT_TYPE_DECORATION: 	pkName = "OBJECT_TYPE_DECORATION";	break;
		}

	return pkName;
}

char* ObjectManager::GetPropertyTypeName(int iType)
{
	char* pkName = "";

	switch(iType) {
		case PROPERTY_TYPE_ALL: 		pkName = "PROPERTY_TYPE_ALL";	break;
		case PROPERTY_TYPE_NORMAL: 		pkName = "PROPERTY_TYPE_NORMAL";	break;
		case PROPERTY_TYPE_RENDER: 		pkName = "PROPERTY_TYPE_RENDER";	break;
		case PROPERTY_TYPE_PHYSIC: 		pkName = "PROPERTY_TYPE_PHYSIC";	break;
		}

	return pkName;

}

char* ObjectManager::GetPropertySideName(int iSide)
{
	char* pkName = "";

	switch(iSide) {
		case PROPERTY_SIDE_ALL: 		pkName = "PROPERTY_SIDE_ALL";	break;
		case PROPERTY_SIDE_CLIENT: 		pkName = "PROPERTY_SIDE_CLIENT";	break;
		case PROPERTY_SIDE_SERVER: 		pkName = "PROPERTY_SIDE_SERVER";	break;
		}

	return pkName;


}







// Object ArcheTypes

ObjectArcheType*	ObjectManager::GetArcheType(string strName)
{
	for(list<ObjectArcheType*>::iterator it=m_akArcheTypes.begin();it!=m_akArcheTypes.end();it++) {
		if((*it)->m_strName == strName)
			return (*it);
		}

	return NULL;
	
}

void ObjectManager::AddArchPropertys(Object* pkObj, string strName)
{
	ObjectArcheType* pkAt = GetArcheType(strName);
	if(!pkAt)
		return;

	Property* pkProperty;

	AddArchPropertys(pkObj, pkAt->m_strParentName);
	for(int i=0; i<pkAt->m_kArchPropertys.size(); i++) {
		pkProperty = pkObj->AddProxyProperty(pkAt->m_kArchPropertys[i].m_strName.c_str());

		for(int j=0; j<pkAt->m_kArchPropertys[i].m_kVariables.size(); j++) {
			pkProperty = pkObj->GetProperty(pkAt->m_kArchPropertys[i].m_strName.c_str());
	
			pkProperty->SetValue(pkAt->m_kArchPropertys[i].m_kVariables[j].m_strVariable.c_str(),
				pkAt->m_kArchPropertys[i].m_kVariables[j].m_strValue.c_str());
			}
		}
	
}

void ObjectManager::TESTVIM_LoadArcheTypes(char* szFileName)
{
	SimpleScriptFile kMMScipt;

	if(kMMScipt.LoadScript(szFileName) == false) {
		cout << "Failed to load script " << szFileName << endl;
		return;
		}


	char* ucpToken;
	ucpToken = kMMScipt.GetToken();
	ObjectArcheType* pkAt = NULL;
	
	string strPropName, strPropVar, strPropValue;

	while(ucpToken)
	{
		if (!strcmp (ucpToken, "parent")) {
			if(pkAt) {
				ucpToken = kMMScipt.GetToken();
				pkAt->m_strParentName = ucpToken;
				}
		}

		if (!strcmp (ucpToken, "class")) {
			if(pkAt) {
				m_akArcheTypes.push_back(pkAt);
				pkAt = NULL;
				}

			pkAt = new ObjectArcheType;
			ucpToken = kMMScipt.GetToken();
			pkAt->m_strName = ucpToken;
			cout << "Creating class: " << ucpToken << endl;
			}

		
		if (!strcmp (ucpToken, "set")) {
			if(pkAt) {
				ucpToken = kMMScipt.GetToken();
				strPropName = ucpToken;

				ucpToken = kMMScipt.GetToken();
				strPropVar = ucpToken;

				ucpToken = kMMScipt.GetToken();
				strPropValue = ucpToken;
	
				pkAt->SetValue(strPropName, strPropVar, strPropValue);
				cout << "Set:  " << strPropName << "." <<  strPropVar << "=" << strPropValue << endl;
				}
			}

		if (!strcmp (ucpToken, "add")) {
			if(pkAt) {
				ucpToken = kMMScipt.GetToken();
				strPropName = ucpToken;
				pkAt->GetAddArchProperty(string (strPropName) );
				cout << "Add:  " << strPropName << endl;
				}
			}

		ucpToken = kMMScipt.GetToken();
	}

	if(pkAt) {
		m_akArcheTypes.push_back(pkAt);
		pkAt = NULL;
		}
}

void ObjectManager::GetPropertys(int iType,int iSide)
{
	m_akPropertys.clear();
	m_pkWorldObject->GetAllPropertys(&m_akPropertys,iType,iSide);

/*
	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		(*it)->GetPropertys(&m_akPropertys,iType,iSide);		
	}
*/
//	cout<<"TOTAL propertys: "<<m_akPropertys.size()<<endl;
}



void ObjectManager::RunCommand(int cmdid, const CmdArgument* kCommand) 
{ 

	switch(cmdid) {
		case FID_LOGOHTREE:
			DisplayTree();
			break;

		case FID_LOGACTIVEPROPERTYS:
			DumpActiverPropertysToLog("Active propertys");
			break;

		case FID_SENDMESSAGE:
/*			gm.m_FromObject = -1;
			gm.m_ToObject	= atoi(kCommand->m_kSplitCommand[2].c_str());
			gm.m_Name		= kCommand->m_kSplitCommand[1].c_str();*/
//			m_pkConsole->Printf("Sending Msg '%s' to %d from %d", gm.m_Name.c_str(), gm.m_ToObject, gm.m_FromObject);
			//RouteMessage(gm);
			break;
	}	

}











//vector<ObjectTemplate*>	m_ObjectTemplates;
/*void ObjectManager::Create_OT(int iID)
{
	Console* pkConsole;
	pkConsole = dynamic_cast<Console*>(g_ZFObjSys.GetObjectPtr("Console"));

	if(iID >= 0) {
		pkConsole->Printf("Object template ID must be below zero.");
		return;
		}

	// Check if ID already create.
	ObjectTemplate* pkOT = new ObjectTemplate;
//	pkOT->m_iArvFrom = 0;

//	m_ObjectTemplates.push_back( pkOT );
//	pkConsole->Printf("Object template (%d) created.", iID);

}

void ObjectManager::Destory_OT(int iID)
{

}

void ObjectManager::LoadGameObjects(const char* szFileName)
{

}

void ObjectManager::SaveGameObjects(const char* szFileName)
{

}
*/


/*
Object*	ObjectManager::GetObjectByNetWorkID(int iNetID)
{
	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		if((*it)->iNetWorkID == iNetID)
			return (*it);
	}

	return NULL;
}*/



/*
void ObjectManager::Update(){
	if(m_bNoUpdate)
		return;
	
	UpdateDelete();

	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
//		(*it)->ObjectUpdate();	
		(*it)->Update();		 
	}
}

void ObjectManager::Update(int iType){
	UpdateDelete();

	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		//if the object is of specified type, then update it
		if((*it)->GetType()==iType){
//			(*it)->ObjectUpdate();				
			(*it)->Update();
		}
	}
}
*/



 




/*	string strFileName;
	string strName;

	while(ucpToken)
	{
		if (!strcmp (ucpToken, "!scale"))
		{
			ucpToken = kMMScipt.GetToken();
			g_fExportScale = atof(ucpToken);
			cout << "Setting Scale to: " << g_fExportScale << endl;
			g_fTotalScale	= (1.0 / g_fUnitsMeter) * g_fExportScale;
		}

		if (!strcmp (ucpToken, "!filetype")) {
			cout << "Command filetype" << endl;
			}
		
		if (!strcmp (ucpToken, "!target"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Target file is: " << ucpToken << endl;
			ucaOutFile = ucpToken;
		}

		if (!strcmp (ucpToken, "!sd-export"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Setting Bones" << ucpToken << endl;
			ReadExportSD(ucpToken);
		}

		if (!strcmp (ucpToken, "!ad-export"))
		{
			strFileName = kMMScipt.GetToken();
			strName = kMMScipt.GetToken();
			cout << "Add Animation '" << strName.c_str() << "' from " << strFileName.c_str() << endl;
			ReadExportAD(strFileName.c_str(),strName.c_str());
		}

		if (!strcmp (ucpToken, "!add-md"))
		{
			strFileName = kMMScipt.GetToken();
			strName = kMMScipt.GetToken();
			cout << "Add Mesh '" << strName.c_str() << "' from " << strFileName.c_str() << endl;
			ReadCoreMesh(strFileName.c_str(),strName.c_str());
		}

		if (!strcmp (ucpToken, "!add-fd"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command add-fd: " << ucpToken << endl;
			ReadBaseFrame(ucpToken);
		}

		if (!strcmp (ucpToken, "!add-meshanim"))
		{
			ucpToken = kMMScipt.GetToken();
			cout << "Command add-meshanim: " << ucpToken << endl;
			ReadAnimation(ucpToken);
		}
		
		ucpToken = kMMScipt.GetToken();
	}
*/
void ObjectManager::TESTVIM_SpawnArcheTypes()
{
/*	ObjectArcheType* pkAt;

	// Create Object
	pkAt = new ObjectArcheType;
	
	pkAt->m_strName = "Object";
	pkAt->m_strParentName = "";

	// Create Physics
		pkAt = new ObjectArcheType;
		pkAt->m_strName = "Physic";
		pkAt->m_strParentName = "";
		pkAt->SetValue("PhysicProperty", "ColShape",	"3");
		pkAt->SetValue("PhysicProperty", "m_bGravity",	"false");

		m_akArcheTypes.push_back(pkAt);

	// Create Tree
		pkAt = new ObjectArcheType;
		pkAt->m_strName = "Tree";
		pkAt->m_strParentName = "";
		pkAt->SetValue("MadProperty", "m_kMadFile","../data/mad/tree.mad");
		pkAt->SetValue("MadProperty", "m_fScale","1");
		m_akArcheTypes.push_back(pkAt);

	// Create Avsats
		pkAt = new ObjectArcheType;
		pkAt->m_strName = "Avsats";
		pkAt->m_strParentName = "Physic";
		pkAt->SetValue("MadProperty", "m_kMadFile",	"../data/mad/avsats.mad");
		pkAt->SetValue("MadProperty", "m_fScale",		"1");
	

		pkAt->SetValue("PhysicsProperty", "Gravity",	"0");
		pkAt->SetValue("PhysicsProperty", "Float",	"0");
		pkAt->SetValue("PhysicsProperty", "Solid",	"1");
		pkAt->SetValue("PhysicsProperty", "Glide",	"1");
		pkAt->SetValue("PhysicsProperty", "Stride",	"0");
		
		pkAt->SetValue("PhysicsProperty", "StrideHieght",	"0.7");
		pkAt->SetValue("PhysicsProperty", "Radius",	"4.6");
		pkAt->SetValue("PhysicsProperty", "ColShape",	"3");
		
		m_akArcheTypes.push_back(pkAt);*/
}





Object* ObjectManager::CloneObject(int iNetID)
{
	Object* pkObjOrginal = GetObjectByNetWorkID(iNetID);
	if(pkObjOrginal == NULL)
		return NULL;

	Object* pkObjClone = new Object;
	pkObjClone->MakeCloneOf(pkObjOrginal);
	return pkObjClone;
}
