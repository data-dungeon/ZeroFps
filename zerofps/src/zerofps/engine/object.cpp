#include "object.h"
#include "collisionproperty.h"

Object::Object() {
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkPropertyFactory = static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));	
	
	m_kPos=Vector3(0,0,0);
	m_kRot=Vector3(0,0,0);
	m_kVel=Vector3(0,0,0);

	m_kName="Object";
		
	m_bLockedChilds=false;
	m_iUpdateStatus=UPDATE_ALL;
	m_bLoadChilds=true;
	
	m_pkParent=NULL;
	m_akChilds.clear();	
}


Object::~Object() {
	if(m_pkParent!=NULL)
		m_pkParent->RemoveChild(this);
	
	DeleteAllChilds();
/*	
	m_bLockedChilds=true;
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
//		m_pkObjectMan->Delete(*it);
//		m_pkObjectMan->Remove(*it);
//		delete (*it);
		delete (*it);		
	}
	m_bLockedChilds=false;
*/	
	
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		delete (*it);
	}
	
	cout<<"Object Destroyed"<<endl;
}

Property* Object::GetProperty(const char* acName) {
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if(strcmp((*it)->m_acName,acName)==0) {
			return (*it);
	
		}
	}
	return NULL;
}


void Object::GetAllPropertys(list<Property*> *akPropertys,int iType,int iSide)
{
	
	//first get propertys from all childs
	if(m_iUpdateStatus != UPDATE_NONE ){
		for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
			(*it)->GetAllPropertys(akPropertys,iType,iSide);
		}			
	}
	
	GetPropertys(akPropertys,iType,iSide);
/*	
	//then get this objects propertys
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it)->m_iType == iType || iType == PROPERTY_TYPE_ALL){
			if((*it)->m_iSide == iSide || iSide == PROPERTY_SIDE_ALL){
				akPropertys->push_back((*it));			
			}
		}	
	}*/
}



void  Object::GetPropertys(list<Property*> *akPropertys,int iType,int iSide)
{
//	int iNrOProps=0;
	
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it)->m_iType == iType || iType == PROPERTY_TYPE_ALL){
			if((*it)->m_iSide == iSide || iSide == PROPERTY_SIDE_ALL){
				akPropertys->push_back((*it));			
//				cout<<"NAME "<<(*it)->m_acName<<endl;
//				iNrOProps++;
			}
		}	
	}
//	return iNrOProps;
}


bool Object::AddProperty(Property* pkNewProperty) {
	if(pkNewProperty==NULL)
		return false;

	pkNewProperty->SetObject(this);
	m_akPropertys.push_back(pkNewProperty);
	return true;
}

bool Object::AddProperty(const char* acName)
{
/*	Property* pProp = m_pkPropFactory->CreateProperty(acName);
	if(!pProp)
		return;*/


	Property* pProp = m_pkPropertyFactory->CreateProperty(acName);
	
	if(pProp==NULL)
	{
		cout<<"Error Property "<<acName<<" Not Registered"<<endl;
		return false;
	}


	AddProperty(pProp);
	return true;
}

Property* Object::AddProxyProperty(char* acName)
{
	Property* pProp = GetProperty(acName);
	if(pProp)
		return pProp;
	AddProperty(acName);
	pProp = GetProperty(acName);
	return pProp;
}


bool Object::RemoveProperty(const char* acName) {
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if(strcmp((*it)->m_acName,acName)==0) {
			delete (*it);
			m_akPropertys.erase(it);
			return true;
		}
	}
	return false;
}

/*
void Object::Update(){

	//if the object is static then only update those propertys that are static propertys
//	if(m_bStatic){
//		Update(PROPERTY_TYPE_STATIC);
//		return;
//	}

	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		(*it)->Update();
	}
}
*/

/*
void Object::Update(int iType,int iSide){

	//first update all childs
	if(m_bUpdateChilds){
		for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
			(*it)->Update(iType,iSide);
		}		
	}	
	
	//thenupdate propertys
	for(list<Property*>::iterator it2=m_akPropertys.begin();it2!=m_akPropertys.end();it2++) {
		if((*it2)->m_iType == iType){
			if((*it2)->m_iSide == iSide){
				(*it2)->Update();
			}
		}
	}
}
*/

bool Object::Update(const char* acName){
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if(strcmp((*it)->m_acName,acName)==0) {
			(*it)->Update();
			return true;
		}
	}
	return false;
}

/*
void Object::Remove() {
	if(m_pkObjectMan!=NULL) {
		m_pkObjectMan->Delete(this);	
	}
}
*/

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

	char szPropertyName[256];

	// Loop all properys med Propery::bNetwork = true
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it)->bNetwork) {
			cout << "Object::PackTo : " << (*it)->m_acName << endl;
			strcpy(szPropertyName, (*it)->m_acName);
			pkNetPacket->Write_Str((*it)->m_acName);
			Property* hora = (*it);
			(*it)->PackTo(pkNetPacket);
			}
	}

	pkNetPacket->Write_Str("");
}

void Object::PackFrom(NetPacket* pkNetPacket)
{
	pkNetPacket->Read(m_kPos);
	pkNetPacket->Read(m_kRot);

	char szProperty[256];

	pkNetPacket->Read_Str(szProperty);
	while(strcmp(szProperty,"") != 0) {
		Property* pProp  = AddProxyProperty(szProperty);
		pProp->PackFrom(pkNetPacket);
	
		pkNetPacket->Read_Str(szProperty);
		}	
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


void Object::AddChild(Object* pkObject) 
{
	if(m_bLockedChilds)
		return;
	//check if this child is already added
	if(HasChild(pkObject))
		return;
	
	//add child to list
	m_akChilds.push_back(pkObject);	
	//tell the child to set its new parent
	pkObject->SetParent(this);
	
}

void Object::RemoveChild(Object* pkObject)
{
	if(m_bLockedChilds)
		return;
		
	//remove child from list
	m_akChilds.remove(pkObject);
	//tell child to set its parent to null
	pkObject->SetParent(NULL);
}

void Object::SetParent(Object* pkObject) 
{
	//dont have a parent
	if(pkObject==NULL){
		m_pkParent=NULL;
		return;
	}
	
	//dont do anything if this parent is already set
	if(m_pkParent==pkObject)
		return;
	
	//tell the curent parent to remove this child
	if(m_pkParent!=NULL){
		m_pkParent->RemoveChild(this);	
	}
	
	//set parent
	m_pkParent=pkObject;
	//tell parent to add this child
	pkObject->AddChild(this);
}

bool Object::HasChild(Object* pkObject)
{
	//check if this object is in the child list
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)==pkObject) return true;
	}
	
	return false;
}


float Object::GetBoundingRadius()
{
	Property* pr=GetProperty("CollisionProperty");
	if(pr==NULL)
		return 1;

	return (static_cast<CollisionProperty*>(pr))->GetBoundingRadius();
}

int Object::NrOfChilds()
{
	return m_akChilds.size();
}

void Object::DeleteAllChilds()
{
	m_bLockedChilds=true;
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		delete (*it);
	}

	m_akChilds.clear();
	m_bLockedChilds=false;	
}

void Object::PrintTree(int pos)
{
	for(int i=0;i<pos;i++)
		cout<<" ";
	cout<<m_kName<<endl;

	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		(*it)->PrintTree(pos+1);
	}
}

void Object::GetAllObjects(list<Object*> *pakObjects)
{
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		(*it)->GetAllObjects(pakObjects);
	}	

	pakObjects->push_back(this);
}

void Object::AttachToClosestZone()
{
	list<Object*> temp;
	float mindistance=999999999;
	Object* minobject=m_pkObjectMan->GetWorldObject();

	
	m_pkObjectMan->GetWorldObject()->GetAllObjects(&temp);
	

	for(list<Object*>::iterator it=temp.begin();it!=temp.end();it++) {
		if((*it)->GetName()=="ZoneObject"){
			float distance = abs(((*it)->GetPos() - m_kPos).Length());
			if(distance<mindistance){
				mindistance=distance;
				minobject=(*it);
			}
		}
	}		
	
	temp.clear();
	
	SetParent(minobject);
}

