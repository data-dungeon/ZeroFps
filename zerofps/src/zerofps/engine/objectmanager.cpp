#include "objectmanager.h"
#include "network.h"
#include "zerofps.h"
#include "netslaveobject.h"

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







ObjectManager::ObjectManager() 
: ZFObject("ObjectManager") 
{
	iNextObjectID	= 0;
	m_bUpdate		= true;
	
	m_pkWorldObject	=	new Object();	
	m_pkWorldObject->GetName() = "WorldObject";
}

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


void ObjectManager::Update(int iType,int iSide,bool bSort)
{
//	UpdateDelete();
	if(!m_bUpdate)
		if(iType!=PROPERTY_TYPE_RENDER)
			return;

	GetPropertys(iType,iSide);
	
	m_iNrOfActivePropertys = m_akPropertys.size();
	
	if(bSort){
		m_akPropertys.sort(Less_Property);	
	}
	
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
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
	tempobject->GetPos()=pkObjDesc->m_kPos;
	tempobject->GetRot()=pkObjDesc->m_kRot;
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
	Object *pkNew;
	pkNew = new NetSlaveObject;
	Add(pkNew);
	pkNew->iNetWorkID = iNetID;
	return pkNew;
}

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




void ObjectManager::UpdateState(NetPacket* pkNetPacket)
{
	cout << "ObjectManager::UpdateState" << endl;

	Object* pkNetSlave;
	int iObjectID;
	pkNetPacket->Read(iObjectID);
	while(iObjectID != -1) {
		pkNetSlave = GetObjectByNetWorkID(iObjectID);
		if(pkNetSlave == NULL)
			pkNetSlave = CreateObjectByNetWorkID(iObjectID);

		if(pkNetSlave == NULL) {
			cout << "GAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << endl; 
			}

		pkNetSlave->PackFrom(pkNetPacket);
		pkNetPacket->Read(iObjectID);
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
			(*it)->PackTo(&NP);
			}

		iPacketSize++;

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

void ObjectManager::DisplayTree()
{
	m_pkWorldObject->PrintTree(0);
}










//vector<ObjectTemplate*>	m_ObjectTemplates;
void ObjectManager::Create_OT(int iID)
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









