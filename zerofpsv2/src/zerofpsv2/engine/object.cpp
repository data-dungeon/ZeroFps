#include "object.h"

#include "../engine_systems/physicsengine/physicsengine.h"
#include "../basic/zfobjectmanger.h"
#include "objectmanager.h"
#include "../engine_systems/propertys/physicproperty.h"
 
typedef list<Object*>::iterator		itListObject;
typedef list<Property*>::iterator	itListProperty;

Object::Object() {
	// Get Ptrs to some usefull objects.
	m_pkObjectMan			= static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
//	m_pkLevelMan			= static_cast<LevelManager*>(g_ZFObjSys.GetObjectPtr("LevelManager"));	
	m_pkPropertyFactory	= static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));	
	m_pkFps					= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
		
	ZFAssert(m_pkObjectMan, "Object::Object(): Failed to find ObjectManger");
	ZFAssert(m_pkPropertyFactory, "Object::Object(): Failed to find PropertyFactory");

	m_pkObjectMan->Add(this);	// Add ourself to objectmanger and get NetID.

	// SetDefault Values.
	m_kPos		= Vector3::ZERO;
	m_kRot		= Vector3::ZERO;
	m_kVel		= Vector3::ZERO;
	m_kAcc		= Vector3::ZERO;
	m_fRadius	= 1;
	
	m_kOldPos	= Vector3::ZERO;
	m_kOldRot	= Vector3::ZERO;	
	
	m_kName		= "Object";	
	m_strType	= "Object";

	m_iNetUpdateFlags = 0;

	m_eRole			= NETROLE_AUTHORITY;
	m_eRemoteRole	= NETROLE_PROXY;

	m_iObjectType			=	OBJECT_TYPE_DYNAMIC;	
	m_iUpdateStatus		=	UPDATE_ALL;
//	m_piDecorationStep 	=	1;//m_pkLevelMan->GetDecorationStepPointer();
	m_bSave					=	true;
	m_pkParent				=	NULL;
	m_akChilds.clear();	
	
	
	m_bRelativeOri	=	true;
	
	SetLocalRotV(Vector3(0,0,0));
	SetLocalPosV(Vector3(0,0,0));
	
	ResetGotData();
	

}

Object::~Object() 
{
	// First we remove all childs
	DeleteAllChilds();

	// Then we remove ourself from our master.
	if(m_pkParent != NULL)
		m_pkParent->RemoveChild(this);
	
	// Remove All our propertys.
	for(itListProperty it = m_akPropertys.begin(); it != m_akPropertys.end(); it++)
		delete (*it);
	
	// Tell object manger that we are no more.
	m_pkObjectMan->Remove(this);
}

bool Object::IsA(string strStringType)
{
	return m_pkObjectMan->IsA(this, strStringType);
}

// Property Mangement Code.
Property* Object::AddProperty(Property* pkNewProperty) 
{
	if(pkNewProperty == NULL)
		return NULL;

	pkNewProperty->SetObject(this);
	m_akPropertys.push_back(pkNewProperty);
	
	pkNewProperty->Init();
	return pkNewProperty;
}

Property* Object::AddProperty(const char* acName)
{
	ZFAssert(acName, "Object::AddProperty(): acName is NULL");
	Property* pProp = m_pkPropertyFactory->CreateProperty(acName);
	
	if(pProp==NULL)
	{
		cout << "Error Property " << acName << " Not Registered" <<endl;
		return false;
	}

	AddProperty(pProp);
	return pProp;
}

void Object::RemoveProperty(Property* pkProp) 
{
	m_akPropertys.remove(pkProp);
}

bool Object::DeleteProperty(const char* acName) 
{
	for(itListProperty it = m_akPropertys.begin(); it != m_akPropertys.end(); it++) {
		if(strcmp((*it)->m_acName,acName)==0) {
			delete (*it);
			m_akPropertys.erase(it);
			return true;
		}
	}

	return false;
}

Property* Object::GetProperty(const char* acName) 
{
	for(itListProperty it = m_akPropertys.begin(); it != m_akPropertys.end(); it++) {
		if(strcmp((*it)->m_acName,acName) == 0) 
			return (*it);
	}

	return NULL;
}

void  Object::GetPropertys(vector<Property*> *akPropertys,int iType,int iSide)
{
	for(itListProperty it = m_akPropertys.begin(); it != m_akPropertys.end(); it++) {
		if((*it)->m_iType & iType || iType == PROPERTY_TYPE_ALL){
			if((*it)->m_iSide & iSide || iSide == PROPERTY_SIDE_ALL){
				akPropertys->push_back((*it));			
			}
		}	

/*		if((*it)->m_iType == iType || iType == PROPERTY_TYPE_ALL){
			if((*it)->m_iSide == iSide || iSide == PROPERTY_SIDE_ALL){
				akPropertys->push_back((*it));			
			}
		}	*/
	}
}

void Object::GetAllPropertys(vector<Property*> *akPropertys,int iType,int iSide)
{
	if(m_iUpdateStatus & UPDATE_NONE)
		return;

//	int iDecStep=m_pkLevelMan->GetDecorationStep();
	int iCurDec=0;

	//get this objects propertys
	GetPropertys(akPropertys,iType,iSide);	
	
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++)
	{	
		if(m_iUpdateStatus & UPDATE_ALL)
		{
			(*it)->GetAllPropertys(akPropertys,iType,iSide);
		} 
		else
		{
			
			switch((*it)->GetObjectType())
			{				
				case OBJECT_TYPE_DYNAMIC:
					if(m_iUpdateStatus & UPDATE_DYNAMIC)		
						(*it)->GetAllPropertys(akPropertys,iType,iSide);
					break;	
		
				case OBJECT_TYPE_STATIC:
					if(m_iUpdateStatus & UPDATE_STATIC)		
						(*it)->GetAllPropertys(akPropertys,iType,iSide);
					break;
			
				case OBJECT_TYPE_PLAYER:
					if(m_iUpdateStatus & UPDATE_PLAYERS)		
						(*it)->GetAllPropertys(akPropertys,iType,iSide);
					break;
		
				case OBJECT_TYPE_STATDYN:
					if(m_iUpdateStatus & UPDATE_STATDYN)		
						(*it)->GetAllPropertys(akPropertys,iType,iSide);
					break;
			
				case OBJECT_TYPE_DECORATION:
					if(m_iUpdateStatus & UPDATE_DECORATION)
					{
						iCurDec++;
						if(iCurDec >= *m_piDecorationStep)
						{
							iCurDec=0;						
							(*it)->GetAllPropertys(akPropertys,iType,iSide);
						}
					}
					break;
			}
		}
	}
}

Property* Object::AddProxyProperty(const char* acName)
{
	Property* pProp = GetProperty(acName);
	if(pProp)
		return pProp;
	AddProperty(acName);
	pProp = GetProperty(acName);
	return pProp;
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


// Child/Parent object mangement.
void Object::AddChild(Object* pkObject) 
{
	// Check so we don't have child already.
	if(HasChild(pkObject))
		return;

	m_akChilds.push_back(pkObject);	
	pkObject->SetParent(this);

/*	if(m_bLockedChilds)
		return;
	//check if this child is already added
	if(HasChild(pkObject))
		return;
	
	//add child to list
	m_akChilds.push_back(pkObject);	
	//tell the child to set its new parent
	pkObject->SetParent(this);*/
}

void Object::RemoveChild(Object* pkObject)
{
	// If we don't have object as a child return.
	if(HasChild(pkObject) == false)
		return;

	m_akChilds.remove(pkObject);	// Remove object as child.
	pkObject->SetParent(NULL);		// Set objects parent to NULL.

/*		if(m_bLockedChilds)
		return;
		
	//remove child from list
	m_akChilds.remove(pkObject);
	//tell child to set its parent to null
	pkObject->SetParent(NULL);*/
}

void Object::SetParent(Object* pkObject) 
{
	// Remove Parent
	if(pkObject == NULL) {
		if(m_pkParent == NULL)
			return;
		
		Object* pkParent = m_pkParent;
		m_pkParent = NULL;
		pkParent->RemoveChild(this);
		return;
		}

	if(m_pkParent == pkObject)		// Dont do anything if this parent is already set
		return;
	SetParent(NULL);				// Remove Parent.
	
	m_pkParent = pkObject;
	m_pkParent->AddChild(this);

/*	//dont have a parent
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
	pkObject->AddChild(this);*/
}

bool Object::HasChild(Object* pkObject)
{
	//check if this object is in the child list
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)==pkObject) return true;
	}
	
	return false;
}


int Object::NrOfChilds()
{
	return m_akChilds.size();
}

void Object::DeleteAllChilds()
{
	Object* pkChildObject;

//	m_bLockedChilds=true;
	
	while(m_akChilds.size()) {
		pkChildObject = (*m_akChilds.begin());

		cout << "Child " << pkChildObject->m_kName << endl; 
		pkChildObject->DeleteAllChilds();
		delete pkChildObject;
		}

/*for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		pkChildObject = (*it);

		cout << "Child " << pkChildObject->m_kName << endl; 
		pkChildObject->DeleteAllChilds();
		delete pkChildObject;
	}*/

	m_akChilds.clear();
//	m_bLockedChilds=false;	
}

void Object::GetAllObjects(vector<Object*> *pakObjects)
{
	pakObjects->push_back(this);	
	
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		(*it)->GetAllObjects(pakObjects);
	}	
}


void Object::AttachToClosestZone()
{
	vector<Object*> temp;
	float mindistance=999999999;
	Object* minobject=m_pkObjectMan->GetWorldObject();

	m_pkObjectMan->GetWorldObject()->GetAllObjects(&temp);

	for(vector<Object*>::iterator it=temp.begin();it!=temp.end();it++) {
		if((*it)->GetName()=="ZoneObject"){
			//dont attach this object to this object ;)
			if((*it)==this)
				continue;
			float distance = abs(((*it)->GetWorldPosV() - m_kPos).Length());
			if(distance<mindistance){
				mindistance=distance;
				minobject=(*it);
			}
		}
	}		
	
	temp.clear();
	
	SetParent(minobject);
}


// NetWork/Demo/Save/Load Code.
/* 
	Returns true if object is one that need to be sent over network.
*/
bool Object::IsNetWork()
{
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it)->bNetwork == true) {
			return true;
		}
	}
	
	return false;
}

/*
	Returns true if this object needs to be sent over network (if anything has changed since last
	net update).
*/
bool Object::NeedToPack()
{
	// We can only send data for object we own.
	if( m_eRole != NETROLE_AUTHORITY)			return false;
	// We only send object that the other side need to know about
	if( m_eRemoteRole	== NETROLE_NONE)	return false;

	int iUpdateFlags = m_iNetUpdateFlags | m_pkObjectMan->m_iForceNetUpdate;

	if(IsNetWork()) {
		for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
			iUpdateFlags |= (*it)->m_iNetUpdateFlags; 
			}
	
		if(iUpdateFlags)
			return true;
		}


/*	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		if((*it)->bNetwork == true) {
			return true;
		}
	}*/

	return false;
}

void Object::PackTo(NetPacket* pkNetPacket)
{
	// Write Object Update Flags.
	pkNetPacket->Write( m_iNetUpdateFlags );

	//	Write Pos, Rotation, radius and name.
	if(m_iNetUpdateFlags & OBJ_NETFLAG_POS)
		pkNetPacket->Write(m_kPos);
	
	if(m_iNetUpdateFlags & OBJ_NETFLAG_ROT)
		pkNetPacket->Write(m_kRot);

	pkNetPacket->Write(m_fRadius);

	pkNetPacket->Write_NetStr(m_kName.c_str());
	g_ZFObjSys.Logf("net", " .Name '%s':", m_kName.c_str() );
	
//	char szPropertyName[256];

	// Write propertys med Propery::bNetwork = true
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
		g_ZFObjSys.Logf("net", " Check '%s': ",(*it)->m_acName );
		if((*it)->bNetwork) {
			(*it)->m_iNetUpdateFlags |= m_pkObjectMan->m_iForceNetUpdate;

			//Property* hora = (*it);
			//strcpy(szPropertyName, (*it)->m_acName);
			if((*it)->m_iNetUpdateFlags) {
				g_ZFObjSys.Logf("net", "Add\n");
				pkNetPacket->Write_NetStr((*it)->m_acName);
				(*it)->PackTo(pkNetPacket);
				}
			else {
				g_ZFObjSys.Logf("net", "Same as last year.\n");
				}
			}
		else 
			g_ZFObjSys.Logf("net", "Dont Add\n");
	}

	pkNetPacket->Write_NetStr("");

	m_iNetUpdateFlags = 0;
}

void Object::PackFrom(NetPacket* pkNetPacket)
{
	int iStart = pkNetPacket->m_iPos;

	Vector3 kVec;
	float	  fFloat;

	pkNetPacket->Read( m_iNetUpdateFlags );
	if(m_iNetUpdateFlags & OBJ_NETFLAG_POS) {
		pkNetPacket->Read(kVec);
		SetWorldPosV(kVec);
		//SetPos(kVec);
		g_ZFObjSys.Logf("net", " .Pos: <%f,%f,%f>", kVec.x,kVec.y,kVec.z);
		}

	if(m_iNetUpdateFlags & OBJ_NETFLAG_ROT) {
		pkNetPacket->Read(kVec);
		SetWorldPosV(kVec);
		SetWorldRotV(kVec);
		g_ZFObjSys.Logf("net", " .Rot: <%f,%f,%f>\n", kVec.x,kVec.y,kVec.z);
		}

	pkNetPacket->Read(fFloat);
	GetRadius() = fFloat;

	char szStr[256];
	pkNetPacket->Read_NetStr(szStr);
	m_kName = szStr;
	g_ZFObjSys.Logf("net", " .Name '%s'\n", m_kName.c_str() );
	g_ZFObjSys.Logf("net", " -Head Size = %d\n",  pkNetPacket->m_iPos - iStart );	

	char szProperty[256];
	pkNetPacket->Read_NetStr(szProperty);

	while(strcmp(szProperty,"") != 0) {
		int iPStart = pkNetPacket->m_iPos;
		Property* pProp  = AddProxyProperty(szProperty);
		if(pProp) {
			g_ZFObjSys.Logf("net", " /%s\n", szProperty);
			pProp->PackFrom(pkNetPacket);
			}
		else {
			cout << "Error in netpacket" << endl;
			pkNetPacket->SetError(true);
			return;
			}

		int iPEnd	= pkNetPacket->m_iPos;
		g_ZFObjSys.Logf("net", " -Size: %d\n", (iPEnd - iPStart) + 4);	// +4 for netstring for property name

		pkNetPacket->Read_NetStr(szProperty);
		}	

	int iEnd = pkNetPacket->m_iPos;
	g_ZFObjSys.Logf("net", " .End Of Propertys size: 4\n");
	g_ZFObjSys.Logf("net", " .Size for Object %d\n",(iEnd - iStart) );
	m_pkObjectMan->m_iTotalNetObjectData += (iEnd - iStart);
	m_pkObjectMan->m_iNumOfNetObjects ++;
	g_ZFObjSys.Logf("net", "\n");
}




void Object::Save(ObjectDescriptor* ObjDesc)
{
	ObjDesc->Clear();
	
	//set name
	ObjDesc->m_kName = GetName();	
	 
	ObjDesc->m_kPos  = GetWorldPosV();
	ObjDesc->m_kRot  = GetWorldRotV();
	ObjDesc->m_kVel  = GetVel();
	ObjDesc->m_kAcc  = GetAcc();	
	ObjDesc->m_fRadius  = GetRadius();		
	
	ObjDesc->m_bSave = m_bSave;
	ObjDesc->m_iObjectType=m_iObjectType;
	
	vector<Property*> pkPropertys;
	
	GetPropertys(&pkPropertys,PROPERTY_TYPE_ALL,PROPERTY_SIDE_ALL);
	
	for(vector<Property*>::iterator it=pkPropertys.begin();it!=pkPropertys.end();it++) 
	{
		if((*it)->m_bSave)
		{
			PropertyDescriptor* pkP=new PropertyDescriptor;
			pkP->m_kName=(*it)->m_acName;
			(*it)->Save(&pkP->m_kData);
		
			ObjDesc->m_acPropertyList.push_back(pkP);
		}
	}
}

// Collision / Shape.
float Object::GetBoundingRadius()
{
	Property* pr=GetProperty("PhysicProperty");
	if(pr==NULL)
		return 1;

	return (static_cast<PhysicProperty*>(pr))->GetBoundingRadius();
}

void Object::Touch(Collision* pkCol)
{
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		(*it)->Touch(pkCol);
	}
}


// Game Messages
/// Adds a GameMessage to be handled later by object.
void Object::AddGameMessage(GameMessage& Msg)
{
	m_kGameMessages.push_back(Msg);
}

void Object::HandleMessages()
{
	GameMessage Msg;

	for(unsigned int i=0; i<m_kGameMessages.size(); i++) {
		Msg = m_kGameMessages[i];
		RouteMessage(Msg);
		}

	m_kGameMessages.clear();
}

/// Send a GameMessage to all object propertys.
void Object::RouteMessage(GameMessage& Msg)
{
	for(list<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
		(*it)->HandleGameMessage(Msg);
}




// Debug
bool Object::CheckLinks(bool bCheckChilds, int iPos)
{
	// Check that our parent link to us.
	if(m_pkParent) {
		if(m_pkParent->HasChild(this) == false) {
			return false;
			}
		}
	
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)->m_pkParent != this)
			return false;
	}

	return true;
}

void TabIn(int iSpaces)
{
	int i;
	for(i=0;i<iSpaces;i++)	
		g_ZFObjSys.Logf("fisklins", " " );
}

void Object::PrintTree(int pos)
{
	const char* szValue;
	string strValue;

	g_ZFObjSys.Logf("fisklins", "" );

	TabIn(pos);

	TabIn(pos);			g_ZFObjSys.Logf("fisklins", "Object[%d]\n", iNetWorkID);
	TabIn(pos);			g_ZFObjSys.Logf("fisklins", "{\n" );

	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Name = %s\n", GetName().c_str() );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "ObjType = %s\n", m_strType.c_str() );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Pos = <%f,%f,%f>\n", m_kPos.x, m_kPos.y, m_kPos.z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Rot = <%f,%f,%f>\n", m_kRot.x, m_kRot.y, m_kRot.z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Vel = <%f,%f,%f>\n", m_kVel.x, m_kVel.y, m_kVel.z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "m_kAcc = <%f,%f,%f>\n", m_kAcc.x, m_kAcc.y, m_kAcc.z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Name = %s\n", m_strType.c_str() );

	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Type = %d\n", m_iObjectType );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "UpdateStatus = %d\n", m_iUpdateStatus );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Save = %d\n", m_bSave );

	vector<string> akPropertyNames;
	
	for(itListProperty it = m_akPropertys.begin(); it != m_akPropertys.end(); it++) {
		TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "%s\n" ,(*it)->m_acName);
		akPropertyNames = (*it)->GetValueNames();

		for(unsigned int i=0; i < akPropertyNames.size(); i++) {
			strValue = (*it)->GetValue(akPropertyNames[i]);
			szValue = strValue.c_str();

			TabIn(pos + 6);
				g_ZFObjSys.Logf("fisklins", "%s = ",akPropertyNames[i].c_str());
				g_ZFObjSys.Logf("fisklins", "%s\n",szValue );
			}
		}

	for(list<Object*>::iterator it2=m_akChilds.begin();it2!=m_akChilds.end();it2++) {
		(*it2)->PrintTree(pos+1);
	}

	TabIn(pos);	g_ZFObjSys.Logf("fisklins", "}\n" );
}

void Object::MakeCloneOf(Object* pkOrginal)
{
	SetParent(m_pkParent);

	m_kPos		= pkOrginal->m_kPos;
	m_kRot		= pkOrginal->m_kRot;
	m_kVel		= pkOrginal->m_kVel;
	m_kOldPos	= pkOrginal->m_kOldPos;
	m_kOldRot	= pkOrginal->m_kOldRot;
	m_kName		= pkOrginal->m_kName;
	m_strType	= pkOrginal->m_strType;
	m_iObjectType			= pkOrginal->m_iObjectType;
	m_iUpdateStatus		= pkOrginal->m_iUpdateStatus;
	m_piDecorationStep	= pkOrginal->m_piDecorationStep;
	m_bSave		= pkOrginal->m_bSave;
	m_kAcc		= pkOrginal->m_kAcc;
	m_fRadius	= pkOrginal->m_fRadius;	

	Property* pkProp;
	vector<string> akPropertyNames;

	for(itListProperty it = pkOrginal->m_akPropertys.begin(); it != pkOrginal->m_akPropertys.end(); it++) {
		pkProp = AddProperty((*it)->m_acName);
		cout << "Creating '" << (*it)->m_acName << "'\n";
		
		// Get Values
		akPropertyNames = (*it)->GetValueNames();

		for(unsigned int i=0; i < akPropertyNames.size(); i++) {
			pkProp->SetValue(akPropertyNames[i], (*it)->GetValue(akPropertyNames[i]));
			cout << " Setting '" << akPropertyNames[i] << "' to '" << (*it)->GetValue(akPropertyNames[i]) << "'\n";
			}
		
		// *pkProp = (*it);
		}
	
	
}

/*
float Object::GetI()
{
	float t = m_pkFps->GetGameFrameTime(); 
	float at = m_pkFps->GetTicks() - m_pkFps->GetLastGameUpdateTime();
	float i = at/t;				
	
	if(i>1)
		i=1;
	
	if(i<0)
		i=0;
		
//	cout<<"I"<<i<<endl;
		
	return i;	
}

Vector3 Object::GetIRot()
{
//	float i = GetI();					
	float t = m_pkFps->GetGameFrameTime(); 
	float at = m_pkFps->GetTicks() - m_fLastRotSetTime;
	float i = at/t;				
	
	if(i>1)
		i=1;	
	if(i<0)
		i=0;

	Vector3 res;	
	
	res.x = (m_kOldRot.x * (1-i)) + (m_kRot.x * i);
	res.y = (m_kOldRot.y * (1-i)) + (m_kRot.y * i);	
	res.z = (m_kOldRot.z * (1-i)) + (m_kRot.z * i);	
		
	return res;
}

Vector3 Object::GetIPos()
{
//	float i = GetI();
	float t = m_pkFps->GetGameFrameTime(); 
	float at = m_pkFps->GetTicks() - m_fLastPosSetTime;
	float i = at/t;				
	
	if(i>1)
		i=1;	
	if(i<0)
		i=0;


	Vector3 res;
	res.Lerp(m_kOldPos,m_kPos,i);
	return res;
}

void Object::SetRot(Vector3 kRot)
{
	if(kRot != m_kRot) {
		m_iNetUpdateFlags |= OBJ_NETFLAG_ROT;
		//cout << "SetRot changed " << endl;
		}

	m_kOldRot = m_kRot;
	m_kRot = kRot;
	
	m_fLastRotSetTime = m_pkObjectMan->m_pkZeroFps->GetEngineTime();	
}

void Object::SetPos(Vector3 kPos)
{
	if(kPos != m_kPos) {
		m_iNetUpdateFlags |= OBJ_NETFLAG_POS;
		//cout << "SetPos changed " << endl;
		}

	m_kOldPos = m_kPos;
	m_kPos = kPos;
	
	m_fLastPosSetTime = m_pkObjectMan->m_pkZeroFps->GetEngineTime();
}
*/
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
	m_fRadius=0.5;
	
	m_iObjectType=OBJECT_TYPE_DYNAMIC;
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
	pkPackage->Write((void*)&m_fRadius,4);
	
	
	pkPackage->Write((void*)&m_bSave,4);
	pkPackage->Write((void*)&m_iObjectType,4);
	
	int iNrOfPropertys=m_acPropertyList.size();
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

void ObjectDescriptor::LoadFromMem(ZFMemPackage* pkPackage)
{
	char namn[50];
			
	pkPackage->Read((void*)namn,50);
						
	m_kName=namn;
			
	pkPackage->Read((void*)&m_kPos,12);	
	pkPackage->Read((void*)&m_kRot,12);	
	pkPackage->Read((void*)&m_kVel,12);
	pkPackage->Read((void*)&m_kAcc,12);			
	pkPackage->Read((void*)&m_fRadius,4);
			
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

void Object::ResetChildsGotData()
{
	ResetGotData();
	
	for(list<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)->GetRelativeOri())
			(*it)->ResetChildsGotData();	
	}	
}

void Object::SetLocalRotM(Matrix4 kNewRot)
{
	ResetChildsGotData();
	
	m_kLocalRotM = kNewRot;
}

void Object::SetLocalRotV(Vector3 kRot)
{
	ResetChildsGotData();
	
	m_kLocalRotM.Identity();
	m_kLocalRotM.Rotate(kRot);
	
}

void Object::SetWorldRotV(Vector3 kRot)
{
	Vector3 crot = GetWorldRotV();
	Vector3 	newlocal = GetLocalRotV();	
	Vector3 diff;
	
	kRot.x = Clamp(kRot.x,0,360);
	kRot.y = Clamp(kRot.y,0,360);
	kRot.z = Clamp(kRot.z,0,360);

	diff.x = kRot.x - crot.x;
	diff.y = kRot.y - crot.y;	
	diff.z = kRot.z - crot.z;	
	
	diff.x = Clamp(diff.x,0,360);
	diff.y = Clamp(diff.y,0,360);
	diff.z = Clamp(diff.z,0,360);
	
	newlocal.x += diff.x;
	newlocal.y += diff.y;	
	newlocal.z += diff.z;	
	
	SetLocalRotV(newlocal);
}

void Object::SetLocalPosV(Vector3 kPos)
{
	ResetChildsGotData();
	
	m_kLocalPosV = kPos;
}

void Object::SetWorldPosV(Vector3 kPos)
{
	Vector3 kDiff = kPos - GetWorldPosV();
	Vector3 newlocalpos = m_kLocalPosV + kDiff;
	
	SetLocalPosV(newlocalpos);
}

Vector3 Object::GetLocalRotV()
{
	if(!m_kGotData[LOCAL_ROT_V])
	{
		m_kLocalRotV = m_kLocalRotM.GetRotVector();
		m_kGotData[LOCAL_ROT_V] = true;
	}

	return m_kLocalRotV;
}

Vector3 Object::GetLocalPosV()
{	
	return m_kLocalPosV;
}

Matrix4 Object::GetLocalRotM()
{
	return m_kLocalRotM;
}

Vector3 Object::GetWorldPosV()
{
	if(!m_kGotData[WORLD_POS_V])
	{
		if(m_bRelativeOri)
		{
			//check if we have a parent else use local pos
			if(m_pkParent)
			{
				m_kWorldPosV  = GetWorldOriM().GetPosVector();
				m_kGotData[WORLD_POS_V] = true;
			}
			else
			{
				m_kWorldPosV = m_kLocalPosV;
				m_kGotData[WORLD_POS_V] = true;			
			}
		
		}
		else
		{
			m_kWorldPosV = m_kLocalPosV;
			m_kGotData[WORLD_POS_V] = true;
		}
	}
	
	return m_kWorldPosV;
}

Matrix4 Object::GetWorldRotM()
{
	if(!m_kGotData[WORLD_ROT_M])
	{	
		if(m_bRelativeOri)
		{
			//check if we have a parent else use local pos
			if(m_pkParent)
			{
				m_kWorldRotM  = m_kLocalRotM * m_pkParent->GetWorldRotM() ;
				m_kGotData[WORLD_ROT_M] = true;
			}
			else
			{
				m_kWorldRotM = m_kLocalRotM;
				m_kGotData[WORLD_ROT_M] = true;			
			}
		
		}
		else
		{
			m_kWorldRotM = m_kLocalRotM;
			m_kGotData[WORLD_ROT_M] = true;
		}
		
	}
	
	return m_kWorldRotM;
}

Vector3 Object::GetWorldRotV()
{
	if(!m_kGotData[WORLD_ROT_V])
	{	
		m_kWorldRotV = GetWorldRotM().GetRotVector();
		m_kGotData[WORLD_ROT_V] = true;
	}

	return m_kWorldRotV;
}

Matrix4 Object::GetWorldOriM()
{
	if(!m_kGotData[WORLD_ORI_M])
	{	
		if(m_bRelativeOri)
		{
			if(m_pkParent)
			{			
				m_kWorldOriM = GetLocalOriM() *m_pkParent->GetWorldOriM() ;
				m_kGotData[WORLD_ORI_M] = true;						
			}
			else
			{
				m_kWorldOriM = GetLocalOriM();
				
				m_kGotData[WORLD_ORI_M] = true;			
			}		
		}
		else
		{
			m_kWorldOriM = GetLocalOriM();
			
			m_kGotData[WORLD_ORI_M] = true;
		}	
	}
	
	return m_kWorldOriM;

}

Matrix4 Object::GetLocalOriM()
{
	if(!m_kGotData[LOCAL_ORI_M])
	{	
		m_kLocalOriM = m_kLocalRotM;
		m_kLocalOriM.Translate(m_kLocalPosV);
	}
	
	return m_kLocalOriM;
}

