#include "object.h"

Object::Object() {
	m_kPos=Vector3(0,0,0);
	m_kRot=Vector3(0,0,0);
}


Object::~Object() {
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		delete (*it);
	}
}




Property* Object::GetProperty(char* acName) {
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if(strcmp((*it)->m_acName,acName)==0) {
			return (*it);
	
		}
	}
	return NULL;
}

void Object::AddProperty(Property* pkNewProperty) {
	pkNewProperty->SetObject(this);
	m_akPropertys.push_back(pkNewProperty);
}

bool Object::RemoveProperty(char* acName) {
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if(strcmp((*it)->m_acName,acName)==0) {
			delete (*it);
			m_akPropertys.erase(it);
			return true;
		}
	}
	return false;
}

void Object::Update(){
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		(*it)->Update();
	}
}

bool Object::Update(char* acName){
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if(strcmp((*it)->m_acName,acName)==0) {
			(*it)->Update();
			return true;
		}
	}
	return false;
}


