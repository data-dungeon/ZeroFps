#include "object.h"

Object::Object() {
	m_bStatic=false;
	m_pkObjectMan=NULL;
	m_kPos=Vector3(0,0,0);
	m_kRot=Vector3(0,0,0);
	m_kVel=Vector3(0,0,0);
	m_iType=0;
}


Object::~Object() {
//	cout<<"OBJECT DIES"<<endl;

	//if the object is in a objectmanager then tell the objectmanager
	//to remove it before we delete it
//	if(m_pkObjectMan!=NULL)
//		m_pkObjectMan->Remove(this);
	
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
	//apply the velocity vector to the position vector
	m_kPos+=m_kVel;

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

void Object::Remove() {
	if(m_pkObjectMan!=NULL) {
		m_pkObjectMan->Delete(this);	
	}
}

void Object::HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue){
//	cout<<"This Object Has not Collision handler"<<endl;
	if(bContinue)
		pkObject->HandleCollision(this,kPos,false);
		
		
		
	
/*			EXAMPLE of how the collisionhandler may look like in a subclass of Object
			

	//if a ball colides with the player destroy it
	if(typeid(*pkOther)==typeid(PlayerBallObject)){
		PlayerBallObject *kO=dynamic_cast<PlayerBallObject*>(pkOther);
		Remove();	
		
	} else if(bContinue){
		pkOther->HandleCollision(this,false);
	}
*/
}






