#include "../script/zfscript.h"
#include "entity.h"
#include "../basic/zfsystem.h"
#include "entitymanager.h"
#include "zerofps.h"
#include "../engine_systems/propertys/p_track.h"
#include "../engine_systems/script_interfaces/si_objectmanager.h" 
#include "../basic/math.h"

using namespace ObjectManagerLua;

//#define LOGALL

#ifdef LOGALL
	#define LOGSIZE(name, size)	m_pkEntityManager->GetWorldEntity()->AddVarDouble(string(name), size )
#else
	#define LOGSIZE(name, size)	
#endif
 

Entity::Entity() 
{
	// Get Ptrs to some usefull Subsystems.
	m_pkEntityManager		= static_cast<ZSSEntityManager*>(g_ZFObjSys.GetObjectPtr("ZSSEntityManager"));
	m_pkPropertyFactory	= static_cast<ZSSPropertyFactory*>(g_ZFObjSys.GetObjectPtr("ZSSPropertyFactory"));	
	m_pkZeroFps			   = static_cast<ZSSZeroFps*>(g_ZFObjSys.GetObjectPtr("ZSSZeroFps"));
		
	ZFAssert(m_pkEntityManager,	"Entity::Entity(): Failed to find ZSSEntityManager");
	ZFAssert(m_pkPropertyFactory,	"Entity::Entity(): Failed to find PropertyFactory");
	ZFAssert(m_pkZeroFps,		   "Entity::Entity(): Failed to find ZeroFps");
 
	// SetDefault Values.
	m_kLocalRotM.Identity();
	m_kLocalPosV= 	Vector3::ZERO;	
	m_kILocalPosV= Vector3::ZERO;	
	m_kVel		= 	Vector3::ZERO;
	m_kAcc		= 	Vector3::ZERO;
	m_fRadius	= 	1;
	
	m_kLocalAABBMin.Set(0,0,0);
	m_kLocalAABBMax.Set(0,0,0);	
	
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
	m_fInterPolateFactor = 20;						//lower factor = slower moving objects/smoother movements
	m_iLastInterPolateFrame = 0;
	m_ucIcon					= 0;
	m_bHide					= 0;
	
	m_fPriority				= -1;
	m_pkSceneAABBNode		= NULL;
	
	//clear child list
	m_akChilds.clear();	
	
	//reset all oritentation data
	ResetGotData();
	
	//set nr of connections
	SetNrOfConnections(m_pkZeroFps->GetMaxPlayers());
	
	//reset network ignore flags
// 	m_kNetIgnoreFlags.reset();
}

Entity::~Entity() 
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

	// Tell Entity manger that we are no more.
	m_pkEntityManager->UnLink(this);
	
	delete(m_pScriptFileHandle);
}


void Entity::SetLocalAABB(const Vector3& kMin,const Vector3& kMax)
{
	if(kMin == m_kLocalAABBMin && kMax == m_kLocalAABBMax)
		return;

	m_kLocalAABBMin = kMin;
	m_kLocalAABBMax = kMax;
	
	if(m_pkSceneAABBNode)
		m_pkSceneAABBNode->Update(this);
		
}

void Entity::SetLocalAABB(float fRadius)
{
	SetLocalAABB(Vector3(-fRadius,-fRadius,-fRadius),Vector3(fRadius,fRadius,fRadius));
}


/**	\brief	Adds a property to the Entity.
*/
Property* Entity::AddProperty(Property* pkNewProperty) 
{
	if(pkNewProperty == NULL)
		return NULL;
	
	///EVIL GUBB/////
// 	vector<Property*>::iterator kIt = m_akPropertys.begin();
// 	while(kIt != m_akPropertys.end())
// 	{
// 		(*kIt)->PropertyFound(pkNewProperty);
// 		++kIt;
// 	}
	
	
	pkNewProperty->m_pkEntity = this;
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
	
	if(!pProp)	
		return NULL;
	
	return AddProperty(pProp);
}

/**	\brief	Adds a property to an Entity if it does not have it yet.
*/
Property* Entity::AddProxyProperty(const char* acName)
{	
	if(Property* pProp = GetProperty(acName))
		return pProp;
	
	return AddProperty(acName);
}

/**	\brief	Removes the property from the Entity ( but does not delete it).
*/
// void Entity::RemoveProperty(Property* pkProp) 
// {
// 	//m_akPropertys.remove(pkProp);
// 	
// 	///EVIL GUBB///// ---changed by EVIL ZEROM who didn't like it when it crashed
//    for ( vector<Property*>::iterator kIte = m_akPropertys.begin();
//          kIte != m_akPropertys.end(); kIte++ )
//    {
// 		if((*kIte) == pkProp)
//       {
//          m_akPropertys.erase ( kIte );
//          break;
//       }
//    }
// 
// // 	PropertyLost(pkProp);
// }

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
			//add property to property delete list (will be deleted at the end of this frame)
			m_pkEntityManager->m_kPropertyDeleteList.push_back(*kIt);

			(*kIt) = m_akPropertys.back();
			m_akPropertys.pop_back();
			
			//add property to entity's delete list
			AddToDeletePropertyList(string(acName));
			
			return true;
		}
		++kIt;
	}
	
	return false;
}

// void Entity::PropertyLost(Property* pkProp)
// {
// 	if(pkProp->m_pkEntity == this)
// 	{
// 		vector<Property*>::iterator kIt = m_akPropertys.begin();
// 		while(kIt != m_akPropertys.end())
// 		{
// 			(*kIt)->PropertyLost(pkProp);
// 			++kIt;
// 		}
// 		pkProp->m_pkEntity = NULL;
// 	}	
// }


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
	static Property* pkProp;
	
	int iSize = m_akPropertys.size();
	for(int i = 0;i < iSize;i++)
	{
		pkProp = m_akPropertys[i];
		if( (pkProp->m_iType & iType) &&
			 (pkProp->m_iSide & iSide) )
			akPropertys->push_back(pkProp);	
	}
}

bool Entity::IsHidden(bool bCheckParents)
{
	if(m_pkEntityManager->m_bAllowHide && m_bHide)
		return true;

	if(m_pScriptFileHandle->IsValid())
	{
		string strFullType = m_pScriptFileHandle->GetRes().c_str();
		set<string>::iterator it = m_pkEntityManager->m_kEditorHide.find(strFullType);

		if(it != m_pkEntityManager->m_kEditorHide.end())
			return true;
	
		// Check if this is a hidden entity.
		for(set<string>::iterator it = m_pkEntityManager->m_kEditorHide.begin(); it != m_pkEntityManager->m_kEditorHide.end(); it++)
		{
			if(strstr(strFullType.c_str() , (*it).c_str()) )
			{
				return true;
			}
		}
	}
	
	if(bCheckParents)
		return IsAnyParentHidden();

	return false;
}

bool Entity::IsAnyParentHidden()
{
	Entity* pkParent = m_pkParent;	

	while(pkParent)
	{
		if(pkParent->IsHidden(false))
		{
			cout << "I have a hidden parent" << endl;
			return true;
		}

		pkParent = pkParent->m_pkParent;
	}

	return false;
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

	if(iType & PROPERTY_TYPE_RENDER)
	{
		if(m_pkEntityManager->m_bAllowHide && m_bHide)
			return;

		if(IsHidden())
			return;
	}

	//add this Entitys propertys to the vector
	static Property* pkProp;
	int iPropertys = m_akPropertys.size();
	for(int i = 0;i < iPropertys;i++)
	{
		pkProp = m_akPropertys[i];
		if( (pkProp->m_iType & iType) &&
			 (pkProp->m_iSide & iSide) )
			akPropertys->push_back(pkProp);	
	}	

	//return if no child updates shuld be done
	if(m_iUpdateStatus & UPDATE_NOCHILDS)
		return;
	
	//go trough all childs and get propertys
	int iChilds = m_akChilds.size();
	for(int i =0;i<iChilds;i++)
		m_akChilds[i]->GetAllPropertys(akPropertys,iType,iSide);
	
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
	
// 	UpdateAABB();
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
	if(pkEntity == NULL) 
	{
		if(m_pkParent == NULL)
			return;
		
		Entity* pkParent = m_pkParent;
		m_pkParent = NULL;
		pkParent->RemoveChild(this);
		
		if(m_pkSceneAABBNode)
			m_pkSceneAABBNode->RemoveEntity(this);
		
		return;
	}

	if(m_pkParent == pkEntity)		// Dont do anything if this parent is already set
		return;
	SetParent(NULL);				// Remove Parent.
	
	m_pkParent = pkEntity;
	m_pkParent->AddChild(this);
	
	//reset current zone if this is not a zone entity and its not suppose to use zones
	if(!m_bZone && !m_bUseZones)
		m_iCurrentZone = -1;
		
	if(!m_pkSceneAABBNode)
		m_pkEntityManager->m_pkSceneAABBTree->InsertEntity(this);
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
		delete pkChildEntity;
	}

	m_akChilds.clear();
}

bool Entity::InActiveZone()
{
	if(ZoneData* pkZone = m_pkEntityManager->GetZoneData(m_iCurrentZone))
	{
		return pkZone->m_bActive;	
	}

	return true;
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
			
			//cout<<"atached entity:"<<GetEntityID()<<" to new zone "<<nZ<<endl;
			
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
	static P_Track* pkTracker;

	//lets fins out wich clients has this entity
	for(vector<P_Track*>::iterator it = m_pkEntityManager->m_kTrackedObjects.begin();it!= m_pkEntityManager->m_kTrackedObjects.end();it++)
	{
		pkTracker = *it;
		
		if(pkTracker->m_iConnectID != -1)
		{
			//if this object is the tracker, dont remove it			
			if(pkTracker->GetEntity() == this)
				continue;
			
			if(GetExistOnClient(pkTracker->m_iConnectID))
			{				
				if( pkTracker->m_kActiveZones.find(iCurrent) != pkTracker->m_kActiveZones.end())
				{
					//cout<<"Connection "<<(*it)->m_iConnectID<<" has this entity"<<endl;				
					if( pkTracker->m_kActiveZones.find(iNew) == pkTracker->m_kActiveZones.end())
					{
						
						cout<<"Entity "<<m_iEntityID<< " has moved to an untracked zone for client "<<pkTracker->m_iConnectID<<endl;
						
						// send delete request to client here =)
						m_pkEntityManager->AddEntityToClientDeleteQueue(pkTracker->m_iConnectID,m_iEntityID);
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

void Entity::GetChilds(vector<Entity*>*	pkEntitys)
{
	for(int i=0;i<m_akChilds.size();i++)
		pkEntitys->push_back(m_akChilds[i]);

}


/**	\brief	Returns true if there is any netactive properys in Entity

	Returns true if this Entity needs to be sent over network (if anything has changed since last
	net update).
*/
bool Entity::HaveSomethingToSend(int iConnectionID)
{	
	//do we own this entity?
	if( m_eRole != NETROLE_AUTHORITY)
		return false;
	
	
	bool bNeedUpdate = false;
	bool bHasNetPropertys = false;
	bool bHasNetChilds = false;

	//check if theres any networked propertys and if any property wants to send data
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		if((*it)->m_bNetwork) 
		{
			bHasNetPropertys = true;
			//bNeedUpdate |= (*it)->GetNetUpdateFlag(iConnectionID);			
			if((*it)->GetNetUpdateFlag(iConnectionID))
				return true;
		}
	}
	
	//entity want to send  if theres any point in sending
	if(IsAnyNetUpdateFlagTrue(iConnectionID))
	{
		//did it have any network property
		if(bHasNetPropertys)
			return true;
	
		//check if this entity has any value to an editor, if so send it if client is an editor
		if(m_ucIcon && m_pkZeroFps->m_kClient[iConnectionID].m_bIsEditor )
			return true;
			
		//this is a client, clients only sends its own client objects, and we always wants to send that. kind of
		if(!m_pkZeroFps->m_bServerMode)
			return true;
	
		//does any child want to send?  
		if(m_bSendChilds && !GetExistOnClient(iConnectionID))
		{
			for(int i = 0;i<m_akChilds.size();i++)
			{
				//my child is on the client (or gona be) but i am not, i better get there
				if(m_akChilds[i]->HaveSomethingToSend(iConnectionID) || m_akChilds[i]->GetExistOnClient(iConnectionID))
					return true;				
				
// 				if(m_akChilds[i]->HaveSomethingToSend(iConnectionID) || m_akChilds[i]->GetExistOnClient(iConnectionID) && !GetExistOnClient(iConnectionID))
// 					return true;
				
// 				//a child has changed something
// 				if(m_akChilds[i]->HaveSomethingToSend(iConnectionID))
// 				{
// 					return true;
// 				}
			}	
		}			
	}
	
return false;

}


/**	\brief	Pack Entity.
*/
void Entity::PackTo(NetPacket* pkNetPacket, int iConnectionID)
{	
	//cout<<"sending entity "<<GetEntityID()<<" to "<<iConnectionID<<endl;
	
	SetExistOnClient(iConnectionID,true);
	
	//apply ignore flags
	for(int i =0;i<MAX_NETUPDATEFLAGS;i++)
		if(m_kNetIgnoreFlags[iConnectionID][i])
			m_kNetUpdateFlags[iConnectionID][i] = false;
	
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

	
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_RELPOS) || GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_INTERPOLATE) || GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_ISZONE))
	{
		unsigned char ucEntityFlags = 0;
      if(m_bRelativeOri)	ucEntityFlags |= 1;
      if(m_bInterpolate)	ucEntityFlags |= 2;
		if(m_bZone)				ucEntityFlags |= 4;
		pkNetPacket->Write(ucEntityFlags );		
		
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
 		pkNetPacket->Write_Str(m_strName);
	}
	
	//send type
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_TYPE))	
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_TYPE,false);	
		pkNetPacket->Write_Str(m_strType);
		pkNetPacket->Write( m_ucIcon );
	}	

	//send propertys
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		if((*it)->m_bNetwork) 
		{
			if((*it)->GetNetUpdateFlag(iConnectionID))
			{	
				pkNetPacket->Write_Str((*it)->m_acName);
				(*it)->PackTo(pkNetPacket,iConnectionID);
				
				//dvoid test, s�ter automatiskt n�verks flaggan till false f� propertyt
				(*it)->SetNetUpdateFlag(iConnectionID,false);
			}
		}
	}
	
	//end whit and empty property name so that client knows theres no more propertys
	pkNetPacket->Write_Str("");
	
	
	//send deleteproperty list
	if(GetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_DELETEPROPLIST))
	{
		SetNetUpdateFlag(iConnectionID,NETUPDATEFLAG_DELETEPROPLIST,false);
		
		//cout<<"sending delete property list do client:"<<iConnectionID<<" for entity:"<<GetEntityID()<<endl;		
		
		pkNetPacket->Write((char) m_kNetDeletePropertyList.size() );		
		for(unsigned int i=0; i<m_kNetDeletePropertyList.size(); i++)
			pkNetPacket->Write_Str(	m_kNetDeletePropertyList[i]);
	}	
	
}

/**	\brief	Unpack Entity.
*/
void Entity::PackFrom(NetPacket* pkNetPacket, int iConnectionID)
{
 	//cout<<"got entity "<<GetEntityID()<<"  "<<GetName()<<endl;

	int iStart = pkNetPacket->m_iPos;

	//read update flags
	pkNetPacket->Read(m_kNetUpdateFlags[0]);			//con id is 0

	//get parent
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_PARENT))
	{
		int iParentID;		
		pkNetPacket->Read( iParentID );		
		
		Entity* parent = m_pkEntityManager->GetEntityByID(iParentID);			
		if(!parent)
		{
			parent = m_pkEntityManager->CreateEntityByNetWorkID(iParentID);
		}
		
		SetParent(parent);
	
		LOGSIZE("Entity::ParentID", 4);	
	}
   
   // get update status
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_UPDATESTATUS))
	{
		static int iUpdateStatus;	
	   pkNetPacket->Read( iUpdateStatus );		   
		
		m_iUpdateStatus = iUpdateStatus;		
	}


	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_RELPOS) || GetNetUpdateFlag(0,NETUPDATEFLAG_INTERPOLATE) || GetNetUpdateFlag(0,NETUPDATEFLAG_ISZONE) )
	{
		unsigned char ucEntityFlags;
		pkNetPacket->Read(ucEntityFlags );
		
		m_bRelativeOri = ucEntityFlags & 1;
		m_bInterpolate = ucEntityFlags & 2;
		m_bZone 			= ucEntityFlags & 4;		
	}

	//get position
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_POS))
	{
		static Vector3 kPos;		
		pkNetPacket->Read(kPos);
		
		SetLocalPosV(kPos);		
	
		LOGSIZE("Entity::Position", sizeof(kPos) );	
	}
	
	//get rotation	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_ROT))
	{
		static Matrix3 kRot;
		pkNetPacket->Read(kRot);
		
		SetLocalRotM(kRot);
			
		LOGSIZE("Entity::Rotation", sizeof(kRot));
	}
	
	//get velocity	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_VEL))
	{
		static Vector3 kVel;
		pkNetPacket->Read(kVel);
		
		SetVel(kVel);
				
		LOGSIZE("Entity::Velocity", sizeof(kVel));
	}
	
	//get acceleration	
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_ACC))
	{
		static Vector3 kAcc;
		pkNetPacket->Read(kAcc);
		
		SetAcc(kAcc);
			
		LOGSIZE("Entity::Acceleration", sizeof(kAcc));
	}
	
	//get radius
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_RADIUS))	
	{
		static float fRadius;	
		pkNetPacket->Read(fRadius);
		
		m_fRadius = fRadius;		
			
		LOGSIZE("Entity::Radius", sizeof(m_fRadius));
	}
	
	//get name
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_NAME))	
	{
		static string strName;	
		pkNetPacket->Read_Str(strName);
		
		m_strName = strName;
		
		LOGSIZE("Entity::Name", strlen(szStr) + 1);
	}	
	
	//get type
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_TYPE))	
	{
 		static string strType;
		static unsigned char ucIcon;
		
		pkNetPacket->Read_Str(strType);
		pkNetPacket->Read( ucIcon );
		
		m_strType = strType;
		m_ucIcon	 = ucIcon;
		
		if(m_ucIcon != 0 && m_pkZeroFps->m_bEditMode)
			AddProxyProperty("P_EditIcon");
		
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

	
	
	//get deleteproperty list
	if(GetNetUpdateFlag(0,NETUPDATEFLAG_DELETEPROPLIST))
	{
		char cSize; 
		string strPropertyName;
		
		pkNetPacket->Read(cSize );
		for(unsigned int i=0; i<cSize; i++)
		{
			pkNetPacket->Read_Str(strPropertyName);
			
			DeleteProperty(strPropertyName.c_str());
		}
	}		
	
	
	int iEnd = pkNetPacket->m_iPos;
	
	m_pkEntityManager->m_iTotalNetEntityData += (iEnd - iStart);
	m_pkEntityManager->m_iNumOfNetEntitys ++;
}

/**	\brief	Load Entity.
*/
void Entity::Load(ZFIoInterface* pkFile,bool bLoadID,bool bLoadChilds)
{
	static Vector3 pos;
	static Matrix3 rot;

	int iNewID;
	pkFile->Read(iNewID);

	if(bLoadID)
	{
		m_pkEntityManager->Link(this,iNewID);
	}
	else
		m_pkEntityManager->Link(this);	

	pkFile->Read(m_ucIcon);	
	pkFile->Read(m_bRelativeOri);	
	pkFile->Read(m_bInterpolate);	
	
	pkFile->Read(pos);	
	pkFile->Read(rot);	
		
	pkFile->Read(m_kVel);	
	pkFile->Read(m_kAcc);	
	pkFile->Read(m_fRadius);		
	
	pkFile->Read(m_iUpdateStatus);
	pkFile->Read(m_bSave);		
	
	pkFile->Read(m_bZone);			
	pkFile->Read(m_bUseZones);	
	
	SetLocalRotM(rot);					//dvoid flytta ner dessa tv� s� dom hamna efter usezones, annars kopplas odm inte korret till zonen vid laddning
	SetLocalPosV(pos);					//
		
	pkFile->Read(m_eRole);		
	pkFile->Read(m_eRemoteRole);				

	char acTemp[128];

	int i;

	unsigned int iNumOfEntVars;
	pkFile->Read(iNumOfEntVars);		
	EntityVariable kEntVar;

	for(i=0; i<iNumOfEntVars; i++) 
	{
		pkFile->Read(acTemp,128,1);		
		kEntVar.m_strName = acTemp;
		pkFile->Read(kEntVar.m_eType );		
		pkFile->Read(kEntVar.m_fValue );		
		pkFile->Read(acTemp,128,1);		
		kEntVar.m_strValue = acTemp;
		if(kEntVar.m_eType == EVAR_VECTOR)
			pkFile->Read(kEntVar.m_kVector );		
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
	{
		m_pScriptFileHandle->SetRes(string(acTemp));
		if(!m_pScriptFileHandle->IsValid())
			ZFWarning("Entity ID:%d,NAME:%s,TYPE:%s have a non valid entity script (%s)", GetEntityID(), m_strName.c_str(),m_strType.c_str(),acTemp);
	}

	if(!m_pScriptFileHandle->IsValid() && m_strType != string("Entity"))
	{
		//ZFWarning("Entity %d do not have a entity script (Type=%s)", GetEntityID(), m_strType.c_str());
	}


	//nr of propertys
	int iProps = 0;
	pkFile->Read(iProps);	
	//load all propertys
	for(i = 0;i< iProps;i++)
	{
		char name[50];		
		pkFile->Read(name,50,1);
		int iVersion;
		pkFile->Read(iVersion);
		
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
		pkFile->Read(iChilds);		
	
		//load all childs
		for( i = 0; i < iChilds; i++ )
		{
			Entity* newobj = m_pkEntityManager->CreateEntity(false);
			newobj->SetParent(this);
			newobj->Load(pkFile,bLoadID);	
		}
	}
	
	//reset alla update flags for this Entity
	ResetAllNetFlags();
}

/**	\brief	Save Entity.
*/
void Entity::Save(ZFIoInterface* pkFile)
{
	Vector3 pos = GetLocalPosV();
	Matrix3 rot = GetLocalRotM();

	pkFile->Write(m_iEntityID);	
	

	pkFile->Write(m_ucIcon);	
	pkFile->Write(m_bRelativeOri);	
	pkFile->Write(m_bInterpolate);	
	
	pkFile->Write(pos);	
	pkFile->Write(rot);	
	
	pkFile->Write(m_kVel);	
	pkFile->Write(m_kAcc);	
	pkFile->Write(m_fRadius);	
	
	pkFile->Write(m_iUpdateStatus);	
	pkFile->Write(m_bSave);	
	
	pkFile->Write(m_bZone);	
	pkFile->Write(m_bUseZones);	
	
	pkFile->Write(m_eRole);	
	pkFile->Write(m_eRemoteRole);	

	char acTemp[128];
	unsigned int i;

	// Write Ent Vars
	unsigned int iNumOfEntVars = m_kVariables.size();
	pkFile->Write(iNumOfEntVars);	
	

	for(i=0; i<iNumOfEntVars; i++) 
	{
		strcpy(acTemp,m_kVariables[i].m_strName.c_str());
		pkFile->Write(acTemp,128,1);		
		pkFile->Write(m_kVariables[i].m_eType);		
		pkFile->Write(m_kVariables[i].m_fValue);		
		strcpy(acTemp,m_kVariables[i].m_strValue.c_str());
		pkFile->Write(acTemp,128,1);		
		if(m_kVariables[i].m_eType == EVAR_VECTOR)
			pkFile->Write(m_kVariables[i].m_kVector);		
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
	pkFile->Write(iProps);	
	//save all propertys
	for( i = 0;i<iProps;i++)
	{
		pkFile->Write(&m_akPropertys[i]->m_acName,50,1);	
		pkFile->Write(m_akPropertys[i]->m_iVersion);	
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
	pkFile->Write(iChilds);		
	
	//save all childs
	for( i = 0;i<m_akChilds.size();i++)
	{
		if(m_akChilds[i]->GetSave())
			m_akChilds[i]->Save(pkFile);
	}
}

void Entity::Touch(int iId)
{
	// First we call the entity script collision function.
	if(!m_bZone)
	{
		vector<ScriptFuncArg> args(1);
		args[0].m_kType.m_eType = tINT;
		args[0].m_pData = &iId;
		
		
		m_pkEntityManager->CallFunction(this, "Collission",&args);	
//		m_pkEntityManager->CallFunction(this, "Collission",NULL);	
	}

	// Then we call all property collision functions.
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
	string strName = string("on") + Msg.m_Name;
	m_pkEntityManager->CallFunction(this, strName.c_str(), 0);


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
		g_ZFObjSys.Logf("fisklins"," ");
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

void Entity::SetType(const string& strType)
{
	if(m_strType == strType)
		return;
		
	m_strType = strType;
	
	SetNetUpdateFlag(NETUPDATEFLAG_TYPE,true);
}

void Entity::SetName(const string& strName)
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
	
	kRot.x = Math::Clamp(kRot.x,0,360);
	kRot.y = Math::Clamp(kRot.y,0,360);
	kRot.z = Math::Clamp(kRot.z,0,360);

	diff.x = kRot.x - crot.x;
	diff.y = kRot.y - crot.y;	
	diff.z = kRot.z - crot.z;	
	
	diff.x = Math::Clamp(diff.x,0,360);
	diff.y = Math::Clamp(diff.y,0,360);
	diff.z = Math::Clamp(diff.z,0,360);
	
	newlocal.x += diff.x;
	newlocal.y += diff.y;	
	newlocal.z += diff.z;	
	
	SetLocalRotV(newlocal);
}

void Entity::SetLocalPosV(const Vector3& kPos)
{
	//check new zone
	if(m_bUseZones && !m_bRelativeOri)
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
	
	
	if(m_pkSceneAABBNode)
		m_pkSceneAABBNode->Update(this);
	
// 	if(m_pkParent)
// 		m_pkParent->UpdateAABB();
}

void Entity::SetWorldPosV(const Vector3& kPos)
{
	if(m_bRelativeOri)
	{
		SetLocalPosV(m_kLocalPosV + (kPos - GetWorldPosV()) );
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
		
		unsigned int iFrame = m_pkZeroFps->GetCurrentFrame();
		if(m_iLastInterPolateFrame == iFrame)
			return m_kILocalPosV;
		
		m_iLastInterPolateFrame = iFrame; 
		
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
	if(!m_bRelativeOri || !m_pkParent)
	{
		return m_kLocalPosV;
	}
	else if(!m_kGotOrientationData[WORLD_POS_V])
	{
		m_kWorldPosV  = GetWorldOriM().GetPos();
		m_kGotOrientationData[WORLD_POS_V] = true;
	}
	
	return m_kWorldPosV;
/*		
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
	
	return m_kWorldPosV;*/
}

Matrix3 Entity::GetWorldRotM()
{
	if(!m_bRelativeOri || !m_pkParent)
	{
		return m_kLocalRotM;
	}
	else if(!m_kGotOrientationData[WORLD_ROT_M])
	{
		m_kWorldRotM  = m_kLocalRotM * m_pkParent->GetWorldRotM() ;
		m_kGotOrientationData[WORLD_ROT_M] = true;
	}
	
	return m_kWorldRotM;	
	
	
/*	
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
	
	return m_kWorldRotM;*/
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
	m_kNetIgnoreFlags.resize(iConNR);
	m_kExistOnClient.resize(iConNR);
	m_kLastSent.resize(iConNR);

	ResetAllNetFlags();
}

void	Entity::SetExistOnClient(int iConID,bool bStatus)
{
	if(iConID < 0 || iConID >= m_kExistOnClient.size())
		return;
		
	//cout<<"E:"<<m_iEntityID<<" con:"<<iConID<<" "<<bStatus<<endl;
	m_kExistOnClient[iConID] = bStatus;
}

bool	Entity::GetExistOnClient(int iConID)
{
	if(iConID < 0 || iConID >= m_kExistOnClient.size())
		return false;
	
	return m_kExistOnClient[iConID];
}

void	Entity::ResetAllNetFlags()
{
	for(int i = 0;i<m_kLastSent.size();i++)
		m_kLastSent[i] = -1;


	for(int i = 0;i<m_kExistOnClient.size();i++)
		m_kExistOnClient[i] = false;

	for(unsigned int i = 0;i<m_kNetIgnoreFlags.size();i++)
	{
		m_kNetIgnoreFlags[i].reset();	//reset all bits to false
	}
		

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

void	Entity::ResetAllNetFlags(int iConID)
{
	m_kLastSent[iConID] = -1;
	m_kExistOnClient[iConID] = false;	

	m_kNetIgnoreFlags[iConID].reset();	//reset all bits to false
		
	m_kNetUpdateFlags[iConID].reset();	//reset all bits to false
	m_kNetUpdateFlags[iConID].flip();  //flip all bits to true

	//reset all propertys
	for(unsigned int j = 0;j<m_akPropertys.size();j++)
	{
		m_akPropertys[j]->SetNetUpdateFlag(iConID,true);
	}	
	
}

void	Entity::ResetAllNetFlagsAndChilds(int iConID)
{
	m_kLastSent[iConID] = -1;
	m_kExistOnClient[iConID] = false;

	m_kNetIgnoreFlags[iConID].reset();	//reset all bits to false
	
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
		m_akChilds[i]->ResetAllNetFlagsAndChilds(iConID);
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
		if( ((iFlagID == NETUPDATEFLAG_POS) || (iFlagID == NETUPDATEFLAG_ROT)) && m_akChilds[i]->m_bRelativeOri )
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



void	Entity::AddToDeletePropertyList(const string& strName)
{
	m_kNetDeletePropertyList.push_back(strName);
	SetNetUpdateFlag(NETUPDATEFLAG_DELETEPROPLIST,true);
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

void Entity::Edit_GetDataNames(vector<string>& vkList)
{
	vkList.reserve( 5 );
	vkList.push_back(string( "position" ));
	vkList.push_back(string( "name" ));
	vkList.push_back(string( "EntityID" ));
	vkList.push_back(string( "Type" ));
	vkList.push_back(string( "Rotation" ));
}

string Entity::Edit_GetDataString(const string& strName)
{
	char szString[256];

	if(strName == string("position"))
	{
		Vector3 kPos = GetWorldPosV();
		sprintf(szString, "%f %f %f", kPos.x, kPos.y, kPos.z); 
		return string(szString);
	}

	if(strName == string("name"))
	{
		return this->m_strName;
	}

   if(strName == string("EntityID"))
	{
		sprintf(szString, "%d", m_iEntityID); 
		return string(szString);
	}

	if(strName == string("type"))
	{
		return this->m_strType;
	}

	if(strName == string("rotation"))
	{
		Vector3 kPos = GetLocalRotV();
		sprintf(szString, "%f %f %f", kPos.x, kPos.y, kPos.z); 
		return string(szString);
	}

	return string("");
}

void Entity::Edit_SetDataString(const string& strName, string strValue)
{
	Vector3 kVec;

	if(strName == string("position"))
	{
		sscanf(strValue.c_str(),"%f %f %f",&kVec.x, &kVec.y, &kVec.z);
		SetWorldPosV(kVec);
	}

	if(strName == string("rotation"))
	{
		sscanf(strValue.c_str(),"%f %f %f",&kVec.x, &kVec.y, &kVec.z);
		SetLocalRotV(kVec);
	}

	if(strName == string("name"))
	{
		SetName(strValue);
	}
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
	kEntVar.m_kVector    = Vector3::ZERO;
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
	if(pkVar->m_eType == EVAR_VECTOR)
	{
		char szValue[128];
		sprintf(szValue,"%f %f %f",pkVar->m_kVector.x,pkVar->m_kVector.y,pkVar->m_kVector.z);
		return string(szValue);
	}


	return string("");
}

Vector3 Entity::GetVarVector(const string& strName)
{
	EntityVariable* pkVar = GetVar(strName);
	
	if(pkVar == NULL)
		return Vector3::ZERO;

	if(pkVar->m_eType == EVAR_STRING)
		return Vector3::ZERO;
	if(pkVar->m_eType == EVAR_DOUBLE)
		return Vector3::ZERO;

	return pkVar->m_kVector;
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

	if(pkVar->m_eType == EVAR_VECTOR)
	{
		sscanf(strValue.c_str(),"%f %f %f",&pkVar->m_kVector.x, &pkVar->m_kVector.y, &pkVar->m_kVector.z);
	}
}

void Entity::SetVarVector(const string& strName, Vector3 kVec)	
{
	EntityVariable* pkVar = GetVar(strName);
	
	if(pkVar == NULL)
		pkVar = CreateVar(strName, EVAR_VECTOR);

	if(pkVar->m_eType == EVAR_STRING)
		return;
	if(pkVar->m_eType == EVAR_DOUBLE)
		return;

	if(pkVar->m_eType == EVAR_VECTOR)
		pkVar->m_kVector = kVec;
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











/* ********************************** SCRIPT INTERFACE ****************************************/
/** \class SIEntity 
   \ingroup NewSi
   
	Scriptfunctions that modify one entity. Almost all of them takes entity to modify as first parameter.
*/

/**	\var EntityId
 		\relates SIEntity
		\brief This is a id to a entity.

		Bhla Bhla Bhla Bhla
*/



namespace SI_Entity
{
// Property Mangment ******************************************************************************************************

/**	\fn AddProperty(Entity, PropertyName)
 	\relates SIEntity
   \brief Gives a Entity a property.
   \param Entity Entity to add property to.
   \param PropertyName Name of property to assign to object.

	Adds the property to the entity with def parameters. 
*/
int AddPropertyLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

/*	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	Entity* pkObject = g_pkObjMan->GetEntityByID((int)dTemp);	
	if(!pkObject)
		return 0;*/

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	ObjectManagerLua::g_kScriptState.g_pkLastProperty = pkEnt->AddProperty(acName);
	return 1;
}		

/**	\fn DelProperty(Entity, PropertyName)
 	\relates SIEntity
   \brief Removes a property from a entity (NOT STABLE).
   \param Entity Entity to remove property to.
   \param PropertyName Name of property to remove from object.

	Adds the property to the entity with def parameters. 
*/
int DelPropertyLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

/*	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	Entity* pkObject = g_pkObjMan->GetEntityByID((int)dTemp);	
	if(!pkObject)
		return 0;*/

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	pkEnt->DeleteProperty( acName );
	return 1;
}		


/**	\fn SetParameter(Entity, Property, szName, szValue )
 	\relates SIEntity
   \brief Sets the value of a variable in a property.
   \param Entity Entity to modify.
   \param Property The property whith the parameter we wish to set.
   \param szName Name of variable to set.
   \param szValue Value to set variable to.

	Sets the variable to a given value on the given entity and property. If
	entity or property is non valid nothing will happen. 
*/
int SetParameterLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 4))
		return 0;

	/*double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
	Entity* pkObject = g_pkObjMan->GetEntityByID((int)dTemp);	
	if(!pkObject)
		return 0;*/

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	char acProperty[50];
	g_pkScript->GetArg(pkLua, 1, acProperty);
	Property* pkProp = pkEnt->GetProperty(acProperty);
	if(!pkProp)
	{
		g_pkScript->Error(pkLua, "Warning: Entity %d dont have property %s", pkEnt->GetEntityID(), acProperty);
		return 0;
	}

	//char acName[50];
	string strName;
	g_pkScript->GetArgString(pkLua, 2, strName);
	
	//char acData[50];
	string strData;
	g_pkScript->GetArgString(pkLua, 3, strData);
	
	if(!pkProp->SetValue(strName,strData))
		cout<<"Error setting parameter:"<<strName<<" to "<<strData<<endl;
	return 0;
}

/**	\fn GetParameter(Entity, Property, szName )
 	\relates SIEntity
   \brief Get the value of a variable in a property.
   \param Entity Entity to get.
   \param Property The property whith the parameter we wish to get.
   \param szName Name of variable to get.

	Gets the variable from the given entity and property. If
	entity or property is non valid nothing will happen. 
*/
int GetParameterLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 3))
		return 0;

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	char acProperty[50];
	g_pkScript->GetArg(pkLua, 1, acProperty);
	Property* pkProp = pkEnt->GetProperty(acProperty);
	if(!pkProp)
	{
		g_pkScript->Error(pkLua, "Warning: Entity %d dont have property %s", pkEnt->GetEntityID(), acProperty);
		return 0;
	}

	string strName;
	g_pkScript->GetArgString(pkLua, 2, strName);
	
	string strValue = pkProp->GetValue(strName);
	
	g_pkScript->AddReturnValue(pkLua, (char*)strValue.c_str(), strValue.size());

	return 1;
}
// Entity Values ******************************************************************************************************
/**	\fn GetObjectType( Entity )
 		\relates SIEntity
		\param Entity Id of entity to get type of.
		\brief Returns the type of the entity
*/
int GetObjectTypeLua(lua_State* pkLua)
{
	int iId = ObjectManagerLua::g_kScriptState.g_iCurrentObjectID;
	
	//get id
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iId = (int)dTemp;
	}
	
	//get object
	Entity*	pkObj = g_pkObjMan->GetEntityByID(iId);
	
	if(pkObj)
		g_pkScript->AddReturnValue(pkLua,(char*)pkObj->GetType().c_str(),pkObj->GetType().size());
	
	return 1;
}

/**	\fn GetObjectName( Entity )
 		\relates SIEntity
		\param Entity Id of entity to get name of.
		\brief Returns the name of the entity.
*/
int GetObjectNameLua(lua_State* pkLua)
{
	int iId = ObjectManagerLua::g_kScriptState.g_iCurrentObjectID;
	
	//get id
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iId = (int)dTemp;
	}
	
	//get object
	Entity*	pkObj = g_pkObjMan->GetEntityByID(iId);
	
	if(pkObj)
		g_pkScript->AddReturnValue(pkLua,(char*)pkObj->GetName().c_str(),pkObj->GetName().size());
	
	return 1;
}

/**	\fn SetObjectName( Entity , name)
 		\relates SIEntity
		\param Entity Id of entity to get name of.
		\param Name name of entity.
*/
int SetObjectNameLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1 && g_pkScript->GetNumArgs(pkLua) != 2)
	{
		cerr<<"WARNING: SetObjectNameLua - wrong number of parameters 1 or 2"<<endl;
		return 0;
	}

	int iId = ObjectManagerLua::g_kScriptState.g_iCurrentObjectID;
	string strName;
	
	//get id
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iId = (int)dTemp;
		g_pkScript->GetArgString(pkLua,1,strName);
	}
	else
	{
		g_pkScript->GetArgString(pkLua, 0, strName);
	}
	
	//get object
	if(Entity*	pkObj = g_pkObjMan->GetEntityByID(iId))
		pkObj->SetName( strName);
	
	return 0;
}

/**	\fn SetEditIcon( Entity, iIcon )
 		\relates SIEntity
		\param Entity Id of entity to get name of.
		\brief Returns the name of the entity.
*/
int SetEditIcon(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	// Get ObjectID ID
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	g_pkScript->GetArgNumber(pkLua, 1, &dTemp);		
	int iIcon = (int) dTemp;

	Entity* o1 = g_pkObjMan->GetEntityByID(iId1);
	if(!o1)
		return 1;

	o1->m_ucIcon = iIcon;
	o1->AddProxyProperty("P_EditIcon");

	return 1;	
}

// Entity Variables ******************************************************************************************************
/**	\fn IsLocalSet( EntityID, VariableName )
 		\relates SIEntity
		\param EntityID entity to check.
		\param VariableName Name of variable to look for.
		\brief Returns true if variable exist in entity.
*/
int IsLocalSet(lua_State* pkLua)
{
	int iRetValue = 0;
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	Entity* pkEnt = GetEntityPtr(pkLua, 0);

	// Get Variable Name
	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	if(!pkEnt)
	{
		g_pkScript->AddReturnValue( pkLua, iRetValue );
		return 1;
	}
	
	EntityVariable* pkVar = pkEnt->GetVar(acName);
	if(pkVar)
		iRetValue = 1;
   
	g_pkScript->AddReturnValue( pkLua, iRetValue );
	
	return 1;
}

/**	\fn SetLocal(EntityID, VariableName, Value)
 	\relates SIEntity
   \brief Set value of a variable stored in Entity.
   \param EntityID Entity to set variable on.
   \param VariableName Name of variable.
   \param Value New value to set variable to.

	There are three of these functions. SetLocalDouble, SetLocalString and SetLocalVector. They take a 
	number, string and table with numbers as last parameter each.

	If the variable don't exist it will be created with std values. 0.0 for Double, empty string for String
	and zero vector for Vector.
*/

/**	\fn GetLocal( EntityId Entity , VariableName)
 	\relates SIEntity
   \brief Get value of a variable stored in Entity.
   \param Entity Entity to get variable from.
   \param VariableName Name of variable.
	\return Return Return value of variable

	There are three of these functions. GetLocalDouble, GetLocalString and GetLocalVector. They take a 
	number, string and table with numbers as last parameter each.

	If no variable with the given name is found a std value will be returned.
*/
int GetLocalDouble(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	// Get Variable Name
	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	string arne = acName;
	double dValue = pkEnt->GetVarDouble(arne);
	//printf("GetLocalDouble Entity[%d] = %s is %f\n", iId1, acName, dValue);
	g_pkScript->AddReturnValue(pkLua, dValue);
	return 1;

}

int SetLocalDouble(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 3))
		return 0;

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	// Get Variable Name
	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 2, &dTemp);		
	float fValue = (float) dTemp;

	//printf("SetLocalDouble Entity[%d] = %s is set to %f \n", iId1, acName,fValue);
	string arne = acName;
	pkEnt->SetVarDouble(arne, fValue);
	return 1;	
}

int GetLocalString(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	// Get Variable Name
	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	string arne = acName;
	string strValue = pkEnt->GetVarString(arne);

	g_pkScript->AddReturnValue(pkLua,(char*)strValue.c_str(),strValue.size());
	return 1;
}

int SetLocalString(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua,3))
		return 0;

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	// Get Variable Name
	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	char acValue[100];
	g_pkScript->GetArg(pkLua, 2, acValue);

	string arne = acName;
	pkEnt->SetVarString(arne, string(acValue));
	return 1;	
}

int SetLocalVector(lua_State* pkLua)
{
	if( !(g_pkScript->GetNumArgs(pkLua) == 3 || g_pkScript->GetNumArgs(pkLua) == 5) )
	{
		cout<<"SetLocalVector takes 3 or 5 argument(s)"<<endl;
		return 0;
	}	

	// Get ObjectID ID
	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId1 = (int)dTemp;

	// Get Variable Name
	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	
	Vector3 kVector;
	
	if(g_pkScript->GetNumArgs(pkLua) == 3)
		kVector = GetVectorArg(pkLua, 3);
	else
	{
		double x,y,z;
		g_pkScript->GetArgNumber(pkLua, 2, &x);		
		g_pkScript->GetArgNumber(pkLua, 3, &y);		
		g_pkScript->GetArgNumber(pkLua, 4, &z);		
		kVector.Set(x,y,z);
	}	
	
	if(Entity* o1 = g_pkObjMan->GetEntityByID(iId1))
		o1->SetVarVector(acName, kVector);
		
	return 0;	
}

int GetLocalVector(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	// Get Variable Name
	char acName[100];
	g_pkScript->GetArg(pkLua, 1, acName);

	Vector3 pos = pkEnt->GetVarVector(acName);

		vector<TABLE_DATA> vkData;

		TABLE_DATA temp;

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = pos.x;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = pos.y;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = pos.z;
		vkData.push_back(temp);

		g_pkScript->AddReturnValueTable(pkLua, vkData);

	return 1;	
}

// Position/Rotation ******************************************************************************************************

/**	\fn SetObjectRotToIdentityLua( TargetEntity, SourceEntity)
 	\relates SIEntity
   \brief Sets the rotation of an entity to that of the seccond entity
   \param TargetEntity Entity to set rotation on
   \param SourceEntity Entity to take rotation from
*/
int SetObjectRotToIdentityLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 1))
		return 0;

	if(Entity* pkTargetEnt = GetEntityPtr(pkLua, 0))
	{
		static Matrix3 kIdentity;
		kIdentity.Identity();
		
		pkTargetEnt->SetLocalRotM(kIdentity);
	}
	else
	{
		cout<<"WARNING, missing entity when calling SetObjectRotToIdentityLua"<<endl;
	}

	return 1;
}


/**	\fn SetObjectRotFromObjectLua( TargetEntity, SourceEntity)
 	\relates SIEntity
   \brief Sets the rotation of an entity to that of the seccond entity
   \param TargetEntity Entity to set rotation on
   \param SourceEntity Entity to take rotation from
*/
int SetObjectRotFromObjectLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	Entity* pkTargetEnt = GetEntityPtr(pkLua, 0);	
	Entity* pkSourceEnt = GetEntityPtr(pkLua, 1);	
		
	if(pkTargetEnt && pkSourceEnt)
	{
		pkTargetEnt->SetLocalRotM(pkSourceEnt->GetLocalRotM());
	}
	else
	{
		cout<<"WARNING, missing entity when calling SetObjectRotFromObjectLua"<<endl;
	}

	return 1;
}


/**	\fn SetObjectPos( Entity, Pos)
 	\relates SIEntity
   \brief Sets the position of the Entity
   \param Entity Entity to set position on
   \param Pos Name of variable.
*/
int SetObjectPosLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	Entity* pkEnt = GetEntityPtr(pkLua, 0);
	if(!pkEnt)	return 0;

	if(pkEnt)
	{
		Vector3 kPos;
		kPos = GetVectorArg(pkLua, 2);
		/*vector<TABLE_DATA> vkData;
		g_pkScript->GetArgTable(pkLua, 2, vkData); // f�sta argumetet startar p�1

		pkObject->SetWorldPosV( Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData)) );*/
		pkEnt->SetWorldPosV(kPos);
		//g_pkScript->DeleteTable(vkData);
	}

	return 1;
}

/**	\fn SetObjectLocalPos(x,y,z)
 	\relates SIEntity
	\brief Sets the local pos of the last object.
*/
int SetLocalPosLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 2))
		return 0;

	if(Entity* pkEnt = GetEntityPtr(pkLua, 0))
	{
		Vector3 kPos;
		kPos = GetVectorArg(pkLua, 2);
	
		pkEnt->SetLocalPosV(kPos);
		return 0;
	}
		
	return 0;
}

/**	\fn GetObjectPos( Entity)
 	\relates SIEntity
   \brief Returns position of entity
*/
int GetObjectPosLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 1))
		return 0;

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId = (int) dTemp;

	Entity* pkObject = g_pkObjMan->GetEntityByID(iId);

	if(pkObject)
	{
		Vector3 pos = pkObject->GetWorldPosV();

		vector<TABLE_DATA> vkData;

		TABLE_DATA temp;

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = pos.x;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = pos.y;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = pos.z;
		vkData.push_back(temp);

		g_pkScript->AddReturnValueTable(pkLua, vkData);
	}

	return 1;
}

/**	\fn SetObjectPos( Entity)
 	\relates SIEntity
   \brief Returns rotation of entity
*/
int GetObjectRotLua(lua_State* pkLua)
{
	if(!g_pkScript->VerifyArg(pkLua, 1))
		return 0;

	/*if(g_pkScript->GetNumArgs(pkLua) != 1)
	{
		printf("Script funtion GetObjectRot failed! Expects 1 arguments.\n");
		return 0;
	}*/

	double dTemp;
	g_pkScript->GetArgNumber(pkLua, 0, &dTemp);		
	int iId = (int) dTemp;

	Entity* pkObject = g_pkObjMan->GetEntityByID(iId);

	if(pkObject)
	{
		Vector3 kRot = pkObject->GetWorldRotV();

		vector<TABLE_DATA> vkData;

		TABLE_DATA temp;

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = kRot.x;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = kRot.y;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = kRot.z;
		vkData.push_back(temp);

		g_pkScript->AddReturnValueTable(pkLua, vkData);
	}

	return 1;
}


}



void Register_SIEntityProperty(ZSSZeroFps* pkZeroFps)
{
	// Register Property Script Interface
	g_pkScript->ExposeFunction("AddProperty",			SI_Entity::AddPropertyLua);
	g_pkScript->ExposeFunction("SetParameter",		SI_Entity::SetParameterLua);
	g_pkScript->ExposeFunction("GetParameter",		SI_Entity::GetParameterLua);
	g_pkScript->ExposeFunction("DelProperty",		   SI_Entity::DelPropertyLua);		

	// Entity Values
	g_pkScript->ExposeFunction("GetObjectType",		SI_Entity::GetObjectTypeLua);		
	g_pkScript->ExposeFunction("GetObjectName",		SI_Entity::GetObjectNameLua);		
	g_pkScript->ExposeFunction("SetObjectName",		SI_Entity::SetObjectNameLua);	
	g_pkScript->ExposeFunction("SetEditIcon",			SI_Entity::SetEditIcon);

	// Entity Variables

	g_pkScript->ExposeFunction("IsLocalSet",			SI_Entity::IsLocalSet);
	g_pkScript->ExposeFunction("GetLocalDouble",		SI_Entity::GetLocalDouble);
	g_pkScript->ExposeFunction("SetLocalDouble",		SI_Entity::SetLocalDouble);
	g_pkScript->ExposeFunction("GetLocalString",		SI_Entity::GetLocalString);
	g_pkScript->ExposeFunction("SetLocalString",		SI_Entity::SetLocalString);
	g_pkScript->ExposeFunction("SetLocalVector",		SI_Entity::SetLocalVector);
	g_pkScript->ExposeFunction("GetLocalVector",		SI_Entity::GetLocalVector);

	// Position/Rotation
	g_pkScript->ExposeFunction("SetObjectRotToIdentity",		SI_Entity::SetObjectRotToIdentityLua);
	g_pkScript->ExposeFunction("SetObjectRotFromObject",		SI_Entity::SetObjectRotFromObjectLua);
	g_pkScript->ExposeFunction("SetObjectPos",		SI_Entity::SetObjectPosLua);
	g_pkScript->ExposeFunction("SetObjectLocalPos",		SI_Entity::SetLocalPosLua);
	g_pkScript->ExposeFunction("GetObjectPos",		SI_Entity::GetObjectPosLua);
	g_pkScript->ExposeFunction("GetObjectRot",		SI_Entity::GetObjectRotLua);


}












//graveyard





/*
void Entity::AddToDeleteList(int iId)
{
	m_aiNetDeleteList.push_back(iId);
	SetNetUpdateFlag(NETUPDATEFLAG_DELETE,true);

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
	/
	
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

*/




/*

void Entity::GetRenderPropertys(vector<Property*> *akPropertys,bool bNoChilds)
{
	//return if no updates shuld be done
	if(m_iUpdateStatus & UPDATE_NONE)
		return;

	int iChilds = m_akChilds.size();

	//cull
// 	if(m_bZone && iChilds > 0) 
// 	{	
// // 		if(!m_pkZeroFps->GetCam()->GetFrustum()->PointInFrustum(GetWorldPosV()))
// // 			return;
// 	
// 		if(ZoneData* pkData = m_pkEntityManager->GetZoneData(m_iCurrentZone))
// 		{	
// 			float fRad = sqrt(pkData->m_kSize.x*pkData->m_kSize.x + pkData->m_kSize.y*pkData->m_kSize.y + pkData->m_kSize.z*pkData->m_kSize.z) / 2.0;
// 			if(!m_pkZeroFps->GetCam()->GetFrustum()->SphereInFrustum(GetWorldPosV(),fRad * 1.5))
// 				return;
// 		}
// 	}

	//add this Entitys propertys to the vector
	static Property* pkProp;
	int iPropertys = m_akPropertys.size();
	for(int i = 0;i < iPropertys;i++)
	{
		pkProp = m_akPropertys[i];
		if( (pkProp->m_iType & PROPERTY_TYPE_RENDER) &&
			 (pkProp->m_iSide & PROPERTY_SIDE_CLIENT) )
			akPropertys->push_back(pkProp);	
	}	
	
	//return if no child updates shuld be done
	if(m_iUpdateStatus & UPDATE_NOCHILDS || bNoChilds)
		return;
	
	
	//go trough all childs and get propertys
	for(int i =0;i<iChilds;i++)
		m_akChilds[i]->GetRenderPropertys(akPropertys);	
}*/