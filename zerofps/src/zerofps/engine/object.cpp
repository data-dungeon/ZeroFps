#include "object.h"
#include "collisionproperty.h"
#include "physicsengine.h"

ObjectDescriptor::~ObjectDescriptor()
{
	for(list<PropertyDescriptor*>::iterator it=m_acPropertyList.begin();it!=m_acPropertyList.end();it++)
	{
		delete (*it);
	}
}

void ObjectDescriptor::Clear()
{
	m_kName="";
	m_kPos.Set(0,0,0);
	m_kRot.Set(0,0,0);
	m_kVel.Set(0,0,0);			
	m_kAcc.Set(0,0,0);				
	
	m_bSave=true;
	
	for(list<PropertyDescriptor*>::iterator it=m_acPropertyList.begin();it!=m_acPropertyList.end();it++)
	{
		delete (*it);
	}
	
	m_acPropertyList.clear();
}


bool ObjectDescriptor::SaveToFile(ZFFile* pkFile)
{
	ZFMemPackage temp;
	
	SaveToMem(&temp);
	
	temp.SaveToFile(pkFile);
	
	temp.Clear();
	
	return true;
}

bool ObjectDescriptor::LoadFromFile(ZFFile* pkFile)
{
	ZFMemPackage temp;
	
	if(!temp.LoadFromFile(pkFile)){
		cout<<"Error Loading from file (EOF)"<<endl;
		return false;
	}
		
	LoadFromMem(&temp);

	temp.Clear();

	return true;
}

/*
void ObjectDescriptor::SaveToMem(ZFMemPackage* pkPackage)
{
	cout<<"saving"<<endl;
	char namn[50];
	strcpy(namn,m_kName.c_str());
	pkPackage->Write((void*)namn,50);	
		
		
	pkPackage->Write(m_kPos);	
	pkPackage->Write(m_kRot);	
	pkPackage->Write(m_kVel);

	
	
//	pkPackage->Write((void*)&m_kPos,12);	
//	pkPackage->Write((void*)&m_kRot,12);	
//	pkPackage->Write((void*)&m_kVel,12);
	
	
	
	int iNrOfPropertys=m_acPropertyList.size();
	pkPackage->Write(iNrOfPropertys);
	
	
	for(list<PropertyDescriptor*>::iterator it=m_acPropertyList.begin();it!=m_acPropertyList.end();it++)
	{
		//write property name
		char propertyname[50];
		strcpy(propertyname,(*it)->m_kName.c_str());
		pkPackage->Write((void*)propertyname,50);
		
		//write size
		int iSize=(*it)->m_kData.GetSize();
		pkPackage->Write(iSize);
		
		//write data
		pkPackage->Write((*it)->m_kData.GetDataPointer(),(*it)->m_kData.GetSize());
	}
}
*/

void ObjectDescriptor::SaveToMem(ZFMemPackage* pkPackage)
{
	cout<<"saving"<<endl;
	char namn[50];
	strcpy(namn,m_kName.c_str());
	pkPackage->Write((void*)namn,50);	
	
	
	pkPackage->Write((void*)&m_kPos,12);	
	pkPackage->Write((void*)&m_kRot,12);	
	pkPackage->Write((void*)&m_kVel,12);
	pkPackage->Write((void*)&m_kAcc,12);
	
	
	pkPackage->Write((void*)&m_bSave,4);
	pkPackage->Write((void*)&m_iObjectType,4);
	
	
	int iNrOfPropertys=m_acPropertyList.size();
//pkPackage->Write(iNrOfPropertys);
	pkPackage->Write((void*)&iNrOfPropertys,4);
	
	for(list<PropertyDescriptor*>::iterator it=m_acPropertyList.begin();it!=m_acPropertyList.end();it++)
	{
		//write property name
		char propertyname[50];
		strcpy(propertyname,(*it)->m_kName.c_str());
		pkPackage->Write((void*)propertyname,50);
		
		//write size
		int iSize=(*it)->m_kData.GetSize();
		pkPackage->Write((void*)&iSize,4);
		
		//write data
		pkPackage->Write((*it)->m_kData.GetDataPointer(),(*it)->m_kData.GetSize());
	}
}

/*
void ObjectDescriptor::LoadFromMem(ZFMemPackage* pkPackage)
{
	cout<<"loading"<<endl;
	char namn[50];
			
	pkPackage->Read((void*)namn,50);
						
	m_kName=namn;
	cout<<"NAMN:"<<namn<<endl;
			
	pkPackage->Read(m_kPos);	
	pkPackage->Read(m_kRot);	
	pkPackage->Read(m_kVel);
			
	int iNrOfPropertys;
	pkPackage->Read(iNrOfPropertys);
			
	cout<<"propertys "<<iNrOfPropertys<<endl;
		
	for(int i=0;i<iNrOfPropertys;i++)
	{
		//create a new propertydescription
		PropertyDescriptor* newpropdesc=new PropertyDescriptor;
				
		//read property name
		char propertyname[50];
		pkPackage->Read((void*)propertyname,50);				
		cout<<"property name "<<propertyname<<endl;
		newpropdesc->m_kName=propertyname;
		
		//read size
		int iSize;
		pkPackage->Read(iSize);
		
		cout<<iSize<<endl;
		
		//read data
		char data;
		for(int i=0;i<iSize;i++)
		{
			pkPackage->Read(data);
			newpropdesc->m_kData.Write(data);
		}
		
//		newpropdesc->m_kData.Write((void*)((char*)pkPackage->GetDataPointer())[pkPackage->GetPos()],iSize);
		
		//add property to list
		m_acPropertyList.push_back(newpropdesc);
	}
}
*/

void ObjectDescriptor::LoadFromMem(ZFMemPackage* pkPackage)
{
	char namn[50];
			
	pkPackage->Read((void*)namn,50);
						
	m_kName=namn;
			
	pkPackage->Read((void*)&m_kPos,12);	
	pkPackage->Read((void*)&m_kRot,12);	
	pkPackage->Read((void*)&m_kVel,12);
	pkPackage->Read((void*)&m_kAcc,12);			
			
	pkPackage->Read((void*)&m_bSave,4);			
	pkPackage->Read((void*)&m_iObjectType,4);	
			
	int iNrOfPropertys;
	pkPackage->Read((void*)&iNrOfPropertys,4);
			
	for(int i=0;i<iNrOfPropertys;i++)
	{
		//create a new propertydescription
		PropertyDescriptor* newpropdesc=new PropertyDescriptor;
				
		//read property name
		char propertyname[50];
		pkPackage->Read((void*)propertyname,50);				
		newpropdesc->m_kName=propertyname;
		
		//read size
		int iSize;
		pkPackage->Read((void*)&iSize,4);
		
		//read data
		char data;
		for(int i=0;i<iSize;i++)
		{
			pkPackage->Read((void*)&data,1);
			newpropdesc->m_kData.Write((void*)&data,1);
		}
		
		//add property to list
		m_acPropertyList.push_back(newpropdesc);
	}
}


Object::Object() {
	m_pkObjectMan = static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkPropertyFactory = static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));	
	
	m_kPos=Vector3(0,0,0);
	m_kRot=Vector3(0,0,0);
	m_kVel=Vector3(0,0,0);
	m_kAcc=Vector3(0,0,0);
	
	m_kName="Object";
		
//	m_iObjectType=OBJECT_TYPE_DYNAMIC;
	m_iObjectType=OBJECT_TYPE_STATIC;	
	
	m_bLockedChilds=false;
	m_iUpdateStatus=UPDATE_ALL;
	m_bLoadChilds=true;
	m_bSave=true;
	
	m_pkParent=NULL;
	m_akChilds.clear();	
}


Object::~Object() {
	if(m_pkParent!=NULL)
		m_pkParent->RemoveChild(this);
	
	DeleteAllChilds();
	
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
	//get this objects propertys
	GetPropertys(akPropertys,iType,iSide);	
	
	switch(m_iUpdateStatus)
	{
		//if UPDATE_NONE dont return any child propertys
		case UPDATE_NONE:
			return;
		
		//update both players and dynamic objects
		case UPDATE_DYNAMIC:{
			for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
				if((*it)->GetObjectType() == OBJECT_TYPE_DYNAMIC || (*it)->GetObjectType() == OBJECT_TYPE_PLAYER)
					(*it)->GetAllPropertys(akPropertys,iType,iSide);
			}
			break;
		}
		case UPDATE_STATIC:{
			for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
				if((*it)->GetObjectType() == OBJECT_TYPE_STATIC)
					(*it)->GetAllPropertys(akPropertys,iType,iSide);
			}			
			break;
		}
		case UPDATE_PLAYERS:{
			for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
				if((*it)->GetObjectType() == OBJECT_TYPE_PLAYER)
					(*it)->GetAllPropertys(akPropertys,iType,iSide);
			}			
			break;
		}			
		case UPDATE_ALL:{
			for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
				(*it)->GetAllPropertys(akPropertys,iType,iSide);
			}			
			break;
		}
	}
}



void  Object::GetPropertys(list<Property*> *akPropertys,int iType,int iSide)
{
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it)->m_iType == iType || iType == PROPERTY_TYPE_ALL){
			if((*it)->m_iSide == iSide || iSide == PROPERTY_SIDE_ALL){
				akPropertys->push_back((*it));			
			}
		}	
	}
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


bool Object::DeleteProperty(const char* acName) {
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if(strcmp((*it)->m_acName,acName)==0) {
			delete (*it);
			m_akPropertys.erase(it);
			return true;
		}
	}
	return false;
}

void Object::RemoveProperty(Property* pkProp) {
	m_akPropertys.remove(pkProp);
	
/*	
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it) == pkProp) {
			m_akPropertys.erase(it);
			return true;
		}
	}
	return false;
*/
}


bool Object::Update(const char* acName){
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if(strcmp((*it)->m_acName,acName)==0) {
			(*it)->Update();
			return true;
		}
	}
	return false;
}

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
	pakObjects->push_back(this);	
	
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		(*it)->GetAllObjects(pakObjects);
	}	
}

void Object::AttachToClosestZone()
{
	list<Object*> temp;
	float mindistance=999999999;
	Object* minobject=m_pkObjectMan->GetWorldObject();

	m_pkObjectMan->GetWorldObject()->GetAllObjects(&temp);

	for(list<Object*>::iterator it=temp.begin();it!=temp.end();it++) {
		if((*it)->GetName()=="ZoneObject"){
			//dont attach this object to this object ;)
			if((*it)==this)
				continue;
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

void Object::Save(ObjectDescriptor* ObjDesc)
{
	ObjDesc->Clear();
	
	//set name
	ObjDesc->m_kName=GetName();	
	
	ObjDesc->m_kPos=GetPos();
	ObjDesc->m_kRot=GetRot();
	ObjDesc->m_kVel=GetVel();
	
	list<Property*> pkPropertys;
	
	GetPropertys(&pkPropertys,PROPERTY_TYPE_ALL,PROPERTY_SIDE_ALL);
	
	for(list<Property*>::iterator it=pkPropertys.begin();it!=pkPropertys.end();it++) 
	{
		PropertyDescriptor* pkP=new PropertyDescriptor;
		pkP->m_kName=(*it)->m_acName;
		(*it)->Save(&pkP->m_kData);
		
		ObjDesc->m_acPropertyList.push_back(pkP);
	}
	
}


void Object::Touch(CollisionData *pkData)
{
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		(*it)->Touch(pkData);
	}
}



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
