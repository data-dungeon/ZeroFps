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

void Object::AddProperty(char* acName)
{
/*	Property* pProp = m_pkPropFactory->CreateProperty(acName);
	if(!pProp)
		return;*/

	PropertyFactory* pkPropFactory = static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));
	Property* pProp = pkPropFactory->CreateProperty(acName);
	

	AddProperty(pProp);
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

	//if the object is static then only update those propertys that are static propertys
	if(m_bStatic){
		Update(PROPERTY_TYPE_STATIC);
		return;
	}

	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		(*it)->Update();
	}
}

void Object::Update(int iType){
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it)->GetType() == iType){
			(*it)->Update();
		}
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

/*
void Object::ObjectUpdate() {
	
}

*/

void Object::HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue){
//	cout<<"This Object Has not Collision handler"<<endl;
	if(bContinue)
		pkObject->HandleCollision(this,kPos,false);

/*	EXAMPLE of how the collisionhandler may look like in a subclass of Object
			

	//if a ball colides with the player destroy it
	if(typeid(*pkOther)==typeid(PlayerBallObject)){
		PlayerBallObject *kO=dynamic_cast<PlayerBallObject*>(pkOther);
		Remove();	
		
	} else if(bContinue){
		pkOther->HandleCollision(this,false);
	}
*/
}


bool Object::Save(void *pkData,int iSize) {
	iSize=0;
	pkData=NULL;
	
	return false;
}

/*
 PackTo och PackFrom används för att spara ned / upp object till/från
 network. 

 Format:
	Vector3 m_kPos;
	Vector3 m_kRot;
	Properys[]
	 "name"
	 propery_data

  För närvarande sparas allt ned hela tiden. Vilka propertys som tas med
  anges genom Propery::bNetwork;
*/


void Object::PackTo(NetPacket* pkNetPacket)
{
	//	Write Pos, Rotation.	
	pkNetPacket->Write(m_kPos);
	pkNetPacket->Write(m_kRot);

	// Loop all properys med Propery::bNetwork = true
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it)->bNetwork) {
			pkNetPacket->Write_Str((*it)->m_acName);
			(*it)->PackTo(pkNetPacket);
			}
	}
}

void Object::PackFrom(NetPacket* pkNetPacket)
{
/*
	Read Pos, Rotation.
	Så länge Read Propery Str.
		Get Prop Ptr eller skapa om inte finns.
		property->PackFrom(pacData)
*/
}

bool Object::NeedToPack()
{
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it)->bNetwork == true) {
			return true;
		}
	}

	return false;
}



