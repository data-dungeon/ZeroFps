#include "objectmanager.h"
#include "network.h"

ObjectManager::ObjectManager() 
: ZFObject("ObjectManager") 
{
	iNextObjectID = 0;
}

void ObjectManager::Add(Object* pkObject) {
	pkObject->iNetWorkID = iNextObjectID++;
	pkObject->SetObjectMan(this);	
	m_akObjects.push_back(pkObject);
}

void ObjectManager::Delete(Object* pkObject) {
	pkObject->SetObjectMan(NULL);
	m_akDeleteList.push_back(pkObject);
}


void ObjectManager::Remove(Object* pkObject) {	
	pkObject->SetObjectMan(NULL);
	m_akObjects.remove(pkObject);
	delete pkObject;
}


void ObjectManager::Update(){
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

void ObjectManager::UpdateState(char* pacData)
{
/*
	While Read ObjectID.
		
*/
}

void ObjectManager::PackToClients()
{
 	NetPacket NP;

	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		if((*it)->NeedToPack()) {
			NP.Write((*it)->iNetWorkID);
			(*it)->PackTo(&NP);
			}
	}

	int iEndOfObject = -1;
	NP.Write(iEndOfObject);


	NetWork* net = static_cast<NetWork*>(g_ZFObjSys.GetObjectPtr("NetWork"));
//	net->SendToAllClients(&NP);
}


