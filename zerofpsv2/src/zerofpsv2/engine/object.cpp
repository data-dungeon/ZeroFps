#include "object.h"

#include "../engine_systems/physicsengine/physicsengine.h"
#include "../basic/zfsystem.h"
#include "objectmanager.h"
#include "../engine_systems/propertys/physicproperty.h"
 
typedef list<Object*>::iterator		itListObject;
typedef list<Property*>::iterator	itListProperty;

Object::Object() 
{
	// Get Ptrs to some usefull objects.
	m_pkObjectMan			= static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));
	m_pkPropertyFactory	= static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));	
	m_pkFps					= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
		
	ZFAssert(m_pkObjectMan,			"Object::Object(): Failed to find ObjectManger");
	ZFAssert(m_pkPropertyFactory, "Object::Object(): Failed to find PropertyFactory");

	m_pkObjectMan->Add(this);	// Add ourself to objectmanger and get NetID.

	// SetDefault Values.
	m_kLocalRotM.Identity();
	m_kLocalPosV= Vector3::ZERO;	
	m_kVel		= Vector3::ZERO;
	m_kAcc		= Vector3::ZERO;
	m_fRadius	= 1;
	
	m_strName	= "A Object";	
	m_strType	= "Object";

	m_pScriptFileHandle	= new ZFResourceHandle;

	m_iNetUpdateFlags		= 0;

	m_eRole					= NETROLE_AUTHORITY;
	m_eRemoteRole			= NETROLE_PROXY;

	m_iObjectType			=	OBJECT_TYPE_DYNAMIC;	
	m_iUpdateStatus		=	UPDATE_ALL;
	m_bZone					=	false;
	m_iCurrentZone			=	-1;
	m_bUseZones				=	false;
	m_bSave					=	true;	
	m_pkParent				=	NULL;
	m_akChilds.clear();	
	
	m_bRelativeOri	=	false;
	
	ResetGotData();
}

Object::~Object() 
{
	// First we remove all childs
	DeleteAllChilds();

	// Then we remove ourself from our master.
	if(m_pkParent != NULL)
		m_pkParent->RemoveChild(this);
	
	while(!m_akPropertys.empty())
	{
		Property* TempProperty = m_akPropertys.back();
		m_akPropertys.pop_back();
		delete TempProperty;
	}
	// Tell object manger that we are no more.
	m_pkObjectMan->Remove(this);
	
	delete(m_pScriptFileHandle);
//	m_pkFps->Logf("zerofps", "Object Deleted %d", iNetWorkID);
}

bool Object::IsA(string strStringType)
{
	return m_pkObjectMan->IsA(this, strStringType);
}

/**	\brief	Adds a property to the object.
*/
Property* Object::AddProperty(Property* pkNewProperty) 
{
	if(pkNewProperty == NULL)
		return NULL;
	
	///EVIL GUBB/////
	vector<Property*>::iterator kIt = m_akPropertys.begin();
	while(kIt != m_akPropertys.end())
	{
		(*kIt)->PropertyFound(pkNewProperty);
		++kIt;
	}
	/////////////////7
	pkNewProperty->SetObject(this);
	m_akPropertys.push_back(pkNewProperty);
	
	pkNewProperty->Init();
	return pkNewProperty;
}

/**	\brief	Creates and adds a property to the object.
*/
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

/**	\brief	Removes the property from the object.
*/
void Object::RemoveProperty(Property* pkProp) 
{
	//m_akPropertys.remove(pkProp);
	
	///EVIL GUBB/////
	vector<Property*>::iterator kIt = m_akPropertys.begin();
	while(kIt != m_akPropertys.end())
	{
		if((*kIt) == pkProp)
		{
			(*kIt) = m_akPropertys.back();
			m_akPropertys.pop_back();
			kIt = m_akPropertys.end();
		}
		++kIt;
	}

	PropertyLost(pkProp);
}

/**	\brief	Removes and deletes the property. 
	
	Removes a property from the object and then deletes it,
*/
bool Object::DeleteProperty(const char* acName) 
{
	if(m_akPropertys.empty())
		return false;
	
	vector<Property*>::iterator kIt = m_akPropertys.begin();
	while(kIt != m_akPropertys.end())
	{
		if(strcmp((*kIt)->m_acName,acName)==0)
		{
			Property* TempProp = (*kIt);
			(*kIt) = m_akPropertys.back();
			m_akPropertys.pop_back();
			delete (TempProp);
			
			return true;
		}
		++kIt;
	}
	
	return false;
}

void Object::PropertyLost(Property* pkProp)
{
	if(pkProp->m_pkObject == this)
	{
		vector<Property*>::iterator kIt = m_akPropertys.begin();
		while(kIt != m_akPropertys.end())
		{
			(*kIt)->PropertyLost(pkProp);
			++kIt;
		}
		pkProp->m_pkObject = 0;
	}	
}


/**	\brief	Returns a pointer to the choosen property.
	
	Returns a pointer the the property 'name' or NULL if the property is not part of the object.
*/
Property* Object::GetProperty(const char* acName) 
{
	vector<Property*>::iterator kIt = m_akPropertys.begin();
	while(kIt != m_akPropertys.end())
	{
		if(strcmp((*kIt)->m_acName,acName)==0)
		{
			return (*kIt);
		}
		++kIt;
	}

	return NULL;
}

/**	\brief	Returns a vector with propertys.
	
	Returns a vector with the the propertys that have the correct type and side flags.
*/
void  Object::GetPropertys(vector<Property*> *akPropertys,int iType,int iSide)
{
	for(vector<Property*>::iterator it = m_akPropertys.begin(); it != m_akPropertys.end(); it++) {
		if((*it)->m_iType & iType || iType == PROPERTY_TYPE_ALL){
			if((*it)->m_iSide & iSide || iSide == PROPERTY_SIDE_ALL){
				akPropertys->push_back((*it));			
			}
		}	
	}
}

void Object::GetAllPropertys(vector<Property*> *akPropertys,int iType,int iSide)
{
	if(m_iUpdateStatus & UPDATE_NONE)
		return;

	int iCurDec=0;

	//get this objects propertys
	GetPropertys(akPropertys,iType,iSide);	
	
	for(vector<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++)
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
			}
		}
	}
}

/**	\brief	Adds a property to an object if it does not have it yet.
*/
Property* Object::AddProxyProperty(const char* acName)
{
	Property* pProp = GetProperty(acName);
	if(pProp)
		return pProp;
	AddProperty(acName);
	pProp = GetProperty(acName);
	return pProp;
}

bool Object::Update(const char* acName)
{
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		if(strcmp((*it)->m_acName,acName)==0) 
		{
			(*it)->Update();
			return true;
		}
	}
	return false;
}

/**	\brief	Add a object to be a our child.

	Checks that we don't have this object as a child and if not adds it.
*/
void Object::AddChild(Object* pkObject) 
{
	// Check so we don't have child already.
	if(HasChild(pkObject))
		return;

	m_akChilds.push_back(pkObject);	
	pkObject->SetParent(this);
}

/**	\brief	Remove a object from our list of children.

	Check that we have this object as a child and if so removes it.
*/
void Object::RemoveChild(Object* pkObject)
{
	// If we don't have object as a child return.
	if(HasChild(pkObject) == false)
		return;

	//m_akChilds.remove(pkObject);	// Remove object as child.
	vector<Object*>::iterator kIt = m_akChilds.begin();
	while(kIt != m_akChilds.end())
	{
		if((*kIt) == pkObject)
		{
			m_akChilds.erase(kIt);
			break;
			//(*kIt) = m_akChilds.back();
			//m_akChilds.pop_back();
			//kIt = m_akChilds.end();
		}
		++kIt;
	}
	pkObject->SetParent(NULL);		// Set objects parent to NULL.
}

/**	\brief	Sets object to be our parent.
*/
void Object::SetParent(Object* pkObject) 
{
	// Check so we don't try to set ourself to be our own parent.
	ZFAssert(this != pkObject, "SetParent(this)");
	if(this == pkObject)
		return;

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
}

/**	\brief	Returns true if object is our child of this.
*/
bool Object::HasChild(Object* pkObject)
{
	//check if this object is in the child list
	for(vector<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)==pkObject) return true;
	}
	
	return false;
}

int Object::NrOfChilds()
{
	return m_akChilds.size();
}

/**	\brief	Delete all children of the choosen object.

	Deletes all children and the children of those object down the the youngest most defenseless object.
	MOhhahah HAIL AND KILL:
*/
void Object::DeleteAllChilds()
{
	Object* pkChildObject;

	while(m_akChilds.size()) {
		pkChildObject = (*m_akChilds.begin());

		//cout << "Child " << pkChildObject->m_kName << endl; 
		pkChildObject->DeleteAllChilds();
		delete pkChildObject;
		}

	m_akChilds.clear();
}

/**	\brief	Adds ourself and all our children to the list of objects.
*/
void Object::GetAllObjects(vector<Object*> *pakObjects)
{
	pakObjects->push_back(this);	
	
	for(vector<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
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
			float distance = abs(((*it)->GetWorldPosV() - GetWorldPosV()).Length());
			if(distance<mindistance){
				mindistance=distance;
				minobject=(*it);
			}
		}
	}		
	
	temp.clear();
	
	SetParent(minobject);
}


/**	\brief	Returns true if object is one that need to be sent over network.
*/
bool Object::IsNetWork()
{

	if(m_strType == "ZoneObject")
		return true;

	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {

		if((*it)->bNetwork == true) {
			return true;
		}
	}
	
	return false;
}

/**	\brief	Returns true if there is any netactive properys in object

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
		for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
			iUpdateFlags |= (*it)->m_iNetUpdateFlags; 
			}
	
		if(iUpdateFlags)
			return true;
		}

	return false;
}


/**	\brief	Pack Object.
*/
void Object::PackTo(NetPacket* pkNetPacket)
{
	int iParentID	=	-1;
	if(m_pkParent)
		iParentID = m_pkParent->iNetWorkID;

	pkNetPacket->Write( iParentID );

	// Force Pos Updates
	m_iNetUpdateFlags |= (OBJ_NETFLAG_POS | OBJ_NETFLAG_ROT);

	// Write Object Update Flags.
	pkNetPacket->Write( m_iNetUpdateFlags );

	//	Write Pos, Rotation, radius and name.
	Vector3 kPos;
	kPos = GetLocalPosV();
	if(m_iNetUpdateFlags & OBJ_NETFLAG_POS)
		pkNetPacket->Write(GetWorldPosV());
	
	Matrix4 kRotMatrix = GetLocalRotM();
	if(m_iNetUpdateFlags & OBJ_NETFLAG_ROT)
		pkNetPacket->Write( kRotMatrix );

	pkNetPacket->Write(m_fRadius);

	pkNetPacket->Write_NetStr(m_strName.c_str());
	g_ZFObjSys.Logf("net", " .Name '%s':", m_strName.c_str() );
	
//	char szPropertyName[256];

	// Write propertys med Propery::bNetwork = true
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
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

/**	\brief	Unpack Object.
*/
void Object::PackFrom(NetPacket* pkNetPacket)
{
	int iParentID;
	pkNetPacket->Read(iParentID);
//	m_pkParent = ;
	this->SetParent(m_pkObjectMan->GetObjectByNetWorkID(iParentID));

	int iStart = pkNetPacket->m_iPos;
	

	Vector3 kVec;
	float	  fFloat;

	pkNetPacket->Read( m_iNetUpdateFlags );
	if(m_iNetUpdateFlags & OBJ_NETFLAG_POS) {
		pkNetPacket->Read(kVec);
		SetLocalPosV(kVec);
		//SetPos(kVec);
		g_ZFObjSys.Logf("net", " .Pos: <%f,%f,%f>", kVec.x,kVec.y,kVec.z);
		}

	if(m_iNetUpdateFlags & OBJ_NETFLAG_ROT) {
		Matrix4 kRotMatrix;
		pkNetPacket->Read(kRotMatrix);
		//SetWorldPosV(kVec);
		SetLocalRotM(kRotMatrix);
		//SetWorldRotV(kVec);
		g_ZFObjSys.Logf("net", " .Rot: <%f,%f,%f>\n", kVec.x,kVec.y,kVec.z);
		}

	pkNetPacket->Read(fFloat);
	GetRadius() = fFloat;

	char szStr[256];
	pkNetPacket->Read_NetStr(szStr);
	m_strName = szStr;
	g_ZFObjSys.Logf("net", " .Name '%s'\n", m_strName.c_str() );
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

/**	\brief	Load object.
*/
void Object::Load(ZFIoInterface* pkFile)
{
	Vector3 pos;
	Matrix4 rot;

	pkFile->Read(&m_bRelativeOri,sizeof(m_bRelativeOri),1);	
	
	pkFile->Read(&pos,sizeof(pos),1);	
	pkFile->Read(&rot,sizeof(rot),1);	
	
	SetLocalRotM(rot);
	SetLocalPosV(pos);
	
	pkFile->Read(&m_kVel,sizeof(m_kVel),1);	
	pkFile->Read(&m_kAcc,sizeof(m_kAcc),1);	
	pkFile->Read(&m_fRadius,sizeof(m_fRadius),1);		
	
	pkFile->Read(&m_iObjectType,sizeof(m_iObjectType),1);		
	pkFile->Read(&m_iUpdateStatus,sizeof(m_iUpdateStatus),1);
	pkFile->Read(&m_bSave,sizeof(m_bSave),1);		
	
	pkFile->Read(&m_bZone,sizeof(m_bZone),1);			
	pkFile->Read(&m_bUseZones,sizeof(m_bUseZones),1);				
	pkFile->Read(&m_iCurrentZone,sizeof(m_bUseZones),1);						
	
	pkFile->Read(&m_eRole,sizeof(m_eRole),1);		
	pkFile->Read(&m_eRemoteRole,sizeof(m_eRemoteRole),1);				

	char acTemp[128];
	
	//name
	pkFile->Read(acTemp,128,1);		
	m_strName = acTemp;
	
	//type
	pkFile->Read(acTemp,128,1);		
	m_strType = acTemp;
	
	//load script res
	pkFile->Read(acTemp,128,1);		
	
	if(strcmp(acTemp,"none") == 0)
		m_pScriptFileHandle->FreeRes();
	else
		m_pScriptFileHandle->SetRes(string(acTemp));
	
	int i;
	
	//nr of propertys
	int iProps = 0;
	pkFile->Read(&iProps,sizeof(iProps),1);	
	//save all propertys
	for(i = 0;i< iProps;i++)
	{
		char name[50];		
		pkFile->Read(&name,50,1);			
		
		Property* prop = AddProperty(name);
		
		if(prop)
			prop->Load(pkFile);
	}
		
	//nr of childs
	int iChilds = 0;		
	pkFile->Read(&iChilds,sizeof(iChilds),1);		
	//save all childs
	for(i = 0;i<iChilds;i++)
	{
		Object* newobj = m_pkObjectMan->CreateObject();
		newobj->SetParent(this);
		newobj->Load(pkFile);		
	}
}

/**	\brief	Save Object.
*/
void Object::Save(ZFIoInterface* pkFile)
{
	Vector3 pos = GetLocalPosV();
	Matrix4 rot = GetLocalRotM();

	pkFile->Write(&m_bRelativeOri,sizeof(m_bRelativeOri),1);	
	pkFile->Write(&pos,sizeof(pos),1);	
	pkFile->Write(&rot,sizeof(rot),1);	
	
	pkFile->Write(&m_kVel,sizeof(m_kVel),1);	
	pkFile->Write(&m_kAcc,sizeof(m_kAcc),1);	
	pkFile->Write(&m_fRadius,sizeof(m_fRadius),1);		
	
	pkFile->Write(&m_iObjectType,sizeof(m_iObjectType),1);		
	pkFile->Write(&m_iUpdateStatus,sizeof(m_iUpdateStatus),1);
	pkFile->Write(&m_bSave,sizeof(m_bSave),1);		
	
	pkFile->Write(&m_bZone,sizeof(m_bZone),1);			
	pkFile->Write(&m_bUseZones,sizeof(m_bUseZones),1);				
	pkFile->Write(&m_iCurrentZone,sizeof(m_bUseZones),1);					
	
	pkFile->Write(&m_eRole,sizeof(m_eRole),1);		
	pkFile->Write(&m_eRemoteRole,sizeof(m_eRemoteRole),1);				



	char acTemp[128];
	
	//name
	strcpy(acTemp,m_strName.c_str());
	pkFile->Write(acTemp,128,1);		
	
	//type
	strcpy(acTemp,m_strType.c_str());	
	pkFile->Write(acTemp,128,1);		
		
	//save script file
	if(m_pScriptFileHandle->IsValid())
		strcpy(acTemp,m_pScriptFileHandle->GetRes().c_str());	
	else	
		strcpy(acTemp,"none");	
	pkFile->Write(acTemp,128,1);		
		
	int i;
		
	//nr of propertys
	int iProps = m_akPropertys.size();		
	pkFile->Write(&iProps,sizeof(iProps),1);	
	//save all propertys
	for( i = 0;i<iProps;i++)
	{
		pkFile->Write(&m_akPropertys[i]->m_acName,50,1);	
		m_akPropertys[i]->Save(pkFile);
	}
		
		
	//nr of childs
	int iChilds = m_akChilds.size();		
	pkFile->Write(&iChilds,sizeof(iChilds),1);		
	//save all childs
	for( i = 0;i<iChilds;i++)
	{
		m_akChilds[i]->Save(pkFile);
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
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
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
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
		(*it)->HandleGameMessage(Msg);
}

/**	\brief	Checks parent / Child links.

	Checks that the parent/child links is ok and returns false if they are not.
*/
bool Object::CheckLinks(bool bCheckChilds, int iPos)
{
	// Check that our parent link to us.
	if(m_pkParent) {
		if(m_pkParent->HasChild(this) == false) {
			return false;
			}
		}
	
	for(vector<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
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

/**	\brief	Print Info about a object and all its children to file.
*/
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
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Pos = <%f,%f,%f>\n", GetWorldPosV().x, GetWorldPosV().y, GetWorldPosV().z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Rot = <%f,%f,%f>\n", GetWorldRotV().x, GetWorldRotV().y, GetWorldRotV().z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Vel = <%f,%f,%f>\n", m_kVel.x, m_kVel.y, m_kVel.z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "m_kAcc = <%f,%f,%f>\n", m_kAcc.x, m_kAcc.y, m_kAcc.z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Name = %s\n", m_strType.c_str() );

	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Type = %d\n", m_iObjectType );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "UpdateStatus = %d\n", m_iUpdateStatus );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Save = %d\n", m_bSave );

	vector<string> akPropertyNames;
	
	for(vector<Property*>::iterator it = m_akPropertys.begin(); it != m_akPropertys.end(); it++) {
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

	for(vector<Object*>::iterator it2=m_akChilds.begin();it2!=m_akChilds.end();it2++) {
		(*it2)->PrintTree(pos+1);
	}

	TabIn(pos);	g_ZFObjSys.Logf("fisklins", "}\n" );
}

void Object::MakeCloneOf(Object* pkOrginal)
{
	SetParent(m_pkParent);

	m_kVel		= pkOrginal->m_kVel;
	m_strName	= pkOrginal->m_strName;
	m_strType	= pkOrginal->m_strType;
	m_iObjectType			= pkOrginal->m_iObjectType;
	m_iUpdateStatus		= pkOrginal->m_iUpdateStatus;
	m_bSave		= pkOrginal->m_bSave;
	m_kAcc		= pkOrginal->m_kAcc;
	m_fRadius	= pkOrginal->m_fRadius;	

	Property* pkProp;
	vector<string> akPropertyNames;

	for(vector<Property*>::iterator it = pkOrginal->m_akPropertys.begin(); it != pkOrginal->m_akPropertys.end(); it++) {
		pkProp = AddProperty((*it)->m_acName);
		
		// Get Values
		akPropertyNames = (*it)->GetValueNames();

		for(unsigned int i=0; i < akPropertyNames.size(); i++) {
			pkProp->SetValue(akPropertyNames[i], (*it)->GetValue(akPropertyNames[i]));
			}
		}
}

void Object::ResetChildsGotData()
{
	ResetGotData();
	
	for(vector<Object*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)->GetRelativeOri())
			(*it)->ResetChildsGotData();	
	}	
}

void Object::SetLocalRotM(Matrix4 kNewRot)
{
	m_iNetUpdateFlags |= OBJ_NETFLAG_ROT;
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
	
	//check new zone
	if(m_bUseZones)
	{
		if(!m_bZone)
		{
			if(!m_bRelativeOri)
			{		
				int nZ = m_pkObjectMan->GetZoneIndex(kPos,m_iCurrentZone,false);
				if(nZ == -1)
				{
					cout<<"object tried to move outside zone"<<endl;
					return;
				}
				
				ZoneData* cz = m_pkObjectMan->GetZoneData(nZ);
				if(!cz)
				{
					cout<<"zone does not exist"<<endl;
					return;
				}
		
				if(!cz->m_pkZone)
				{
					cout<<"object tried to move to a unloaded zone"<<endl;
					return;
				}
		
				m_iCurrentZone = nZ;
				SetParent((Object*)cz->m_pkZone);				
			}
		}
	}

	
	m_iNetUpdateFlags |= OBJ_NETFLAG_POS;
	ResetChildsGotData();
	
	m_kLocalPosV = kPos;
}

void Object::SetWorldPosV(Vector3 kPos)
{
	m_iNetUpdateFlags |= OBJ_NETFLAG_POS;
	Vector3 kDiff = kPos - GetWorldPosV();
	Vector3 newlocalpos = m_kLocalPosV + kDiff;
	
	SetLocalPosV(newlocalpos);
}

void Object::RotateLocalRotV(Vector3 kRot)
{
	ResetChildsGotData();
	
	m_kLocalRotM.Rotate(kRot);
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
				m_kWorldOriM = GetLocalOriM() * m_pkParent->GetWorldOriM() ;
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


