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
//		if((*it)->GetDelete()==true){
//			m_akObjects.remove(*it);
//		}
	}
}

void ObjectManager::UpdateDelete(){
	if(m_akDeleteList.size()==0)
		return;
	
	Object* hora=NULL;
	
//	cout<<"ANTAL"<<m_akDeleteList.size()<<endl;
	for(vector<Object*>::iterator it=m_akDeleteList.begin();it!=m_akDeleteList.end();it++) 
	{
		hora=(*it);
		m_akObjects.remove(*it);
		delete hora;		
	}
	
	m_akDeleteList.clear();
}



