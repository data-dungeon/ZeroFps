#include "objectmanager.h"

void ObjectManager::Add(Object* pkNewObject) {
	m_akObjects.push_back(pkNewObject);
}

void ObjectManager::Remove(Object* pkObject) {
	m_akObjects.remove(pkObject);
}

void ObjectManager::Update(){
	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		(*it)->Update();
	}
}




