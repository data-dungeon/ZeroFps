#include "objectmanager.h"
#include "network.h"
#include "zerofps.h"
#include "netslaveobject.h"

ObjectManager::ObjectManager() 
: ZFObject("ObjectManager") 
{
	iNextObjectID = 0;
	m_bNoUpdate=false;
	
	m_pkWorldObject=new Object();	
	m_pkWorldObject->GetName()="WorldObject";
}

void ObjectManager::Add(Object* pkObject) {
	pkObject->iNetWorkID = iNextObjectID++;
	pkObject->SetObjectMan(this);	
	m_akObjects.push_back(pkObject);
}

void ObjectManager::Clear()
{
//	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
//		Delete(*it);
//	}
	
	m_pkWorldObject->DeleteAllChilds();
	
//	UpdateDelete();
}

void ObjectManager::Delete(Object* pkObject) {
//	pkObject->SetObjectMan(NULL);
	m_akDeleteList.push_back(pkObject);
}


void ObjectManager::Remove(Object* pkObject) {	
//	pkObject->SetObjectMan(NULL);
	m_akObjects.remove(pkObject);
	delete pkObject;
}


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

void ObjectManager::UpdateDelete(){
	if(m_akDeleteList.size()==0)
		return;
	
//	cout<<"ANTAL"<<m_akDeleteList.size()<<endl;
	for(vector<Object*>::iterator it=m_akDeleteList.begin();it!=m_akDeleteList.end();it++) 
	{
		m_akObjects.remove(*it);
		delete (*it);		
	}
	
	m_akDeleteList.clear();
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

Object*	ObjectManager::GetObjectByNetWorkID(int iNetID)
{
	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		if((*it)->iNetWorkID == iNetID)
			return (*it);
	}

	return NULL;
}

Object* ObjectManager::CreateObjectByNetWorkID(int iNetID)
{
	Object *pkNew;
	pkNew = new NetSlaveObject;
	Add(pkNew);
	pkNew->iNetWorkID = iNetID;
	return pkNew;
}

int ObjectManager::GetNumOfObjects()
{
	return m_akObjects.size();
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

void ObjectManager::Update(int iType,int iSide,bool bSort)
{
//	UpdateDelete();

	if(m_bNoUpdate)
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

bool ObjectManager::MakeTemplate(const char* acName,Object* pkObject)
{
	if(GetTemplate(acName)!=NULL)
		return false;
		

	ObjectDescriptor* tempdesc = new ObjectDescriptor;
	
	//set name
	tempdesc->m_kName=acName;
	
	list<Property*> pkPropertys;
	
	pkObject->GetPropertys(&pkPropertys,PROPERTY_TYPE_ALL,PROPERTY_SIDE_ALL);
	
	for(list<Property*>::iterator it=pkPropertys.begin();it!=pkPropertys.end();it++) 
	{
		PropertyDescriptor* pkP=new PropertyDescriptor;
		pkP->m_kName=(*it)->m_acName;
		(*it)->Save(&pkP->m_kData);
		
		tempdesc->m_acPropertyList.push_back(pkP);
	}
	
	AddTemplate(tempdesc);
	return true;
}

Object* ObjectManager::CreateObject(const char* acName)
{	
	Object* tempobject=new Object;
	
	ObjectDescriptor *objtemplate = GetTemplate(acName);
	
	//return null if the template does not exist
	if(objtemplate==NULL)
		return NULL;
	
	tempobject->GetName()=objtemplate->m_kName;
	
	for(list<PropertyDescriptor*>::iterator it=objtemplate->m_acPropertyList.begin();it!=objtemplate->m_acPropertyList.end();it++) 
	{
		if(tempobject->AddProperty((*it)->m_kName.c_str()))
		{
			tempobject->GetProperty((*it)->m_kName.c_str())->Load(&(*it)->m_kData);
		}
	}
	
	return tempobject;
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





