#include "entity.h"
#include "../basic/zfsystem.h"
#include "entitymanager.h"
#include "zerofps.h"
#include "../engine_systems/propertys/p_track.h"
 
//#define LOGALL

#ifdef LOGALL
	#define LOGSIZE(name, size)	m_pkEntityManager->GetWorldEntity()->AddVarDouble(string(name), size )
#else
	#define LOGSIZE(name, size)	
#endif
 

Entity::Entity() 
{
	// Get Ptrs to some usefull Subsystems.
	m_pkEntityManager		= static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));
	m_pkPropertyFactory	= static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));	
	m_pkZeroFps			   = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
		
	ZFAssert(m_pkEntityManager,	"Entity::Entity(): Failed to find EntityManager");
	ZFAssert(m_pkPropertyFactory,	"Entity::Entity(): Failed to find PropertyFactory");
	ZFAssert(m_pkZeroFps,		   "Entity::Entity(): Failed to find ZeroFps");

	// SetDefault Values.
	m_kLocalRotM.Identity();
	m_kLocalPosV= 	Vector3::ZERO;	
	m_kILocalPosV= Vector3::ZERO;	
	m_kVel		= 	Vector3::ZERO;
	m_kAcc		= 	Vector3::ZERO;
	m_fRadius	= 	1;
	
  	SetName("A Entity");
	SetType("Entity");

	m_pScriptFileHandle	= new ZFResourceHandle;

	m_eRole					= NETROLE_AUTHORITY;
	m_eRemoteRole			= NETROLE_PROXY;

	m_iUpdateStatus		= UPDATE_ALL;
	m_bZone					= false;
	m_iCurrentZone			= -1;
	m_bUseZones				= false;
	m_bSave					= true;	
	m_pkParent				= NULL;
	m_bRelativeOri			= false;
	m_bFirstSetPos			= true;
	m_bInterpolate			= true;
	m_iEntityID				= -1;
	m_bSendChilds			= true;
	m_fInterPolateFactor = 10;

	//clear child list
	m_akChilds.clear();	
	
	//reset all oritentation data
	ResetGotData();
	
	//set nr of connections
	SetNrOfConnections(m_pkZeroFps->GetMaxPlayers());
}

Entity::~Entity() 
{
	// First we remove all childs
	DeleteAllChilds();

	// Add Ourself to our Net.DeletList
	AddToDeleteList(m_iEntityID);
	
	// Add our Net.DeleteList to our parent.
	if(m_pkParent != NULL)
	{
		for(unsigned int i=0;i<m_aiNetDeleteList.size();i++)
		{
			m_pkParent->AddToDeleteList(m_aiNetDeleteList[i]);
		}
	}

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
	m_pkEntityManager->UnLink(this);
	
	delete(m_pScriptFileHandle);
}

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
	pkNewProperty->SetEntity(this);
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

	if(strcmp(acName,pProp->m_acName) != 0)
	{
		cout<<"WARNING: Internal property name "<<pProp->m_acName<<" does not match whit registered property name "<<acName<<endl;
	}

	AddProperty(pProp);
	return pProp;
}

/**	\brief	Removes the property from the Entity ( but does not delete it).
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
			
			//add property to entity's delete list
			AddToDeletePropertyList(string(acName));
			
			return true;
		}
		++kIt;
	}
	
	return false;
}

void Entity::PropertyLost(Property* pkProp)
{
	if(pkProp->m_pkEntity == this)
	{
		vector<Property*>::iterator kIt = m_akPropertys.begin();
		while(kIt != m_akPropertys.end())
		{
			(*kIt)->PropertyLost(pkProp);
			++kIt;
		}
		pkProp->m_pkEntity = NULL;
	}	
}


/**	\brief	Returns a pointer to the choosen property.
	
	Returns a pointer the the property 'name' or NULL if the property is not part of the Entity.
*/
Property* Entity::GetProperty(const char* acName) 
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

/**	\brief	Returns a vector with propertys for the Entity and child Entitys.
	
	Returns a vector with the the propertys that have the correct type and side flags.
	Walks child Entitys of selected Entity.
*/
void Entity::GetAllPropertys(vector<Property*> *akPropertys,int iType,int iSide)
{
	//return if no updates shuld be done
	if(m_iUpdateStatus & UPDATE_NONE)
		return;

	//get this Entitys propertys
	GetPropertys(akPropertys,iType,iSide);	
	
	//return if no child updates shuld be done
	if(m_iUpdateStatus & UPDATE_NOCHILDS)
		return;
	
	//go trough all childs and get propertys
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++)
	{	
		(*it)->GetAllPropertys(akPropertys,iType,iSide);		
	}		
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
void Entity::AddChild(Entity* pkEntity) 
{
	// Check so we don't have child already.
	if(HasChild(pkEntity))
		return;

	m_akChilds.push_back(pkEntity);	
	pkEntity->SetParent(this);
}

/**	\brief	Remove a Entity from our list of children.

	Check that we have this Entity as a child and if so removes it.
*/
void Entity::RemoveChild(Entity* pkEntity)
{
	// If we don't have Entity as a child return.
	if(HasChild(pkEntity) == false)
		return;

	//m_akChilds.remove(pkEntity);	
	
	// Remove Entity as child.
	vector<Entity*>::iterator kIt = m_akChilds.begin();
	while(kIt != m_akChilds.end())
	{
		if((*kIt) == pkEntity)
		{
			m_akChilds.erase(kIt);
			break;
		}
		++kIt;
	}
	pkEntity->SetParent(NULL);		// Set Entitys parent to NULL.
}

/**	\brief	Sets Entity to be our parent.
*/
void Entity::SetParent(Entity* pkEntity) 
{
	// Check so we don't try to set ourself to be our own parent.
	if(this == pkEntity)
		return;

	SetNetUpdateFlag(NETUPDATEFLAG_PARENT,true);

	// Remove Parent
	if(pkEntity == NULL) {
		if(m_pkParent == NULL)
			return;
		
		Entity* pkParent = m_pkParent;
		m_pkParent = NULL;
		pkParent->RemoveChild(this);
		return;
		}

	if(m_pkParent == pkEntity)		// Dont do anything if this parent is already set
		return;
	SetParent(NULL);				// Remove Parent.
	
	m_pkParent = pkEntity;
	m_pkParent->AddChild(this);
}

/**	\brief	Returns true if Entity is our child of this.
*/
bool Entity::HasChild(Entity* pkEntity)
{
	//check if this Entity is in the child list
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)==pkEntity) return true;
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
	Entity* pkChildEntity;

	while(m_akChilds.size()) 
	{
		pkChildEntity = (*m_akChilds.begin());
		pkChildEntity->DeleteAllChilds();
		delete pkChildEntity;
	}

	m_akChilds.clear();
}

bool Entity::AttachToZone()
{
	return AttachToZone(GetLocalPosV());
}


bool Entity::AttachToZone(const Vector3& kPos)
{
	if(!m_bZone)
	{
		if(!m_bRelativeOri)
		{		
			int nZ = m_pkEntityManager->GetZoneIndex(kPos,m_iCurrentZone,false);
			
			if(nZ == m_iCurrentZone)
				return true;

			if(nZ == -1)
			{
				//cout<<"Entity tried to move outside zone"<<endl;
				return false;
			}
			
			ZoneData* cz = m_pkEntityManager->GetZoneData(nZ);
			if(!cz)
			{
				//cout<<"zone does not exist"<<endl;
				return  false;
			}
	
			if(cz->m_iStatus != EZS_LOADED)
			{
				//cout<<"Entity tried to move to a unloaded zone"<<endl;
				return false;			
			}
			
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
	//lets fins out wich clients has this entity
	for(list<P_Track*>::iterator it = m_pkEntityManager->m_kTrackedObjects.begin();it!= m_pkEntityManager->m_kTrackedObjects.end();it++)
	{	
		if((*it)->m_iConnectID != -1)
		{
			//if this object is the tracker, dont remove it			
			if((*it)->GetEntity() == this)
				continue;
			
			if(GetExistOnClient((*it)->m_iConnectID))
			{				
				if( (*it)->m_iActiveZones.find(iCurrent) != (*it)->m_iActiveZones.end())
				{
					//cout<<"Connection "<<(*it)->m_iConnectID<<" has this entity"<<endl;				
					if( (*it)->m_iActiveZones.find(iNew) == (*it)->m_iActiveZones.end())
					{
						
						cout<<"Entity "<<m_iEntityID<< " has moved to an untracked zone for client "<<(*it)->m_iConnectID<<endl;
						
						// send delete request to client here =)
						//m_pkEntityManager->SendDeleteEntity((*it)->m_iConnectID,m_iEntityID);
						m_pkEntityManager->AddEntityToClientDeleteQueue((*it)->m_iConnectID,m_iEntityID);
					}		
				}
			}
		}
	}
	
	
	//do callback to all propertys
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		(*it)->ZoneChange(iCurrent,iNew);
	}
}

bool Entity::GetZoneNeighbours(vector<Entity*>* pkZones)
{
	if(!IsZone())
		return false;
		
	if(ZoneData* pkZD = m_pkEntityManager->GetZone(this))
	{
		pkZones->push_back(this);
		
		for(int i = 0;i<pkZD->m_iZoneLinks.size();i++)
			if(ZoneData* pkZD2 =	m_pkEntityManager->GetZoneData(pkZD->m_iZoneLinks[i]))
				if(pkZD2->m_pkZone)
					pkZones->push_back(pkZD2->m_pkZone);
				
		return true;
	}
	
	return false;
}

/**	\brief	return entity + childs , fForceAll = always send all entitys, bCheckSendStatus = check m_bSendChilds
*/

void Entity::GetAllEntitys(vector<Entity*> *pakEntitys,bool bForceAll,bool bCheckSendStatus )
{
	//add this entity?
	if(m_iUpdateStatus & UPDATE_NONE && !bForceAll)
		return;
	
	pakEntitys->push_back(this);	
	
	//add this entitys childs?
	if( (m_iUpdateStatus & UPDATE_NOCHILDS) && !bForceAll)
		return;
	
	if( bCheckSendStatus && !m_bSendChilds)
		return;
	
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) 
		(*it)->GetAllEntitys(pakEntitys,bForceAll);
}




/**	\brief	Returns true if there is any netactive properys in Entity

	Returns true if this Entity needs to be sent over network (if anything has changed since last
	net update).
*/
bool Entity::HaveSomethingToSend(int iConnectionID)
{
	if( m_eRole != NETROLE_AUTHORITY)		return false;
	if( m_eRemoteRole	== NETROLE_NONE)		return false;
	
	bool bNeedUpdate = false;

	bNeedUpdate |= IsAnyNetUpdateFlagTrue(iConnectionID);

	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		if((*it)->bNetwork) 
		{
			bNeedUpdate |= (*it)->GetNetUpdateFlag(iConnectionID);
			
			/*
			if((*it)->GetNetUpdateFlag(iConnectionID))
			{
				cout<<"property wants to send:"<<(*it)->m_acName<<endl;
			
			}
			*/
		}
	}
	
	return bNeedUpdate;
}


/**	\brief	Pack Entity.
*/
void Entity::PackTo(NetPacket* pkNetPacket, int iConnectionID)
{	
	SetExistOnClient(iConnectionID,true);

	//send update flags
	pkNetPacket->Write(m_kNetUpdateFlags[iConnectionID]);
	
	//send parent
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_PARENT))
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_PARENT,false);		
		
		int iParentID	=	-1;
		if(m_pkParent)
			iParentID = m_pkParent->m_iEntityID;

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
		/*
		pkNetPacket->Write((int) m_aiNetDeleteList.size() );

		for(unsigned int i=0; i<m_aiNetDeleteList.size(); i++)
		{
			pkNetPacket->Write((int) m_aiNetDeleteList[i] );
			
			if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_aiNetDeleteList[i]))
				pkEnt->SetExistOnClient(iConnectionID,false);
		}
		*/
	}

	//send deleteproperty list
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_DELETEPROPLIST))
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_DELETEPROPLIST,false);
	
		pkNetPacket->Write((int) m_kNetDeletePropertyList.size() );

		for(unsigned int i=0; i<m_kNetDeletePropertyList.size(); i++)
		{
			pkNetPacket->Write_Str(	m_kNetDeletePropertyList[i]);
		}
	}
	
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_RELPOS) || GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_INTERPOLATE) || GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_ISZONE))
	{
		unsigned char ucEntityFlags = 0;
      if(m_bRelativeOri)	ucEntityFlags |= 1;
      if(m_bInterpolate)	ucEntityFlags |= 2;
		if(m_bZone)				ucEntityFlags |= 4;
		pkNetPacket->Write((unsigned char) ucEntityFlags );
		
		
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_RELPOS		,false);
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_INTERPOLATE	,false);
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_ISZONE		,false);
		
	}


	//send position
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_POS))
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_POS,false);		
		pkNetPacket->Write(GetLocalPosV());
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
	
	//send acceleration
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_ACC))	
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_ACC,false);	
		pkNetPacket->Write(m_kAcc);
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

	//send propertys
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		if((*it)->bNetwork) 
		{
			if((*it)->GetNetUpdateFlag(iConnectionID))
			{	
				pkNetPacket->Write_Str((*it)->m_acName);
				(*it)->PackTo(pkNetPacket,iConnectionID);
				
				//dvoid test, sätter automatiskt nätverks flaggan till false för propertyt
				(*it)->SetNetUpdateFlag(iConnectionID,false);
			}
		}
	}
	
	//end whit and empty property name so that client knows theres no more propertys
	pkNetPacket->Write_Str("");
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
		int iParentID	=	-1;
		
		pkNetPacket->Read( iParentID );		
		Entity* parent = m_pkEntityManager->GetEntityByID(iParentID);
		if(!parent)
			parent = m_pkEntityManager->CreateEntityByNetWorkID(iParentID);
		SetParent(parent);
		LOGSIZE("Entity::ParentID", 4);
	}
   
   // get update status
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_UPDATESTATUS))
	{
	   pkNetPacket->Read( m_iUpdateStatus );		   
	}

	//get delete list
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_DELETE))
	{	/*
		int iNumDelEntitys;
		Entity* pkNetSlave;	
		
		pkNetPacket->Read(iNumDelEntitys);
		
		for(int i=0; i<iNumDelEntitys; i++) 
		{
			int iDelEntityID;
			pkNetPacket->Read(iDelEntityID );
			pkNetSlave = m_pkEntityManager->GetEntityByID(iDelEntityID);
			m_pkEntityManager->Delete(pkNetSlave);
		}
		*/
	}

	//get deleteproperty list
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_DELETEPROPLIST))
	{
		int iSize; 
		string strPropertyName;
		
		pkNetPacket->Read(iSize );

		for(unsigned int i=0; i<iSize; i++)
		{
			pkNetPacket->Read_Str(strPropertyName);
			
			cout<<"deleting property:"<<strPropertyName<<" from entity "<<m_strName<<endl;
			DeleteProperty(strPropertyName.c_str());
		}
	}	
	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_RELPOS) || GetNetUpdateFlag(0,NETUPDATEFLAG_INTERPOLATE) || GetNetUpdateFlag(0,NETUPDATEFLAG_ISZONE) )
	{
		unsigned char ucEntityFlags = 0;
		pkNetPacket->Read((unsigned char&) ucEntityFlags );
		
		/*
		if(GetNetUpdateFlag(0,NETUPDATEFLAG_RELPOS))			m_bRelativeOri = ucEntityFlags & 1;
		if(GetNetUpdateFlag(0,NETUPDATEFLAG_INTERPOLATE))	m_bInterpolate = ucEntityFlags & 2;
		if(GetNetUpdateFlag(0,NETUPDATEFLAG_ISZONE))			m_bZone 			= ucEntityFlags & 4;
		*/
		
		m_bRelativeOri = ucEntityFlags & 1;
		m_bInterpolate = ucEntityFlags & 2;
		m_bZone 			= ucEntityFlags & 4;
	}
	
/*	//get rel position flag
	if(GetNetUpdateFlag(0, NETUPDATEFLAG_RELPOS))
	{
		pkNetPacket->Read((int&) m_bRelativeOri );
		LOGSIZE("Entity::RelativePos", 4);
	}

	//get interpolation flag
	if(GetNetUpdateFlag(0, NETUPDATEFLAG_INTERPOLATE))
	{
		pkNetPacket->Read((int&) m_bInterpolate );
		LOGSIZE("Entity::Interpolate", 4);
	}*/

	//get position
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_POS))
	{
		Vector3 kPos;
		pkNetPacket->Read(kPos);
		SetLocalPosV(kPos);
		LOGSIZE("Entity::Position", sizeof(kPos) );
	}
	
	//get rotation	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_ROT))
	{
		Matrix3 kRot;
		pkNetPacket->Read(kRot);
		SetLocalRotM(kRot);
		LOGSIZE("Entity::Rotation", sizeof(kRot));
	}
	
	//get velocity	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_VEL))
	{
		Vector3 kVel;
		pkNetPacket->Read(kVel);
		SetVel(kVel);
		LOGSIZE("Entity::Velocity", sizeof(kVel));
	}
	
	//get acceleration	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_ACC))
	{
		Vector3 kAcc;
		pkNetPacket->Read(kAcc);
		SetAcc(kAcc);
		LOGSIZE("Entity::Acceleration", sizeof(kAcc));
	}
	
	//get radius
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_RADIUS))	
	{
		pkNetPacket->Read(m_fRadius);
		LOGSIZE("Entity::Radius", sizeof(m_fRadius));
	}
	
	//get name
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_NAME))	
	{
		char szStr[256];
		pkNetPacket->Read_Str(szStr);
		m_strName = szStr;		
		LOGSIZE("Entity::Name", strlen(szStr) + 1);
	}	
	
	//get type
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_TYPE))	
	{
		char szStr[256];
		pkNetPacket->Read_Str(szStr);
		m_strType = szStr;		
		LOGSIZE("Entity::Type", strlen(szStr) + 1);
	}		
	
	// Read first property name
	int iPropertyStart = pkNetPacket->m_iPos;
	int iPropettyEnd;

	char szProperty[256];
	pkNetPacket->Read_Str(szProperty);

	while(strcmp(szProperty,"") != 0) 
	{
		//create proxy property
		Property* pProp  = AddProxyProperty(szProperty);
		if(pProp) 
			pProp->PackFrom(pkNetPacket, ZF_NET_NOCLIENT);		//load property
		else 
		{
			cout <<"Error in netpacket" << endl;			
			pkNetPacket->SetError(true);
			return;
		}

		iPropettyEnd = pkNetPacket->m_iPos;
		LOGSIZE(szProperty, iPropettyEnd - iPropertyStart);

		//get next property name
		iPropertyStart = pkNetPacket->m_iPos;
		pkNetPacket->Read_Str(szProperty);
	}	

	int iEnd = pkNetPacket->m_iPos;
	
	m_pkEntityManager->m_iTotalNetEntityData += (iEnd - iStart);
	m_pkEntityManager->m_iNumOfNetEntitys ++;
}

/**	\brief	Load Entity.
*/
void Entity::Load(ZFIoInterface* pkFile,bool bLoadID,bool bLoadChilds)
{
	Vector3 pos;
	Matrix3 rot;

	int iNewID;
	pkFile->Read(&iNewID,sizeof(iNewID),1);	
	if(bLoadID)
	{
		m_pkEntityManager->Link(this,iNewID);
	}
	else
		m_pkEntityManager->Link(this);	

	pkFile->Read(&m_bRelativeOri,sizeof(m_bRelativeOri),1);	
	pkFile->Read(&m_bInterpolate,sizeof(m_bInterpolate),1);	
	
	pkFile->Read(&pos,sizeof(pos),1);	
	pkFile->Read(&rot,sizeof(rot),1);	
	
	SetLocalRotM(rot);
	SetLocalPosV(pos);
	
	pkFile->Read(&m_kVel,sizeof(m_kVel),1);	
	pkFile->Read(&m_kAcc,sizeof(m_kAcc),1);	
	pkFile->Read(&m_fRadius,sizeof(m_fRadius),1);		
	
	pkFile->Read(&m_iUpdateStatus,sizeof(m_iUpdateStatus),1);
	pkFile->Read(&m_bSave,sizeof(m_bSave),1);		
	
	pkFile->Read(&m_bZone,sizeof(m_bZone),1);			
	pkFile->Read(&m_bUseZones,sizeof(m_bUseZones),1);				
	
	pkFile->Read(&m_eRole,sizeof(m_eRole),1);		
	pkFile->Read(&m_eRemoteRole,sizeof(m_eRemoteRole),1);				

	char acTemp[128];

	int i;

	int iNumOfEntVars;
	pkFile->Read(&iNumOfEntVars,sizeof(iNumOfEntVars), 1);		
	EntityVariable kEntVar;

	for(i=0; i<iNumOfEntVars; i++) 
	{
		pkFile->Read(acTemp,128,1);		
		kEntVar.m_strName = acTemp;
		pkFile->Read(&kEntVar.m_eType ,sizeof(int), 1);		
		pkFile->Read(&kEntVar.m_fValue ,sizeof(kEntVar.m_fValue), 1);		
		pkFile->Read(acTemp,128,1);		
		kEntVar.m_strValue = acTemp;
		m_kVariables.push_back(kEntVar);
	}

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
	
	//nr of propertys
	int iProps = 0;
	pkFile->Read(&iProps,sizeof(iProps),1);	
	//load all propertys
	for(i = 0;i< iProps;i++)
	{
		char name[50];		
		pkFile->Read(&name,50,1);
		int iVersion;
		pkFile->Read(&iVersion,sizeof(int),1);
					
		
		Property* prop = AddProperty(name);
		
		if(prop)
			prop->Load(pkFile,iVersion);
		else
		{
			cout<<"ERROR: Entity name: "<<m_strName<<" type:"<<m_strType<<" has unregistered property: "<<name<<endl;
			return;
		}
	}
		
	//Do we want to load the entitys attached childs?
	if(bLoadChilds)
	{
		//nr of childs
		int iChilds = 0;		
		pkFile->Read(&iChilds,sizeof(iChilds),1);		
	
		//load all childs
		for( i = 0; i < iChilds; i++ )
		{
			Entity* newobj = m_pkEntityManager->CreateEntity(false);
			newobj->SetParent(this);
			newobj->Load(pkFile,bLoadID);		
		}
	}
	
	//reset alla update flags for this Entity
	ResetAllNetUpdateFlags();
}

/**	\brief	Save Entity.
*/
void Entity::Save(ZFIoInterface* pkFile)
{
	Vector3 pos = GetLocalPosV();
	Matrix3 rot = GetLocalRotM();

	pkFile->Write(&m_iEntityID,sizeof(m_iEntityID),1);	
	
	pkFile->Write(&m_bRelativeOri,sizeof(m_bRelativeOri),1);	
	pkFile->Write(&m_bInterpolate,sizeof(m_bInterpolate),1);	
	
	pkFile->Write(&pos,sizeof(pos),1);	
	pkFile->Write(&rot,sizeof(rot),1);	
	
	pkFile->Write(&m_kVel,sizeof(m_kVel),1);	
	pkFile->Write(&m_kAcc,sizeof(m_kAcc),1);	
	pkFile->Write(&m_fRadius,sizeof(m_fRadius),1);		
	
	pkFile->Write(&m_iUpdateStatus,sizeof(m_iUpdateStatus),1);
	pkFile->Write(&m_bSave,sizeof(m_bSave),1);		
	
	pkFile->Write(&m_bZone,sizeof(m_bZone),1);			
	pkFile->Write(&m_bUseZones,sizeof(m_bUseZones),1);				
	
	pkFile->Write(&m_eRole,sizeof(m_eRole),1);		
	pkFile->Write(&m_eRemoteRole,sizeof(m_eRemoteRole),1);				

	char acTemp[128];
	unsigned int i;

	// Write Ent Vars
	unsigned int iNumOfEntVars = m_kVariables.size();
	pkFile->Write(&iNumOfEntVars,sizeof(iNumOfEntVars), 1);		
	
	for(i=0; i<iNumOfEntVars; i++) 
	{
		strcpy(acTemp,m_kVariables[i].m_strName.c_str());
		pkFile->Write(acTemp,128,1);		
		pkFile->Write(&m_kVariables[i].m_eType ,sizeof(int), 1);		
		pkFile->Write(&m_kVariables[i].m_fValue ,sizeof(m_kVariables[i].m_fValue), 1);		
		strcpy(acTemp,m_kVariables[i].m_strValue.c_str());
		pkFile->Write(acTemp,128,1);		
	}

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
		
		
	//nr of propertys
	unsigned int iProps = m_akPropertys.size();		
	pkFile->Write(&iProps,sizeof(iProps),1);	
	//save all propertys
	for( i = 0;i<iProps;i++)
	{
		pkFile->Write(&m_akPropertys[i]->m_acName,50,1);	
		pkFile->Write(&m_akPropertys[i]->m_iVersion,sizeof(int),1);	
		m_akPropertys[i]->Save(pkFile);
	}
		
		
	//count number of childs to save
	unsigned int iChilds = 0;
	
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
		PublishEvent(Msg);
		}

	m_kGameMessages.clear();
}

/// Send a GameMessage to all Entity propertys.
void Entity::PublishEvent(GameMessage& Msg)
{
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
		(*it)->OnEvent(Msg);
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

	TabIn(pos);			g_ZFObjSys.Logf("fisklins", "Entity[%d]\n", m_iEntityID);
	TabIn(pos);			g_ZFObjSys.Logf("fisklins", "{\n" );

	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Name = %s\n", GetName().c_str() );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "ObjType = %s\n", m_strType.c_str() );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Pos = <%f,%f,%f>\n", GetWorldPosV().x, GetWorldPosV().y, GetWorldPosV().z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Rot = <%f,%f,%f>\n", GetWorldRotV().x, GetWorldRotV().y, GetWorldRotV().z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Vel = <%f,%f,%f>\n", m_kVel.x, m_kVel.y, m_kVel.z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "m_kAcc = <%f,%f,%f>\n", m_kAcc.x, m_kAcc.y, m_kAcc.z );
	TabIn(pos + 3);	g_ZFObjSys.Logf("fisklins", "Name = %s\n", m_strType.c_str() );

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


void Entity::SetVel(const Vector3& kVel)
{
	if(kVel == m_kVel)
		return;
		
	m_kVel = kVel;
	
	SetNetUpdateFlag(NETUPDATEFLAG_VEL,true);
}

void Entity::SetAcc(const Vector3& kAcc)
{
	if(m_kAcc == kAcc)
		return;
		
	m_kAcc = kAcc;
	
	SetNetUpdateFlag(NETUPDATEFLAG_ACC,true);
}


void Entity::SetRelativeOri(bool bRO)
{	
	m_bRelativeOri = bRO;		
	SetNetUpdateFlag(NETUPDATEFLAG_RELPOS, true);
};

void Entity::ResetChildsGotData()
{
	ResetGotData();
	
	for(vector<Entity*>::iterator it=m_akChilds.begin();it!=m_akChilds.end();it++) {
		if((*it)->GetRelativeOri())
			(*it)->ResetChildsGotData();	
	}	
}

void Entity::SetLocalRotM(const Matrix3& kNewRot)
{
	if(kNewRot == m_kLocalRotM)
		return;

	ResetChildsGotData();
	SetNetUpdateFlagAndChilds(NETUPDATEFLAG_ROT,true);	
	
	m_kLocalRotM = kNewRot;
}

void Entity::SetLocalRotM(const Matrix4& kNewRot)
{
	Matrix3 kMat;
	kMat = kNewRot;
	SetLocalRotM(kMat);
}


void Entity::SetLocalRotV(const Vector3& kRot)
{
	ResetChildsGotData();

   SetNetUpdateFlagAndChilds(NETUPDATEFLAG_ROT,true);
	
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

void Entity::SetLocalPosV(const Vector3& kPos)
{
	//check new zone
	if(m_bUseZones)
	{
		if(!AttachToZone(kPos))
			return;
	}

	//return if pos is the same
	if(kPos == m_kLocalPosV)
		return;

	ResetChildsGotData();
	
	SetNetUpdateFlagAndChilds(NETUPDATEFLAG_POS,true);
	
	m_kLocalPosV = kPos;
	
	if(m_bFirstSetPos)						//if the pos has never been set, the set oldpos to the new one
	{
		m_kILocalPosV=m_kLocalPosV;
		m_bFirstSetPos=false;
	}
}

void Entity::SetWorldPosV(const Vector3& kPos)
{
	if(m_bRelativeOri)
	{
		Vector3 kDiff = kPos - GetWorldPosV();
		Vector3 newlocalpos = m_kLocalPosV + kDiff;
		SetLocalPosV(newlocalpos);
	}
	else
		SetLocalPosV(kPos);
}

void Entity::RotateLocalRotV(const Vector3& kRot)
{
	ResetChildsGotData();
	SetNetUpdateFlagAndChilds(NETUPDATEFLAG_ROT,true);
	
	m_kLocalRotM.Rotate(kRot);	
}


Vector3 Entity::GetLocalRotV()
{
	if(!m_kGotOrientationData[LOCAL_ROT_V])
	{
		m_kLocalRotV = m_kLocalRotM.GetRotVector();
		m_kGotOrientationData[LOCAL_ROT_V] = true;
	}

	return m_kLocalRotV;
}

Vector3 Entity::GetLocalPosV()
{	
	return m_kLocalPosV;
}

Matrix3 Entity::GetLocalRotM()
{
	return m_kLocalRotM;
}

Vector3 Entity::GetIWorldPosV()
{
	//if there has been no setpos return current position
	if(m_bFirstSetPos)
		return GetWorldPosV();

	if(m_bInterpolate)
	{	
		/*
		float fATime = m_pkZeroFps->GetTicks() - m_pkZeroFps->GetLastGameUpdateTime();		
		float fI = (fATime / m_pkZeroFps->GetSystemUpdateFpsDelta());
		//if( fI > 1.0)
		//	fI = 1.0;		
		//cout<<fI<<endl;
		m_kILocalPosV = GetWorldPosV() + m_kVel * (m_pkEntityMan->GetSimDelta() * fI);
		*/
		
		//m_kILocalPosV += (GetWorldPosV() - m_kILocalPosV)/15.0;// * (m_pkZeroFps->GetFrameTime()*3);
		 //m_kILocalPosV += (GetWorldPosV() -m_kILocalPosV ) * (1.0 - fI);
		//m_kILocalPosV += (GetWorldPosV() - m_kILocalPosV ) / 10.0;
		/*
		Vector3 kDir = GetWorldPosV() - m_kILocalPosV;
		m_kILocalPosV += kDir * m_pkZeroFps->GetFrameTime();
		*/

		float fFac = m_fInterPolateFactor * m_pkZeroFps->GetFrameTime();
		if(fFac > 1.0)
			fFac = 1.0;
					
		m_kILocalPosV += (GetWorldPosV() - m_kILocalPosV) * fFac;
		
		return m_kILocalPosV;
	}
	else
	{
		//still calculate the interpolatet position, but return non interpolatet position
		m_kILocalPosV += (GetWorldPosV() - m_kILocalPosV)/5.0;// * (m_pkZeroFps->GetFrameTime()*3);
		return GetWorldPosV();
	}

}

Vector3 Entity::GetWorldPosV()
{
	if(!m_kGotOrientationData[WORLD_POS_V])
	{
		if(m_bRelativeOri)
		{
			//check if we have a parent else use local pos
			if(m_pkParent)
			{
				m_kWorldPosV  = GetWorldOriM().GetPosVector();
				m_kGotOrientationData[WORLD_POS_V] = true;
			}
			else
			{
				m_kWorldPosV = m_kLocalPosV;
				m_kGotOrientationData[WORLD_POS_V] = true;			
			}
		
		}
		else
		{
			m_kWorldPosV = m_kLocalPosV;
			m_kGotOrientationData[WORLD_POS_V] = true;
		}
	}
	
	return m_kWorldPosV;
}

Matrix3 Entity::GetWorldRotM()
{
	if(!m_kGotOrientationData[WORLD_ROT_M])
	{	
		if(m_bRelativeOri)
		{
			//check if we have a parent else use local pos
			if(m_pkParent)
			{
				m_kWorldRotM  = m_kLocalRotM * m_pkParent->GetWorldRotM() ;
				m_kGotOrientationData[WORLD_ROT_M] = true;
			}
			else
			{
				m_kWorldRotM = m_kLocalRotM;
				m_kGotOrientationData[WORLD_ROT_M] = true;			
			}
		
		}
		else
		{
			m_kWorldRotM = m_kLocalRotM;
			m_kGotOrientationData[WORLD_ROT_M] = true;
		}
		
	}
	
	return m_kWorldRotM;
}

Vector3 Entity::GetWorldRotV()
{
	if(!m_kGotOrientationData[WORLD_ROT_V])
	{	
		m_kWorldRotV = GetWorldRotM().GetRotVector();
		m_kGotOrientationData[WORLD_ROT_V] = true;
	}

	return m_kWorldRotV;
}

Matrix4 Entity::GetWorldOriM()
{
	if(!m_kGotOrientationData[WORLD_ORI_M])
	{	
		if(m_bRelativeOri)
		{
			if(m_pkParent)
			{			
				m_kWorldOriM = GetLocalOriM() * m_pkParent->GetWorldOriM() ;
				m_kGotOrientationData[WORLD_ORI_M] = true;						
			}
			else
			{
				m_kWorldOriM = GetLocalOriM();
				
				m_kGotOrientationData[WORLD_ORI_M] = true;			
			}		
		}
		else
		{
			m_kWorldOriM = GetLocalOriM();
			
			m_kGotOrientationData[WORLD_ORI_M] = true;
		}	
	}
	
	return m_kWorldOriM;

}

Matrix4 Entity::GetLocalOriM()
{
	if(!m_kGotOrientationData[LOCAL_ORI_M])
	{	
		m_kLocalOriM = m_kLocalRotM;
		m_kLocalOriM.Translate(m_kLocalPosV);
	}
	
	return m_kLocalOriM;
}



void	Entity::SetNrOfConnections(int iConNR)
{
	m_kNetUpdateFlags.resize(iConNR);
	ResetAllNetUpdateFlags();
	
	m_kExistOnClient.resize(iConNR);
}

void	Entity::SetExistOnClient(int iConID,bool bStatus)
{
	if(iConID < 0 || iConID >= m_kExistOnClient.size())
		return;
		
	//cout<<"E:"<<m_iEntityID<<" con:"<<iConID<<" "<<bStatus<<endl;
	if(bStatus == false)
		cout<<"setting false entity:"<<m_iEntityID<<endl;
		
	m_kExistOnClient[iConID] = bStatus;
}

bool	Entity::GetExistOnClient(int iConID)
{
	if(iConID < 0 || iConID >= m_kExistOnClient.size())
		return false;
	
	return m_kExistOnClient[iConID];
}

void	Entity::ResetAllNetUpdateFlags()
{
	for(int i = 0;i<m_kExistOnClient.size();i++)
		m_kExistOnClient[i] = false;


	for(unsigned int i = 0;i<m_kNetUpdateFlags.size();i++)
	{
		m_kNetUpdateFlags[i].reset();	//reset all bits to false
		m_kNetUpdateFlags[i].flip();  //flip all bits to true
	}

	//reset all propertys
	for(unsigned int j = 0;j<m_akPropertys.size();j++)
	{
		m_akPropertys[j]->ResetAllNetUpdateFlags();
	}
}

void	Entity::ResetAllNetUpdateFlags(int iConID)
{
	m_kExistOnClient[iConID] = false;	
	
	m_kNetUpdateFlags[iConID].reset();	//reset all bits to false
	m_kNetUpdateFlags[iConID].flip();  //flip all bits to true

	//reset all propertys
	for(unsigned int j = 0;j<m_akPropertys.size();j++)
	{
		m_akPropertys[j]->SetNetUpdateFlag(iConID,true);
	}	
}

void	Entity::ResetAllNetUpdateFlagsAndChilds(int iConID)
{
	m_kExistOnClient[iConID] = false;

	m_kNetUpdateFlags[iConID].reset();	//reset all bits to false
	m_kNetUpdateFlags[iConID].flip();  //flip all bits to true
	
	//reset all propertys
	for(unsigned int j = 0;j<m_akPropertys.size();j++)
	{
		m_akPropertys[j]->SetNetUpdateFlag(iConID,true);
	}	
	
	//reset all childs
	for(unsigned int i = 0;i<m_akChilds.size();i++)
	{
		m_akChilds[i]->ResetAllNetUpdateFlagsAndChilds(iConID);
	}	
}

bool	Entity::GetNetUpdateFlag(int iConID,int iFlagID)
{
	return m_kNetUpdateFlags[iConID][iFlagID];
}

bool	Entity::IsAnyNetUpdateFlagTrue(int iConID)
{
	bool bValue = false;

	for(int i = 0; i<MAX_NETUPDATEFLAGS; i++)
	{
		bValue |= m_kNetUpdateFlags[iConID][i];
		
	}
	
	return bValue;
}

void Entity::SetNetUpdateFlag(int iFlagID,bool bValue)
{
	for(unsigned int i = 0;i<m_kNetUpdateFlags.size();i++)
	{
		m_kNetUpdateFlags[i][iFlagID] = bValue;
	}
}

void Entity::SetNetUpdateFlagAndChilds(int iFlagID,bool bValue)
{

	unsigned int i;

	for(i = 0;i<m_kNetUpdateFlags.size();i++)
	{
		m_kNetUpdateFlags[i][iFlagID] = bValue;
	}
	
	//reset all childs
	for(i = 0;i<m_akChilds.size();i++)
	{
		m_akChilds[i]->SetNetUpdateFlagAndChilds(iFlagID,bValue);
	}	
	
}

void	Entity::SetNetUpdateFlag(int iConID,int iFlagID,bool bValue)
{
	m_kNetUpdateFlags[iConID][iFlagID] = bValue;
}


void Entity::SetUpdateStatus(int iUpdateStatus)
{
	if(m_iUpdateStatus == iUpdateStatus)
		return;

	m_iUpdateStatus = iUpdateStatus; 
	SetNetUpdateFlagAndChilds(NETUPDATEFLAG_UPDATESTATUS,true);
}


void Entity::AddToDeleteList(int iId)
{
	m_aiNetDeleteList.push_back(iId);
	SetNetUpdateFlag(NETUPDATEFLAG_DELETE,true);

}

void	Entity::AddToDeletePropertyList(const string& strName)
{
	m_kNetDeletePropertyList.push_back(strName);
	SetNetUpdateFlag(NETUPDATEFLAG_DELETEPROPLIST,true);
}

void Entity::UpdateDeleteList()
{	
	if(m_aiNetDeleteList.empty())
		return;
	
	/*	
	//make sure that all clients have gotten the delete list
	for(unsigned int i = 0;i < m_kNetUpdateFlags.size();i++)
	{
		if(m_pkEntityManager->m_pkNetWork->IsConnected(i))
		{
			//cout<<"testing con "<<i<<endl;
			if(m_kNetUpdateFlags[i][NETUPDATEFLAG_DELETE] == true)
			{	
				return;
			}
		}
	}
	*/
	
	for(int i = 0;i<m_kExistOnClient.size();i++)
	{
		if(m_kExistOnClient[i])
		{
			if(!m_pkEntityManager->m_pkNetWork->IsConnected(i))
				cout<<"WARNING: stuff existed on an unconnectet client...bad"<<endl;
		
			//if this client has not gotten the delete list , return
			if(m_kNetUpdateFlags[i][NETUPDATEFLAG_DELETE] == true)
			{	
				return;
			}			
		}
	}
	
	
	//clear delete list 
	m_aiNetDeleteList.clear();
}

void	Entity::UpdateDeletePropertyList()
{
	if(m_kNetDeletePropertyList.empty())
		return;

	for(int i = 0;i<m_kExistOnClient.size();i++)
	{
		if(m_kExistOnClient[i])
		{
			if(!m_pkEntityManager->m_pkNetWork->IsConnected(i))
				cout<<"WARNING: stuff existed on an unconnectet client...bad"<<endl;
		
			//if this client has not gotten the delete list , return
			if(m_kNetUpdateFlags[i][NETUPDATEFLAG_DELETEPROPLIST] == true)
			{	
				return;
			}			
		}
	}
	
	m_kNetDeletePropertyList.clear();	
}

void Entity::GetAllVarNames(vector<string>& vkList)
{
	vkList.reserve( m_kVariables.size() + 1 );

	for(unsigned int i=0; i<m_kVariables.size(); i++) {
		vkList.push_back( m_kVariables[i].m_strName ); 
		}	
}


EntityVariable* Entity::CreateVar(const string& strName, EntityVariableType eType)
{
	EntityVariable kEntVar;
	kEntVar.m_eType		= eType;
	kEntVar.m_strName		= strName;
	kEntVar.m_fValue		= 0.0;
	kEntVar.m_strValue	= "";
	m_kVariables.push_back(kEntVar);

	return GetVar(strName);
}

EntityVariable* Entity::GetVar(const string& strName)
{
	for(unsigned int i=0; i<m_kVariables.size(); i++) {
		if(strName == m_kVariables[i].m_strName) 
			return &m_kVariables[i];
		}	

	return NULL;

}

double Entity::GetVarDouble(const string& strName) 
{
	EntityVariable* pkVar = GetVar(strName);
	
	if(pkVar == NULL)
		return 0.0;

	return pkVar->m_fValue;
}

string Entity::GetVarString(const string& strName)
{
	EntityVariable* pkVar = GetVar(strName);
	
	if(pkVar == NULL)
		return string("");

	if(pkVar->m_eType == EVAR_STRING)
		return pkVar->m_strValue;
	if(pkVar->m_eType == EVAR_DOUBLE)
	{
		char szValue[128];
		double dValue = pkVar->m_fValue;
		sprintf(szValue,"%f",dValue);
		return string(szValue);
	}

	return string("");
}

void	 Entity::SetVarDouble(const string& strName, double fValue)
{
	EntityVariable* pkVar = GetVar(strName);
	
	if(pkVar == NULL)
		pkVar = CreateVar(strName, EVAR_DOUBLE);

	pkVar->m_fValue = fValue;
}

void Entity::SetVarString(const string& strName, string strValue)
{
	EntityVariable* pkVar = GetVar(strName);
	
	if(pkVar == NULL)
		pkVar = CreateVar(strName, EVAR_STRING);

	if(pkVar->m_eType == EVAR_STRING)
		pkVar->m_strValue = strValue;

	if(pkVar->m_eType == EVAR_DOUBLE)
	{
		pkVar->m_fValue = atof(strValue.c_str());
	}
}

void Entity::AddVarDouble(const string& strName, double fValueToAdd)
{
	double fD = GetVarDouble(strName);
	fD += fValueToAdd;
	SetVarDouble(strName, fD);
}


void Entity::SetInterpolate(bool bInterpolate)
{
	if(m_bInterpolate == bInterpolate)
		return;

	m_bInterpolate = bInterpolate;

	SetNetUpdateFlag(NETUPDATEFLAG_INTERPOLATE,true);	
}
