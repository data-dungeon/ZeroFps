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
	
	if(bSort){
		m_akPropertys.sort(Less_Property);	
	}
	
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		(*it)->Update();
	}
}











