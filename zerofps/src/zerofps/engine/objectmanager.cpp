#include "objectmanager.h"

void ObjectManager::Add(Object* pkObject) {
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
		(*it)->Update();
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



