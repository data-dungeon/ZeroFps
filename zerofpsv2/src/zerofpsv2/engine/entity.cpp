#include "entity.h"

#include "../engine_systems/physicsengine/physicsengine.h"
#include "../basic/zfsystem.h"
#include "entitymanager.h"
#include "../engine_systems/propertys/p_physic.h"
 
typedef list<Entity*>::iterator		itListObject;
typedef list<Property*>::iterator	itListProperty;
 

Entity::Entity() 
{
	// Get Ptrs to some usefull objects.
	m_pkObjectMan			= static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkPropertyFactory	= static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));	
	m_pkFps				   = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
		
	ZFAssert(m_pkObjectMan,			"Entity::Entity(): Failed to find ObjectManger");
	ZFAssert(m_pkPropertyFactory,	"Entity::Entity(): Failed to find PropertyFactory");
	ZFAssert(m_pkFps,				   "Entity::Entity(): Failed to find ZeroFps");

	m_pkObjectMan->Link(this);		// Add ourself to objectmanger and get a NetID.

	// SetDefault Values.
	m_kLocalRotM.Identity();
	m_kLocalPosV= Vector3::ZERO;	
	m_kVel		= Vector3::ZERO;
	m_kAcc		= Vector3::ZERO;
	m_fRadius	= 1;
	
  	SetName("A Entity");
	SetType("Entity");

	m_pScriptFileHandle	= new ZFResourceHandle;

	m_iNetUpdateFlags		= 0;

	m_eRole					= NETROLE_AUTHORITY;
	m_eRemoteRole			= NETROLE_PROXY;

	m_iObjectType			= OBJECT_TYPE_DYNAMIC;	
	m_iUpdateStatus		= UPDATE_ALL;
	m_bZone					= false;
	m_iCurrentZone			= -1;
	m_bUseZones				= false;
	m_bSave					= true;	
	m_pkParent				= NULL;
	m_bRelativeOri			= false;
	m_bFirstSetPos			= true;
	
	//clear child list
	m_akChilds.clear();	
	
	//reset all oritentation data
	ResetGotData();
	
	//set nr of connections
	SetNrOfConnections(m_pkFps->GetMaxPlayers());
}

Entity::~Entity() 
{
	// First we remove all childs
	DeleteAllChilds();

	// Add Ourself to our Net.DeletList
	m_aiNetDeleteList.push_back(iNetWorkID);
	// Add our Net.DeleteList to our parent.
	if(m_pkParent != NULL)
		m_pkParent->m_aiNetDeleteList.insert(m_pkParent->m_aiNetDeleteList.begin(), m_aiNetDeleteList.begin(), m_aiNetDeleteList.end());

	// Then we remove ourself from our master.
	if(m_pkParent != NULL)
		m_pkParent->RemoveChild(this);
	
	while(!m_akPropertys.empty())
	{
		Property* TempProperty = m_akPropertys.back();
		m_akPropertys.pop_back();
		delete TempProperty;
	}

	// Tell Entity manger that we are no more.
	m_pkObjectMan->UnLink(this);
	
	delete(m_pScriptFileHandle);
}

/*
bool Entity::IsA(string strStringType)
{
	return m_pkObjectMan->IsA(this, strStringType);
}*/

/**	\brief	Adds a property to the Entity.
*/
Property* Entity::AddProperty(Property* pkNewProperty) 
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

/**	\brief	Creates and adds a property to the Entity.
*/
Property* Entity::AddProperty(const char* acName)
{
	ZFAssert(acName, "Entity::AddProperty(): acName is NULL");
	Property* pProp = m_pkPropertyFactory->CreateProperty(acName);
	
	if(pProp==NULL)
	{
		cout << "Error Property " << acName << " Not Registered" <<endl;
		return false;
	}

	AddProperty(pProp);
	return pProp;
}

/**	\brief	Removes the property from the Entity.
*/
void Entity::RemoveProperty(Property* pkProp) 
{
	//m_akPropertys.remove(pkProp);
	
	///EVIL GUBB///// ---changed by EVIL ZEROM who didn't like it when it crashed
   for ( vector<Property*>::iterator kIte = m_akPropertys.begin();
         kIte != m_akPropertys.end(); kIte++ )
   {
		if((*kIte) == pkProp)
      {
         m_akPropertys.erase ( kIte );
         break;
      }
   }

	PropertyLost(pkProp);
}

/**	\brief	Removes and deletes the property. 
	
	Removes a property from the Entity and then deletes it,
*/
bool Entity::DeleteProperty(const char* acName) 
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

void Entity::PropertyLost(Property* pkProp)
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
	
	Returns a pointer the the property 'name' or NULL if the property is not part of the Entity.
*/
Property* Entity::GetProperty(const char* acName) 
{
	if(strstr(acName, "P_") == NULL) {
		if(strcmp(acName, "nons") != 0) {
			printf("REQUEST FOR A PROPERTY WITH OLD NAME %s\n", acName);
			assert(0);	
			}
		}

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
void  Entity::GetPropertys(vector<Property*> *akPropertys,int iType,int iSide)
{
	for(vector<Property*>::iterator it = m_akPropertys.begin(); it != m_akPropertys.end(); it++) {
		if((*it)->m_iType & iType /*|| iType & PROPERTY_TYPE_ALL*/ ){
			if((*it)->m_iSide & iSide /*|| iSide == PROPERTY_SIDE_ALL*/ ){
				akPropertys->push_back((*it));			
			}
		}	
	}
}

/**	\brief	Returns a vector with propertys for the Entity and child objects.
	
	Returns a vector with the the propertys that have the correct type and side flags.
	Walks child objects of selected Entity.
*/
void Entity::GetAllPropertys(vector<Property*> *akPropertys,int iType,int iSide)
{
	if(m_iUpdateStatus & UPDATE_NONE)
		return;

	//get this objects propertys
	GetPropertys(akPropertys,iType,iSide);	
	
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++)
	{	
		(*it)->GetAllPropertys(akPropertys,iType,iSide);		
	}		
	
/*		if(m_iUpdateStatus & UPDATE_ALL)
		{

		} 
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
			
				/*case OBJECT_TYPE_PLAYER:
					if(m_iUpdateStatus & UPDATE_PLAYERS)		
						(*it)->GetAllPropertys(akPropertys,iType,iSide);
					break;
		
				case OBJECT_TYPE_STATDYN:
					if(m_iUpdateStatus & UPDATE_STATDYN)		
						(*it)->GetAllPropertys(akPropertys,iType,iSide);
					break;*
			}
		}
	}
*/	
}

/**	\brief	Adds a property to an Entity if it does not have it yet.
*/
Property* Entity::AddProxyProperty(const char* acName)
{
	Property* pProp = GetProperty(acName);
	if(pProp)
		return pProp;
	AddProperty(acName);
	pProp = GetProperty(acName);
	return pProp;
}

bool Entity::Update(const char* acName)
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

/**	\brief	Add a Entity to be a our child.

	Checks that we don't have this Entity as a child and if not adds it.
*/
void Entity::AddChild(Entity* pkObject) 
{
	// Check so we don't have child already.
	if(HasChild(pkObject))
		return;

	m_akChilds.push_back(pkObject);	
	pkObject->SetParent(this);
}

/**	\brief	Remove a Entity from our list of children.

	Check that we have this Entity as a child and if so removes it.
*/
void Entity::RemoveChild(Entity* pkObject)
{
	// If we don't have Entity as a child return.
	if(HasChild(pkObject) == false)
		return;

	//m_akChilds.remove(pkObject);	// Remove Entity as child.
	vector<Entity*>::iterator kIt = m_akChilds.begin();
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

/**	\brief	Sets Entity to be our parent.
*/
void Entity::SetParent(Entity* pkObject) 
{
	// Check so we don't try to set ourself to be our own parent.
//	ZFAssert(this != pkObject, "SetParent(this)");
	if(this == pkObject)
		return;

	SetNetUpdateFlag(NETUPDATEFLAG_PARENT,true);

	// Remove Parent
	if(pkObject == NULL) {
		if(m_pkParent == NULL)
			return;
		
		Entity* pkParent = m_pkParent;
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

/**	\brief	Returns true if Entity is our child of this.
*/
bool Entity::HasChild(Entity* pkObject)
{
	//check if this Entity is in the child list
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)==pkObject) return true;
	}
	
	return false;
}

int Entity::NrOfChilds()
{
	return m_akChilds.size();
}

/**	\brief	Delete all children of the choosen Entity.

	Deletes all children and the children of those Entity down the the youngest most defenseless Entity.
	MOhhahah HAIL AND KILL:
*/
void Entity::DeleteAllChilds()
{
	Entity* pkChildObject;

	while(m_akChilds.size()) {
		pkChildObject = (*m_akChilds.begin());

		//cout << "Child " << pkChildObject->m_kName << endl; 
		pkChildObject->DeleteAllChilds();
		delete pkChildObject;
		}

	m_akChilds.clear();
}

bool Entity::AttachToZone()
{
	return AttachToZone(GetLocalPosV());
}


bool Entity::AttachToZone(Vector3 kPos)
{
	if(!m_bZone)
	{
		if(!m_bRelativeOri)
		{		
			int nZ = m_pkObjectMan->GetZoneIndex(kPos,m_iCurrentZone,false);
			if(nZ == -1)
			{
				//cout<<"Entity tried to move outside zone"<<endl;
				return false;
			}
			
			ZoneData* cz = m_pkObjectMan->GetZoneData(nZ);
			if(!cz)
			{
				//cout<<"zone does not exist"<<endl;
				return  false;
			}
	
			if(!cz->m_pkZone)
			{
				//cout<<"Entity tried to move to a unloaded zone"<<endl;
				return  false;
			}
	
			if(nZ == m_iCurrentZone)
				return true;
			
			ZoneChange(m_iCurrentZone,nZ);
			m_iCurrentZone = nZ;
			SetParent((Entity*)cz->m_pkZone);				
			return true;
		}
	}	
	
	return false;
}

void Entity::ZoneChange(int iCurrent,int iNew)
{
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		(*it)->ZoneChange(iCurrent,iNew);
	}
}

void Entity::GetAllDynamicEntitys(vector<Entity*> *pakObjects)
{
	if( GetName() == "StaticEntity" )
		return;

	pakObjects->push_back(this);	
	
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		(*it)->GetAllDynamicEntitys(pakObjects);
	}	
	
}

/**	\brief	Adds ourself and all our children to the list of objects.
*/
void Entity::GetAllObjects(vector<Entity*> *pakObjects)
{
	if(m_iUpdateStatus & UPDATE_NONE)
		return;
	
	pakObjects->push_back(this);	
	
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		(*it)->GetAllObjects(pakObjects);
	}	
}


/**	\brief	Returns true if Entity is one that need to be sent over network.
*/
bool Entity::IsNetWork()
{
	m_bIsNetWork = false;

	if(m_strName == "ZoneObject")
	{
		m_bIsNetWork = true;
	}
	else if(m_strName == "StaticEntity")
	{
		m_bIsNetWork = true;
	}
	else {
		for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
			if((*it)->bNetwork == true) {
				m_bIsNetWork = true;
				break;
			}
		}
	}
	
	return m_bIsNetWork;
}

/**	\brief	Returns true if there is any netactive properys in Entity

	Returns true if this Entity needs to be sent over network (if anything has changed since last
	net update).
*/
bool Entity::NeedToPack()
{
	// We can only send data for Entity we own.
	if( m_eRole != NETROLE_AUTHORITY)			return false;
	// We only send Entity that the other side need to know about
	if( m_eRemoteRole	== NETROLE_NONE)		return false;

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



/**	\brief	Pack Entity.
*/
void Entity::PackTo(NetPacket* pkNetPacket, int iConnectionID)
{
	//check delete list
	if(m_aiNetDeleteList.size())
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_DELETE,true);
	

	//send update flags
	pkNetPacket->Write(m_kNetUpdateFlags[iConnectionID]);
	
	//send parent
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_PARENT))
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_PARENT,false);		
		
		int iParentID	=	-1;
		if(m_pkParent)
			iParentID = m_pkParent->iNetWorkID;

		pkNetPacket->Write( iParentID );
	}

   // send update status
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_UPDATESTATUS))
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_UPDATESTATUS,false);	   
	   
	   pkNetPacket->Write( m_iUpdateStatus );		   
	}

	//send delete list
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_DELETE))
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_DELETE,false);
		
		pkNetPacket->Write((int) m_aiNetDeleteList.size() );

		for(int i=0; i<m_aiNetDeleteList.size(); i++)
			pkNetPacket->Write((int) m_aiNetDeleteList[i] );
	}

	//send position
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_POS))
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_POS,false);		
		pkNetPacket->Write(GetWorldPosV());
	}
	
	//send rotation
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_ROT))	
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_ROT,false);	
		pkNetPacket->Write(GetLocalRotM());
	}
	
	//send velocity
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_VEL))	
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_VEL,false);	
		pkNetPacket->Write(m_kVel);
	}
	
	
	//send radius
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_RADIUS))	
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_RADIUS,false);	
		pkNetPacket->Write(m_fRadius);
	}
	
	//send name
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_NAME))	
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_NAME,false);	
		pkNetPacket->Write_Str(m_strName.c_str());
	}
	
	//send type
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_TYPE))	
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_TYPE,false);	
		pkNetPacket->Write_Str(m_strType.c_str());
	}	

/*
	// Force Pos Updates
	m_iNetUpdateFlags |= (OBJ_NETFLAG_POS | OBJ_NETFLAG_ROT);
	if(m_aiNetDeleteList.size())
		m_iNetUpdateFlags |= OBJ_NETFLAG_DEL;

	// Write Entity Update Flags.
	pkNetPacket->Write( m_iNetUpdateFlags );

	// Write Delete List
	if(m_iNetUpdateFlags & OBJ_NETFLAG_DEL) {
		pkNetPacket->Write((int) m_aiNetDeleteList.size() );

		for(int i=0; i<m_aiNetDeleteList.size(); i++)
			pkNetPacket->Write((int) m_aiNetDeleteList[i] );
		}
*/
	
/*	Vector3 kPos;
	kPos = GetLocalPosV();
	if(m_iNetUpdateFlags & OBJ_NETFLAG_POS)
		pkNetPacket->Write(GetWorldPosV());
*/	
/*	Matrix4 kRotMatrix = GetLocalRotM();
	if(m_iNetUpdateFlags & OBJ_NETFLAG_ROT)
		pkNetPacket->Write( kRotMatrix );
*/
//	pkNetPacket->Write(m_fRadius);

//	pkNetPacket->Write_NetStr(m_strName.c_str());
	//g_ZFObjSys.Logf("net", " .Name '%s':", m_strName.c_str() );
	
//	char szPropertyName[256];

	// Write propertys med Propery::bNetwork = true
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) {
//		g_ZFObjSys.Logf("net", " Check '%s': ",(*it)->m_acName );
		if((*it)->bNetwork) {
			(*it)->m_iNetUpdateFlags |= m_pkObjectMan->m_iForceNetUpdate;

			//Property* hora = (*it);
			//strcpy(szPropertyName, (*it)->m_acName);
			if((*it)->m_iNetUpdateFlags) {
//				g_ZFObjSys.Logf("net", "Add\n");
				pkNetPacket->Write_NetStr((*it)->m_acName);
				(*it)->PackTo(pkNetPacket,iConnectionID);
				}
			//else {
				//g_ZFObjSys.Logf("net", "Same as last year.\n");
			//	}
			}
//		else 
//			g_ZFObjSys.Logf("net", "Dont Add\n");
	}

	pkNetPacket->Write_NetStr("");

//	m_iNetUpdateFlags = 0;
}

/**	\brief	Unpack Entity.
*/
void Entity::PackFrom(NetPacket* pkNetPacket, int iConnectionID)
{
	int iStart = pkNetPacket->m_iPos;

	//read update flags
	pkNetPacket->Read(m_kNetUpdateFlags[0]);			//con id is 0

	//get parent
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_PARENT))
	{
		cout<<"got parent update"<<endl;
		
		int iParentID	=	-1;

		pkNetPacket->Read( iParentID );
		SetParent(m_pkObjectMan->GetObjectByNetWorkID(iParentID));
	}

   // get update status
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_UPDATESTATUS))
	{
		cout<<"got update status"<<endl;	
		
	   pkNetPacket->Read( m_iUpdateStatus );		   
	}

	//get delete list
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_DELETE))
	{	
		cout<<"got delete data"<<endl;	
	
		int iNumDelObjects;
		Entity* pkNetSlave;	
		
		pkNetPacket->Read(iNumDelObjects);

		for(int i=0; i<iNumDelObjects; i++) 
		{
			int iDelObjectID;
			pkNetPacket->Read(iDelObjectID );
			pkNetSlave = m_pkObjectMan->GetObjectByNetWorkID(iDelObjectID);
			m_pkObjectMan->Delete(pkNetSlave);
		}
	}

	//get position
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_POS))
	{
		cout<<"got position:"<<endl;	
		Vector3 kPos;
		pkNetPacket->Read(kPos);
		SetLocalPosV(kPos);
	}
	
	//get rotation	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_ROT))
	{
		cout<<"got rotation:"<<endl;	
		Matrix4 kRot;
		pkNetPacket->Read(kRot);
		SetLocalRotM(kRot);
	}
	
	//get velocity	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_VEL))
	{
		cout<<"got velocity:"<<endl;	
		Vector3 kVel;
		pkNetPacket->Read(kVel);
		GetVel()=kVel;
	}
	
	//get radius
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_RADIUS))	
	{
		cout<<"got radius"<<endl;	
		pkNetPacket->Read(m_fRadius);
	}
	
	//get name
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_NAME))	
	{
		char szStr[256];
		pkNetPacket->Read_Str(szStr);
		m_strName = szStr;		
	
		cout<<"got name "<<m_strName<<endl;		
	}	
	
	//get type
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_TYPE))	
	{
		char szStr[256];
		pkNetPacket->Read_Str(szStr);
		m_strType = szStr;		
	
		cout<<"got type "<<m_strType<<endl;		
	}		
/*
	int iDelObjectID;

	int iParentID;
	pkNetPacket->Read(iParentID);

   // read update status
   pkNetPacket->Read( m_iUpdateStatus );

//	m_pkParent = ;
	this->SetParent(m_pkObjectMan->GetObjectByNetWorkID(iParentID));

	int iStart = pkNetPacket->m_iPos;
	

	Vector3 kVec;
	float	  fFloat;

	pkNetPacket->Read( m_iNetUpdateFlags );

	if(m_iNetUpdateFlags & OBJ_NETFLAG_DEL) {
		int iNumDelObjects;
		Entity* pkNetSlave;

		pkNetPacket->Read(iNumDelObjects);

		for(int i=0; i<iNumDelObjects; i++) {
			pkNetPacket->Read(iDelObjectID );
			pkNetSlave = m_pkObjectMan->GetObjectByNetWorkID(iDelObjectID);
			m_pkObjectMan->Delete(pkNetSlave);
			}
		}

	if(GetNetUpdateFlag(0,0))
	{
		cout<<"got position:"<<endl;	
		Vector3 kPos;
		pkNetPacket->Read(kPos);
		SetLocalPosV(kPos);
	}
	
	if(GetNetUpdateFlag(0,1))
	{
		cout<<"got rotation:"<<endl;	
		Matrix4 kRot;
		pkNetPacket->Read(kRot);
		SetLocalRotM(kRot);
	}

/*	if(m_iNetUpdateFlags & OBJ_NETFLAG_POS) {
		pkNetPacket->Read(kVec);
		SetLocalPosV(kVec);
		//SetPos(kVec);
		//g_ZFObjSys.Logf("net", " .Pos: <%f,%f,%f>", kVec.x,kVec.y,kVec.z);
		}
*/
/*	if(m_iNetUpdateFlags & OBJ_NETFLAG_ROT) {
		Matrix4 kRotMatrix;
		pkNetPacket->Read(kRotMatrix);
		//SetWorldPosV(kVec);
		SetLocalRotM(kRotMatrix);
		//SetWorldRotV(kVec);
		//g_ZFObjSys.Logf("net", " .Rot: <%f,%f,%f>\n", kVec.x,kVec.y,kVec.z);
		}
*
	pkNetPacket->Read(fFloat);
	GetRadius() = fFloat;

	char szStr[256];
	pkNetPacket->Read_NetStr(szStr);
	m_strName = szStr;
	//g_ZFObjSys.Logf("net", " .Name '%s'\n", m_strName.c_str() );
	//g_ZFObjSys.Logf("net", " -Head Size = %d\n",  pkNetPacket->m_iPos - iStart );	
*/
	char szProperty[256];
	pkNetPacket->Read_NetStr(szProperty);

	while(strcmp(szProperty,"") != 0) {
		int iPStart = pkNetPacket->m_iPos;
		Property* pProp  = AddProxyProperty(szProperty);
		if(pProp) {
			//g_ZFObjSys.Logf("net", " /%s\n", szProperty);
			pProp->PackFrom(pkNetPacket, ZF_NET_NOCLIENT);
			}
		else {
			cout << "Error in netpacket" << endl;
			pkNetPacket->SetError(true);
			return;
			}

		int iPEnd	= pkNetPacket->m_iPos;
		//g_ZFObjSys.Logf("net", " -Size: %d\n", (iPEnd - iPStart) + 4);	// +4 for netstring for property name

		pkNetPacket->Read_NetStr(szProperty);
		}	

	int iEnd = pkNetPacket->m_iPos;
	//g_ZFObjSys.Logf("net", " .End Of Propertys size: 4\n");
	//g_ZFObjSys.Logf("net", " .Size for Entity %d\n",(iEnd - iStart) );
	m_pkObjectMan->m_iTotalNetObjectData += (iEnd - iStart);
	m_pkObjectMan->m_iNumOfNetObjects ++;
	//g_ZFObjSys.Logf("net", "\n");
}

/**	\brief	Load Entity.
*/
void Entity::Load(ZFIoInterface* pkFile)
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
	//load all propertys
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
	
	//load all childs
	for( i = 0; i < iChilds; i++ )
	{
		Entity* newobj = m_pkObjectMan->CreateObject();
		newobj->SetParent(this);
		newobj->Load(pkFile);		
	}
	
	//reset alla update flags for this object
	ResetAllNetUpdateFlags();
}

/**	\brief	Save Entity.
*/
void Entity::Save(ZFIoInterface* pkFile)
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
		
		
	//count number of childs to save
	int iChilds = 0;
	
	for( i = 0;i<m_akChilds.size();i++)
	{
		if(m_akChilds[i]->GetSave())
		{	
			iChilds++;
		}
	}		

	//nr of childs
	pkFile->Write(&iChilds,sizeof(iChilds),1);		
	
	//save all childs
	for( i = 0;i<iChilds;i++)
	{
		if(m_akChilds[i]->GetSave())
			m_akChilds[i]->Save(pkFile);
	}
}


// Collision / Shape.
float Entity::GetBoundingRadius()
{
	Property* pr=GetProperty("P_Physic");
	if(pr==NULL)
		return 1;

	return (static_cast<P_Physic*>(pr))->GetBoundingRadius();
}

void Entity::Touch(Collision* pkCol)
{
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		(*it)->Touch(pkCol);
	}
}

void Entity::Touch(int iId)
{
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		(*it)->Touch(iId);
	}
}


// Game Messages
/// Adds a GameMessage to be handled later by Entity.
void Entity::AddGameMessage(GameMessage& Msg)
{
	m_kGameMessages.push_back(Msg);
}

void Entity::HandleMessages()
{
	GameMessage Msg;

	for(unsigned int i=0; i<m_kGameMessages.size(); i++) {
		Msg = m_kGameMessages[i];
		RouteMessage(Msg);
		}

	m_kGameMessages.clear();
}

/// Send a GameMessage to all Entity propertys.
void Entity::RouteMessage(GameMessage& Msg)
{
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
		(*it)->HandleGameMessage(Msg);
}

/**	\brief	Checks parent / Child links.

	Checks that the parent/child links is ok and returns false if they are not.
*/
bool Entity::CheckLinks(bool bCheckChilds, int iPos)
{
	// Check that our parent link to us.
	if(m_pkParent) {
		if(m_pkParent->HasChild(this) == false) {
			return false;
			}
		}
	
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
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

/**	\brief	Print Info about a Entity and all its children to file.
*/
void Entity::PrintTree(int pos)
{
	const char* szValue;
	string strValue;

	g_ZFObjSys.Logf("fisklins", "" );

	TabIn(pos);

	TabIn(pos);			g_ZFObjSys.Logf("fisklins", "Entity[%d]\n", iNetWorkID);
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

	for(vector<Entity*>::iterator it2=m_akChilds.begin();it2!=m_akChilds.end();it2++) {
		(*it2)->PrintTree(pos+1);
	}

	TabIn(pos);	g_ZFObjSys.Logf("fisklins", "}\n" );
}

void Entity::MakeCloneOf(Entity* pkOrginal)
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

void Entity::SetRadius(float fRadius)
{
	if(m_fRadius == fRadius)
		return;
		
	m_fRadius = fRadius;
	
	SetNetUpdateFlag(NETUPDATEFLAG_RADIUS,true);
}

void Entity::SetType(string strType)
{
	if(m_strType == strType)
		return;
		
	m_strType = strType;
	
	SetNetUpdateFlag(NETUPDATEFLAG_TYPE,true);
}

void Entity::SetName(string strName)
{
	if(strName == m_strName)
		return;
		
	m_strName = strName;
	
	SetNetUpdateFlag(NETUPDATEFLAG_NAME,true);
}


void Entity::SetVel(Vector3 kVel)
{
	if(kVel == m_kVel)
		return;
		
	m_kVel = kVel;
	
	SetNetUpdateFlag(NETUPDATEFLAG_VEL,true);
}


void Entity::ResetChildsGotData()
{
	ResetGotData();
	
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)->GetRelativeOri())
			(*it)->ResetChildsGotData();	
	}	
}

void Entity::SetLocalRotM(Matrix4 kNewRot)
{
	if(kNewRot == m_kLocalRotM)
		return;

	ResetChildsGotData();
	SetNetUpdateFlag(1,true);	
	
	m_kLocalRotM = kNewRot;


}

void Entity::SetLocalRotV(Vector3 kRot)
{
	ResetChildsGotData();
	
	m_kLocalRotM.Identity();
	m_kLocalRotM.Rotate(kRot);
	
}

void Entity::SetWorldRotV(Vector3 kRot)
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

void Entity::SetLocalPosV(Vector3 kPos)
{
	if(kPos == m_kLocalPosV)
		return;

	//check new zone
	if(m_bUseZones)
	{
		if(!AttachToZone(kPos))
			return;
	}

	m_iNetUpdateFlags |= OBJ_NETFLAG_POS;
	ResetChildsGotData();
	
	
	m_kLocalPosV = kPos;
	SetNetUpdateFlag(0,true);
	
	if(m_bFirstSetPos)						//if the pos has never been set, the set oldpos to the new one
	{
		m_kILocalPosV=m_kLocalPosV;
		m_bFirstSetPos=false;
	}
}

void Entity::SetWorldPosV(Vector3 kPos)
{
	m_iNetUpdateFlags |= OBJ_NETFLAG_POS;
	Vector3 kDiff = kPos - GetWorldPosV();
	Vector3 newlocalpos = m_kLocalPosV + kDiff;
	
	SetLocalPosV(newlocalpos);
}

void Entity::RotateLocalRotV(Vector3 kRot)
{
	ResetChildsGotData();
	SetNetUpdateFlag(1,true);	
	
	m_kLocalRotM.Rotate(kRot);
	
}


Vector3 Entity::GetLocalRotV()
{
	if(!m_kGotData[LOCAL_ROT_V])
	{
		m_kLocalRotV = m_kLocalRotM.GetRotVector();
		m_kGotData[LOCAL_ROT_V] = true;
	}

	return m_kLocalRotV;
}

Vector3 Entity::GetLocalPosV()
{	
	return m_kLocalPosV;
}

Matrix4 Entity::GetLocalRotM()
{
	return m_kLocalRotM;
}

Vector3 Entity::GetIWorldPosV()
{
	m_kILocalPosV +=(GetWorldPosV() - m_kILocalPosV) / 12;

	return m_kILocalPosV;
}

Vector3 Entity::GetWorldPosV()
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

Matrix4 Entity::GetWorldRotM()
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

Vector3 Entity::GetWorldRotV()
{
	if(!m_kGotData[WORLD_ROT_V])
	{	
		m_kWorldRotV = GetWorldRotM().GetRotVector();
		m_kGotData[WORLD_ROT_V] = true;
	}

	return m_kWorldRotV;
}

Matrix4 Entity::GetWorldOriM()
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

Matrix4 Entity::GetLocalOriM()
{
	if(!m_kGotData[LOCAL_ORI_M])
	{	
		m_kLocalOriM = m_kLocalRotM;
		m_kLocalOriM.Translate(m_kLocalPosV);
	}
	
	return m_kLocalOriM;
}

Entity* Entity::GetStaticEntity()
{
	Entity* pkStaticEntity=NULL;		
	vector<Entity*> kEntitys;				
	GetAllObjects(&kEntitys);
	
	for(int i=0;i<kEntitys.size();i++)
	{
		if(kEntitys[i]->GetName()=="StaticEntity")
		{
			pkStaticEntity=kEntitys[i]; 
			break;
		}		
	}
	
	return pkStaticEntity;
}


void	Entity::SetNrOfConnections(int iConNR)
{
	m_kNetUpdateFlags.resize(iConNR);
	ResetAllNetUpdateFlags();
}

void	Entity::ResetAllNetUpdateFlags()
{
	for(int i = 0;i<m_kNetUpdateFlags.size();i++)
	{
		m_kNetUpdateFlags[i].reset();	//reset all bits to false
		m_kNetUpdateFlags[i].flip();  //flip all bits to true
	}
}

void	Entity::ResetAllNetUpdateFlags(int iConID)
{
	m_kNetUpdateFlags[iConID].reset();	//reset all bits to false
	m_kNetUpdateFlags[iConID].flip();  //flip all bits to true
}

bool	Entity::GetNetUpdateFlag(int iConID,int iFlagID)
{
	return m_kNetUpdateFlags[iConID][iFlagID];
}

void Entity::SetNetUpdateFlag(int iFlagID,bool bValue)
{
	for(int i = 0;i<m_kNetUpdateFlags.size();i++)
	{
		m_kNetUpdateFlags[i][iFlagID] = bValue;
	}
}

void	Entity::SetNetUpdateFlag(int iConID,int iFlagID,bool bValue)
{
	m_kNetUpdateFlags[iConID][iFlagID] = bValue;
}

