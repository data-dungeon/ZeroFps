#include "entitymanager.h"
#include "network.h"
#include "zerofps.h"
#include "../basic/zfsystem.h"
#include "../basic/simplescript.h"
#include "../engine_systems/propertys/p_primitives3d.h"
#include "../engine_systems/propertys/p_track.h"
#include "../engine_systems/propertys/p_mad.h"
#include "../engine_systems/propertys/p_tcs.h"
#include "fh.h"
#include "../engine_systems/script_interfaces/si_objectmanager.h"
#include "../engine_systems/script_interfaces/si_properties.h"
#include "p_pfmesh.h"
 


ZoneData::ZoneData()
{
	Clear();
}

void ZoneData::Clear()
{
	m_iStatus = 		EZS_UNUSED;

	m_bNew = 			false;
	m_pkZone =			NULL;
	m_iZoneID = 		-1;
	
	m_fInactiveTime = 0;
	m_bTracked = 		false;
	m_iZoneObjectID = -1;
	m_iRange = 			0;		
	m_iRevision = 		0;

	m_kPos.Set(0,0,0);
	m_kSize.Set(0,0,0);
	
		
	m_strEnviroment = "Default";
}

bool ZoneData::IsInside(Vector3 kPoint)
{
	Vector3 half;
	half.Set(m_kSize.x/2,m_kSize.y/2,m_kSize.z/2);

	if(kPoint.x < (m_kPos.x - half.x))	return false;
	if(kPoint.y < (m_kPos.y - half.y))	return false;
	if(kPoint.z < (m_kPos.z - half.z))	return false;

	if(kPoint.x > (m_kPos.x + half.x))	return false;
	if(kPoint.y > (m_kPos.y + half.y))	return false;
	if(kPoint.z > (m_kPos.z + half.z))	return false;

	return true;
}

EntityManager::EntityManager() 
: ZFSubSystem("EntityManager") 
{
	// Set Default values
	m_iNextEntityID			= 0;
	m_iTotalNetEntityData	= 0;
	m_iNumOfNetEntitys		= 0;
	m_bDrawZones				= false;
	m_bDrawZoneConnections	= false;
	m_iTrackerLOS				= 25;	
	m_iObjectDistance			= 50;
	m_fZoneUnloadTime			= 3;	
	m_iMaxZoneIO 				= 1;	

	m_pkWorldEntity			= NULL;
	m_pkZoneEntity				= NULL;
	m_pkClientEntity			= NULL;
	m_pkGlobalEntity			= NULL;

	m_fSimTime					= 0;			
	m_fSimTimeScale			= 1.0;	

	// Register Variables
	RegisterVariable("l_showzones",			&m_bDrawZones,					CSYS_BOOL);
	RegisterVariable("l_showconn",			&m_bDrawZoneConnections,	CSYS_BOOL);
	RegisterVariable("e_simspeed",			&m_fSimTimeScale,				CSYS_FLOAT);
	RegisterVariable("l_trackerlos",			&m_iTrackerLOS,				CSYS_INT);	
	RegisterVariable("l_zoneunloadtime",	&m_fZoneUnloadTime,			CSYS_FLOAT);	
	RegisterVariable("l_maxzoneio",			&m_iMaxZoneIO,					CSYS_INT);	
	RegisterVariable("l_objectdistance",	&m_iObjectDistance,			CSYS_FLOAT);		

	// Register Commands
	Register_Cmd("o_logtree",		FID_LOGOHTREE);	
	Register_Cmd("o_dumpp",			FID_LOGACTIVEPROPERTYS);	
	Register_Cmd("sendmsg",			FID_SENDMESSAGE,	CSYS_FLAG_SRC_ALL, "sendmsg name id",2);	
	Register_Cmd("newworld",		FID_NEWWORLD,		CSYS_FLAG_SRC_ALL);	
	Register_Cmd("loadworld",		FID_LOADWORLD,		CSYS_FLAG_SRC_ALL);	
	Register_Cmd("saveworld",		FID_SAVEWORLD,		CSYS_FLAG_SRC_ALL);		
	Register_Cmd("setworlddir",	FID_SETWORLDDIR,	CSYS_FLAG_SRC_ALL);		
}

bool EntityManager::StartUp()	
{
	// Get Subsystems.
	m_pkZeroFps		=	static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));		
	m_pkNetWork		=	static_cast<NetWork*>(GetSystem().GetObjectPtr("NetWork"));
	m_pkScript		=	static_cast<ZFScriptSystem*>(GetSystem().GetObjectPtr("ZFScriptSystem"));
	m_pkBasicFS		=	static_cast<ZFBasicFS*>(GetSystem().GetObjectPtr("ZFBasicFS"));		
	m_pkRender		=	static_cast<Render*>(GetSystem().GetObjectPtr("Render"));
	m_pkPropertyFactory	= static_cast<PropertyFactory*>(g_ZFObjSys.GetObjectPtr("PropertyFactory"));	
	m_pkZShaderSystem		= static_cast<ZShaderSystem*>(GetSystem().GetObjectPtr("ZShaderSystem"));
	
	m_fEndTimeForceNet		= m_pkZeroFps->GetEngineTime();

	m_kWorldDirectory = "worldtemp";

	//create all base Entitys
	Clear();

	//setup script interface
	ObjectManagerLua::Init(this,m_pkScript);
	PropertiesLua::Init(m_pkScript, this);
	
	return true; 
}

bool EntityManager::ShutDown() 
{ 
	Clear();

	return true; 
}

bool EntityManager::IsValid()	{ return true; }

EntityManager::~EntityManager() 
{
	float fAvgObjSize = -1;

	if(m_iNumOfNetEntitys) {
		fAvgObjSize = float(m_iTotalNetEntityData / m_iNumOfNetEntitys);
		}

	Logf("net", " Avg Obj Size: %f\n", fAvgObjSize);

	// Obs! Här skall resursen laddas ur, går dock inte pga timeout expire.
	//delete m_pScriptFileHandle;

}


void EntityManager::UpdateSimTime()
{
	//calculate new scaled sim time delta
	m_fSimTimeDelta = m_fSimTimeScale * m_pkZeroFps->GetSystemUpdateFpsDelta();
	
	//add sim time delta to current sim time
	m_fSimTime += m_fSimTimeDelta;
}


Entity* EntityManager::GetEntityByType(const char* czType)
{
	vector<Entity*> kEntitys;		
	GetAllEntitys(&kEntitys);
	
	for(vector<Entity*>::iterator it=kEntitys.begin();it!=kEntitys.end();it++) 
	{
		if((*it)->GetType() == czType)
		{
			return (*it);
		}
	}		
	return NULL;
}


/**	\brief	Link this to the Entity manager

  This function is called by Entitys as they are created. It assigned a EntityID to the Entity and
  also put them in the EntityManager.
*/
void EntityManager::Link(Entity* pkEntity,int iId) 
{
	if(pkEntity->m_iEntityID != -1)
	{
		if(IsLinked(pkEntity))
		{
			cout<<"WARNING: Entity is already linked"<<endl;
			return;	
		}
	}

	if(iId == -1)
	{
		pkEntity->m_iEntityID = m_iNextEntityID++;
	}
	else
	{
		if(GetEntityByID(iId))
		{
			cout<<"WARNING: "<<GetNumOfEntitys()<<" Entity whit id:"<<iId<<" already exist"<<" setting new id "<<m_iNextEntityID<<endl;
			pkEntity->m_iEntityID = m_iNextEntityID++;			
		}
		else	
			pkEntity->m_iEntityID = iId;
	}
		
	m_akEntitys[pkEntity->m_iEntityID] = pkEntity; 
}

bool EntityManager::IsLinked(Entity* pkEntity)
{
	if(m_akEntitys.find(pkEntity->m_iEntityID) == m_akEntitys.end())
		return false;
	else
		return true;
}

/**	\brief	UnLink this from EntityManager.

  Remvoves Entity from the EntityManager.
*/
void EntityManager::UnLink(Entity* pkEntity) 
{	
	// If i own Entity mark so we remove it on clients.
	//	if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
	//		m_aiNetDeleteList.push_back(pkObject->m_iEntityID);
	//m_akObjects.remove(pkObject);
	m_akEntitys.erase(pkEntity->m_iEntityID);
}

/**	\brief	Delete all Entitys.

	Clear all data from EntityManager.
*/
void EntityManager::Clear()
{
	//delete all Entitys in world
	while(m_akEntitys.begin() != m_akEntitys.end())
		delete((*m_akEntitys.begin()).second);
	
	
	//clear all zones
	m_kZones.clear();
	
	// Recreate base Entitys
	CreateBaseEntitys();
	
}

/**	\brief	Create the top level Entitys.

*/
void EntityManager::CreateBaseEntitys()
{
	m_iNextEntityID = 0;
	
	//top world Entity parent to all Entitys
	m_pkWorldEntity						=	CreateEntity();	
	m_pkWorldEntity->SetName("WorldObject");
	m_pkWorldEntity->m_eRole			= NETROLE_AUTHORITY;
	m_pkWorldEntity->m_eRemoteRole	= NETROLE_NONE;

	//Entity that is parent to all zones
	m_pkZoneEntity						=	CreateEntity();	
	m_pkZoneEntity->SetParent(m_pkWorldEntity);
	m_pkZoneEntity->SetName("ZoneObject");
	m_pkZoneEntity->m_eRole				= NETROLE_AUTHORITY;
	m_pkZoneEntity->m_eRemoteRole		= NETROLE_NONE;

	//Entity that is parent to all client Entity
	m_pkClientEntity						=	CreateEntity();	
	m_pkClientEntity->SetParent(m_pkWorldEntity);
	m_pkClientEntity->SetName("ClientObject");
	m_pkClientEntity->m_eRole			= NETROLE_AUTHORITY;
	m_pkClientEntity->m_eRemoteRole	= NETROLE_NONE;

	//Entity that is parent to all global Entity (server information etc)
	m_pkGlobalEntity						=	CreateEntity();	
	m_pkGlobalEntity->SetParent(m_pkWorldEntity);
	m_pkGlobalEntity->SetName("GlobalObject");
	m_pkGlobalEntity->m_eRole			= NETROLE_AUTHORITY;
	m_pkGlobalEntity->m_eRemoteRole	= NETROLE_NONE;

	m_iNextEntityID = 100000;
}



/**	\brief	Creates a new clean Entity.

	Creates a basic Entity without any propertys and all values set to defualt. 
*/
Entity* EntityManager::CreateEntity(bool bLink)
{
	Entity* pkObj = new Entity;
	
	if(bLink)
		Link(pkObj);
	
	return pkObj;
}

/**	\brief	Adds an Entity to delete qeue
*/
void EntityManager::Delete(int iNetworkID) 
{
   if( Entity* pkE = GetEntityByID(iNetworkID))
      Delete(pkE);
}


void EntityManager::Delete(Entity* pkEntity) 
{
	if(pkEntity == NULL)
		return;

	for(vector<int>::iterator it=m_aiDeleteList.begin();it!=m_aiDeleteList.end();it++) 
	{
		if(pkEntity->m_iEntityID == (*it)) {
			Logf("net", "Entity [%d] already in delete list\n", pkEntity->m_iEntityID);
			//cout << "Entity already in delete list" << endl;
			return;
		}
	}
	
	//add to delete list
	m_aiDeleteList.push_back(pkEntity->m_iEntityID);
	
	
	//if where the owner of this entity also add entity to clients delete lists
	if(pkEntity->m_eRole == NETROLE_AUTHORITY)
	{
		for(int i = 0;i<m_pkZeroFps->GetMaxPlayers();i++)
		{
			//only add to clients who is suppose to have the entity
			if(pkEntity->GetExistOnClient(i))
			{
				AddEntityToClientDeleteQueue(i,pkEntity->GetEntityID());
			}
		}
	}
}
 
/**	\brief	

	Walk DeleteList and delete all Entitys in it.
*/
void EntityManager::UpdateDelete()
{
//	int iSize = m_aiDeleteList.size();

	if(m_aiDeleteList.size()==0) 
		return;
	
	for(vector<int>::iterator it=m_aiDeleteList.begin();it!=m_aiDeleteList.end();it++) 
	{
		Entity* pkEntity = GetEntityByID((*it));

		if(pkEntity) { // If i own object mark so we remove it on clients.
			/*if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
				m_aiNetDeleteList.push_back((*it));*/
			delete pkEntity;		
			}
	}

	m_aiDeleteList.clear();

}


/**	\brief	Run update on selected propertys.

	This function collects all propertys that fit the selected flags, sorts them if it needs to
	and then runs the update function of each of this propertys.
*/
void EntityManager::Update(int iType,int iSide,bool bSort,Entity* pkRootEntity,bool bForceRootOnly)
{
	m_iUpdateFlags = iType | iSide;
	
	//clear property  list
	m_akPropertys.clear();	

	//get propertys
	if(bForceRootOnly)
	{
		if(pkRootEntity)
			pkRootEntity->GetPropertys(&m_akPropertys,iType,iSide);
		else
			m_pkWorldEntity->GetPropertys(&m_akPropertys,iType,iSide);
	}
	else
	{	
		if(pkRootEntity)
			pkRootEntity->GetAllPropertys(&m_akPropertys,iType,iSide);
		else
			m_pkWorldEntity->GetAllPropertys(&m_akPropertys,iType,iSide);
	}
	
	//logg stuff		
	m_iNrOfActivePropertys = m_akPropertys.size();	
	m_pkZeroFps->DevPrintf("om", "OM::Update(%s, %s,%d) = %d",
		GetPropertyTypeName(iType),GetPropertySideName(iSide),bSort,m_iNrOfActivePropertys);

	//sort all propertys
	if(bSort)
		stable_sort(m_akPropertys.begin(),m_akPropertys.end(),Less_Property);
	
	//run updat ein all propertys
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
		(*it)->Update();
}

bool EntityManager::IsUpdate(int iFlags)
{
	if(m_iUpdateFlags & iFlags)
		return true;

	return false;
}




void EntityManager::UpdateGameMessages(void)
{
/*	// Let Objects/Propertys handle messages
	for(list<Entity*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		(*it)->HandleMessages();
	}*/
}

Entity* EntityManager::CreateEntityByNetWorkID(int iNetID)
{
	Entity *pkNew = CreateEntity(false);

	Link(pkNew,iNetID);
	
	pkNew->SetParent(m_pkWorldEntity);
	pkNew->m_eRole			= NETROLE_PROXY;
	pkNew->m_eRemoteRole	= NETROLE_AUTHORITY;
	pkNew->SetUseZones(false);
	pkNew->SetInterpolateFactor(2.0);
	
	return pkNew;
}

/**	\brief	Uses a script to create the Entity.

	Creates a Entity from a script and use it to set values and propertys. If script file
	is not found no Entity will be created. 
*/
Entity* EntityManager::CreateEntityFromScriptInZone(const char* acName,Vector3 kPos,int iCurrentZone)
{
	int id = GetZoneIndex(kPos,iCurrentZone,false);
	
	if(id == -1)
		return NULL;
	
	//force loading of this zone
	LoadZone(id);
	
	if(m_kZones[id].m_iStatus != EZS_LOADED)
		return NULL;
	
	Entity* newobj = CreateEntityFromScript(acName);
	
	if(newobj)
	{      
		newobj->SetUseZones(true);
		newobj->SetWorldPosV(kPos);	
		if(newobj->m_iCurrentZone == -1)
			cout<<"Error! Tried to create a Entity outside zones."<<endl;
	}

	
	return newobj;
}

Entity* EntityManager::CreateEntityFromScript(const char* acName)
{
	ZFResourceHandle	kScriptFileHandle;
	
	if(!kScriptFileHandle.SetRes(acName))
	{
		cout<<"WARNING: Failed to load entityscript script "<<acName<<endl;;
		return NULL;
	}
	
	//push all pointers
	ObjectManagerLua::Push();
	
	if(!m_pkScript->Call(&kScriptFileHandle, "Create", 0, 0))
	{	
		cout<<"WARNING: Failed to call Create funktion in entity script "<<acName<<endl;
		//pop pointers and return
		ObjectManagerLua::Pop();	
		return NULL;
	}
	
	//find last /
	int len = strlen(acName);
	int pos = 0;
	for(int i = len;i > 0 ;i--)
	{
		if(acName[i] == '/')
		{	
			pos = i+1;
			break;
		}
	}

	//copy return entity pointer
	Entity* pkReturnObj = ObjectManagerLua::g_pkReturnObject;
	
	//setup entity
	pkReturnObj->m_strType	= &acName[pos];
	pkReturnObj->m_strName	= string("A ") + &acName[pos];
	pkReturnObj->m_pScriptFileHandle->SetRes(acName);
   pkReturnObj->m_strCreatedFromScript = acName;
	
	CallFunction(pkReturnObj, "FirstRun");

	//pop pointers
	ObjectManagerLua::Pop();	
		
	return pkReturnObj;
}



// Gets
void EntityManager::GetAllEntitys(vector<Entity*> *pakEntitys)
{
	m_pkWorldEntity->GetAllEntitys(pakEntitys,true);	
}

void EntityManager::GetAllEntitysInArea(vector<Entity*> *pkEntitys,Vector3 kPos,float fRadius)
{
	ZoneData* pkZone = GetZone(kPos);
	
	if(!pkZone)
		return;
		
	if(!pkZone->m_pkZone)
		return;
		
	set<int> kZones;	
	
	kZones.insert(pkZone->m_iZoneID);
	
	for(unsigned int i =0;i<pkZone->m_iZoneLinks.size();i++)
	{
		GetZones(&kZones,pkZone->m_iZoneLinks[i],kPos,fRadius);
	}

	
	for(set<int>::iterator it=kZones.begin();it!=kZones.end();it++) {
		GetZoneData(*it)->m_pkZone->GetAllEntitys(pkEntitys,true);
	}		


}

void EntityManager::GetZones(set<int>* pkZones,int iZone,Vector3 kPos,float fRadius)
{
	set<int>::iterator it = pkZones->find(iZone);
	
	if(it != pkZones->end())
		return;
		
	ZoneData* pkTemp = GetZoneData(iZone);
	if(pkTemp)
	{
		float d = kPos.DistanceTo(pkTemp->m_kPos) - pkTemp->m_kSize.Length()/2;
		if(d > fRadius)
			return;
	
		if(pkTemp->m_pkZone)
		{	
			pkZones->insert(iZone);
			
			for(unsigned int i =0;i<pkTemp->m_iZoneLinks.size();i++)
			{
				GetZones(pkZones,pkTemp->m_iZoneLinks[i],kPos,fRadius);
			}			
		}
	}
}

Entity* EntityManager::GetEntityByName(const char* acName)
{
	vector<Entity*> kObjects;		
	GetAllEntitys(&kObjects);
	
	for(vector<Entity*>::iterator it=kObjects.begin();it!=kObjects.end();it++) {
		if((*it)->GetName() == acName)
		{
			return (*it);
		}
	}	
	
	return NULL;
}

Entity*	EntityManager::GetEntityByID(int iNetID)
{
	if(iNetID == -1)
		return NULL;

	map<int,Entity*>::iterator it = m_akEntitys.find(iNetID);
	
	if(it != m_akEntitys.end())
		return (*it).second;
	else
		return NULL;
}

// NetWork
void EntityManager::UpdateState(NetPacket* pkNetPacket)
{
	Entity* pkNetSlave;
	int iObjectID;
	
	//get the first entity id
	pkNetPacket->Read(iObjectID);

	//loop until end of list, or something bad hapens
	while(iObjectID != -1) 
	{
		//check if entity exist
		pkNetSlave = GetEntityByID(iObjectID);
		
		//if not try to create it
		if(!pkNetSlave) 
		{
			pkNetSlave = CreateEntityByNetWorkID(iObjectID);
			//cout << "Creating Entity: " << iObjectID << endl;
		}
				
		//if entity now exist, pack it up
		if(pkNetSlave) 
		{ 
			pkNetSlave->PackFrom(pkNetPacket, ZF_NET_NOCLIENT);			
						
			if(pkNetPacket->IsReadError()) 
			{
				printf("pkNetPacket Read Error\n"); 
				return;
			}

			//get next entity id
			pkNetPacket->Read(iObjectID);
		}
		else
		{
			//if the entity was not created , we cant continue
			cout << "Faild to create Entity: " << iObjectID << endl;
			return;
		}
	}	
}

void EntityManager::SendDeleteEntity(int iClient,int iEntityID)
{
	m_OutNP.Clear();
	m_OutNP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	m_OutNP.TargetSetClient(iClient);
		
	m_OutNP.Write((char) ZFGP_DELETEOBJECT);
	m_OutNP.Write(iEntityID);
	
	//m_OutNP.Write(ZFGP_ENDOFPACKET);
	m_pkNetWork->Send2(&m_OutNP);


	//if this entity is going to be deleted on the client only, we want ot resets its flags 
	if(Entity* pkEnt = GetEntityByID(iEntityID))
	{
		pkEnt->ResetAllNetUpdateFlagsAndChilds(iClient);
	
		pkEnt->SetExistOnClient(iClient,false);
	}
}

void EntityManager::PackEntityToClient(int iClient, vector<Entity*> kObjects,bool bZoneObject)
{
	int iPacketSize = 	0;
	int iEndOfObject = 	-1;
	int iSentSize = 		0;
	int iMaxPacketSize = 800;
	unsigned int iObj = 	0;	
	
	Entity* pkPackObj;
	int iMaxSendSize = m_pkNetWork->GetMaxSendSize();

	//if max allowed sendsize is less then the package size, shrink the package
	if(bZoneObject)
		if(iMaxSendSize < iMaxPacketSize)
			 iMaxPacketSize = iMaxSendSize;

	//set packate type
	m_OutNP.Write((char) ZFGP_OBJECTSTATE);

	
	//if this is the zone object, start packing att last packed object
	if(bZoneObject)
	{
		if(m_pkNetWork->m_RemoteNodes[iClient].m_iCurrentObject >=  kObjects.size())
			m_pkNetWork->m_RemoteNodes[iClient].m_iCurrentObject = 0;
		
		 iObj =  m_pkNetWork->m_RemoteNodes[iClient].m_iCurrentObject;	
	}

	NetPacket kEntityNp;
	
	for(; iObj < kObjects.size(); iObj++)	
	{
		pkPackObj = kObjects[iObj];

		if(!pkPackObj->HaveSomethingToSend(iClient))  
		{
			//cout << "No need to send object" << endl;
			continue;
		}
		
		kEntityNp.Clear();

		kEntityNp.Write(pkPackObj->m_iEntityID);
		
		pkPackObj->PackTo(&kEntityNp,iClient);
		iPacketSize++; 

		// If this entity will overflow the current UDP message, send it and start a new one.

		if( (m_OutNP.m_iPos + kEntityNp.m_iPos) >= iMaxPacketSize) 
		{
			iSentSize += m_OutNP.m_iPos;			//increse total amount of data sent
			
			m_OutNP.Write(iEndOfObject);
			m_OutNP.Write(ZFGP_ENDOFPACKET);
			m_OutNP.TargetSetClient(iClient);
			m_pkNetWork->Send2(&m_OutNP);

			m_OutNP.Clear();
			m_OutNP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;	// ZF_NETTYPE_UNREL ZF_NETTYPE_REL
			m_OutNP.Write((char) ZFGP_OBJECTSTATE);
			m_OutNP.TargetSetClient(iClient);
			iPacketSize = 0;
		}
	
		m_OutNP.WriteNp( &kEntityNp );

		if(bZoneObject)
		{
			if(iSentSize > iMaxSendSize)
			{
				break;
			}
		}
	}
	
	
	//write final package
	m_OutNP.Write(iEndOfObject);
	
	//cout<<"sent size:"<<iSentSize<<endl;	
	//cout<<"Sent "<<obs-nso <<" object of "<<obs<<endl;
	
	//if zone object save this objectid, so that we can continue at this object next frame
	if(bZoneObject)
	{	
		m_pkNetWork->m_RemoteNodes[iClient].m_iCurrentObject = iObj;		
	}
}

void EntityManager::PackZoneListToClient(int iClient, set<int>& iZones)
{
	//if(iZones.size() != 0)
	//	cout<<"Sending "<<iZones.size()<< " to client "<<iClient<<endl;
		
	m_OutNP.Write((char) ZFGP_ZONELIST);


	for(set<int>::iterator itActiveZone = iZones.begin(); itActiveZone != iZones.end(); itActiveZone++ ) 
	{
		m_OutNP.Write(m_kZones[ (*itActiveZone) ].m_iZoneObjectID);
	}
	
	m_OutNP.Write(-1);
}

bool IsInsideVector(int iVal, vector<int>& iArray)
{
	for(unsigned int i=0; i<iArray.size(); i++) {
		if(iArray[i] == iVal)
			return true;
		}
	

	return false;
}

void EntityManager::UpdateZoneList(NetPacket* pkNetPacket)
{
	unsigned int i;

	vector<int>	kZones;
	int iZoneID;

	pkNetPacket->Read(iZoneID);
	while(iZoneID != -1) 
	{
		kZones.push_back(iZoneID);
		pkNetPacket->Read(iZoneID);
	}

	//dvoids was here ;)
	for( i=0; i<m_pkZoneEntity->m_akChilds.size(); i++) 
	{
		int iLocalZoneID = m_pkZoneEntity->m_akChilds[i]->m_iEntityID;
		
		if(IsInsideVector(iLocalZoneID, kZones)) 
		{
			Delete(m_pkZoneEntity->m_akChilds[i]);
			//cout << "Removing Zone: " << iLocalZoneID << endl;
		}
	}



/*
//	LOGSIZE("EntityManager::ZoneList", kZones.size() * 4 + 4);


	for( i=0; i<m_pkZoneObject->m_akChilds.size(); i++) {
		int iLocalZoneID = m_pkZoneObject->m_akChilds[i]->m_iEntityID;
		
		if(IsInsideVector(iLocalZoneID, kZones) == false) {
			Delete(m_pkZoneObject->m_akChilds[i]);
			//cout << "Removing Zone: " << iLocalZoneID << endl;
			}
		}

	Entity* pkStaticEntity=NULL;		

	for( i=0; i<m_pkZoneObject->m_akChilds.size(); i++) {
			// First find the staticentity	
			Entity* pkZone = m_pkZoneObject->m_akChilds[i];
			pkStaticEntity = pkZone->GetStaticEntity();

			//if no staticentity was found, complain
			if(!pkStaticEntity) {
				GetStaticData(pkZone->m_iEntityID);
//				printf("Need Static Ents for Zone - %d\n", pkZone->m_iEntityID);
				return;
				}
		}*/
}


void EntityManager::PackToClients()
{

	int iMaxSendSize = m_pkNetWork->GetNetSpeed() / m_pkZeroFps->GetSystemFps();
	m_pkNetWork->SetMaxSendSize(iMaxSendSize);

	// If no clients we don't send anything.
	if(m_pkNetWork->GetNumOfClients() == 0)
		return;

	//Logf("net", " *** EntityManager::PackToClients() *** \n");
	vector<Entity*>	kObjects;


	unsigned int iClient;
	// Clear Active Zones for clients.
	for(iClient=0; iClient < m_pkZeroFps->m_kClient.size(); iClient++)
	{
		m_pkZeroFps->m_kClient[iClient].m_iActiveZones.clear();
		m_pkZeroFps->m_kClient[iClient].m_iUnloadZones.clear();		
	}
	
	// Refresh list of active Zones for each tracker with a client.
	for(list<P_Track*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) 
	{		
		if((*iT)->m_iConnectID != -1)
		{
			//setup active zones list
			m_pkZeroFps->m_kClient[(*iT)->m_iConnectID].m_iActiveZones = (*iT)->m_iActiveZones;			
					
			//setup unload zones list
			m_pkZeroFps->m_kClient[(*iT)->m_iConnectID].m_iUnloadZones = (*iT)->m_iUnloadZones;
					
			//when object has been sent , reset list in tracker	
			(*iT)->m_iUnloadZones.clear();					
					
		}
	}		


	// Client Network send.
	if(m_pkZeroFps->m_bClientMode && !m_pkZeroFps->m_bServerMode) 
	{
		m_pkWorldEntity->GetAllEntitys(&kObjects, true);

		m_OutNP.Clear();
		m_OutNP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
		m_OutNP.TargetSetClient(0);
		
		PackEntityToClient(0, kObjects,false);
		m_OutNP.Write(ZFGP_ENDOFPACKET);
		m_pkNetWork->Send2(&m_OutNP);
		return;
	}

	// Server Network send.
	for(iClient=0; iClient < m_pkZeroFps->m_kClient.size(); iClient++) 
	{
		//if(m_pkZeroFps->m_kClient[iClient].m_pkObject == NULL)	continue;
		if(!m_pkNetWork->IsConnected(iClient))	
			continue;
					
			
		m_OutNP.Clear();
		m_OutNP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;	// ZF_NETTYPE_UNREL ZF_NETTYPE_REL
		m_OutNP.TargetSetClient(iClient);

		//pack and send clients delete list
		SendDeleteQueue(iClient);
		
		// Pack And Send unload list to client
		DeleteUnloadedZones(iClient);
		//PackZoneListToClient(iClient, m_pkZeroFps->m_kClient[iClient].m_iUnloadZones);		
//		PackZoneListToClient(iClient, m_pkZeroFps->m_kClient[iClient].m_iActiveZones);

		// Loop and clear send data flag for those zone to this client
		for(set<int>::iterator itActiveZone = m_pkZeroFps->m_kClient[iClient].m_iUnloadZones.begin(); itActiveZone != m_pkZeroFps->m_kClient[iClient].m_iUnloadZones.end(); itActiveZone++ ) 
		{
			int iZoneID = (*itActiveZone);
			
			if(m_kZones[iZoneID].m_pkZone)
				m_kZones[iZoneID].m_pkZone->ResetAllNetUpdateFlagsAndChilds( iClient );
		}

		//send all tracked object first =)
		kObjects.clear();	
		for(list<P_Track*>::iterator it = m_kTrackedObjects.begin(); it != m_kTrackedObjects.end(); it++ ) 
		{
			if((*it)->m_iConnectID == (int) iClient)
				(*it)->GetEntity()->GetAllEntitys(&kObjects, true);
		}		
		PackEntityToClient(iClient, kObjects,false);
		
		//collect objects
		kObjects.clear();		
		
		// Pack and Send all Client Objects
		m_pkClientEntity->GetAllEntitys(&kObjects, true);

		//pack and send global objects
		m_pkGlobalEntity->GetAllEntitys(&kObjects, true);

		// Loop all zones activated by client.
		Entity* pkZoneE;
		ZoneData* pkZoneD;
		for(set<int>::iterator itActiveZone = m_pkZeroFps->m_kClient[iClient].m_iActiveZones.begin(); itActiveZone != m_pkZeroFps->m_kClient[iClient].m_iActiveZones.end(); itActiveZone++ ) 
		{
			pkZoneD = GetZoneData((*itActiveZone));
			if(pkZoneD)
			{
				if(pkZoneD->m_iStatus != EZS_LOADED)
					continue;
				
				// Get Zone and all subobjects.
				//int iZoneID = (*itActiveZone);
				pkZoneE = pkZoneD->m_pkZone;
				assert(pkZoneE);
	
				
				pkZoneE->GetAllEntitys(&kObjects,true,true);
			}
		}
		
		//send all data
		PackEntityToClient(iClient, kObjects,true);			//send in true to packtoclient 
		
		m_OutNP.Write(ZFGP_ENDOFPACKET);
		m_pkNetWork->Send2(&m_OutNP);
	}
		

	for(map<int,Entity*>::iterator it = m_akEntitys.begin(); it != m_akEntitys.end(); it++) 
	{
		//(*it)->m_aiNetDeleteList.clear();
		(*it).second->UpdateDeleteList();
	}

/*	if(m_aiNetDeleteList.size() == 0)
		return;

	// Pack delete data.
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	NP.Write((char) ZFGP_DELETEOBJECT);

	// cout << "Delete List Size:"  << m_aiNetDeleteList.size() << endl;

	for(unsigned int i=0; i<m_aiNetDeleteList.size(); i++) {
		NP.Write((int) m_aiNetDeleteList[i] );

		if(NP.m_iPos >= 512) {
			NP.Write(iEndOfObject);
			NP.Write(ZFGP_ENDOFPACKET);
			m_pkNetWork->SendToAllClients(&NP);

			NP.Clear();
			NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
			NP.Write((char) ZFGP_DELETEOBJECT);

			iPacketSize = 0;
			}
	
		}
	
	NP.Write(iEndOfObject);
	NP.Write(ZFGP_ENDOFPACKET);
	m_pkNetWork->SendToAllClients(&NP);

	m_aiNetDeleteList.clear();*/
}


void EntityManager::StaticData(int iClient, NetPacket* pkNetPacket)
{
	int iEntityID;
	pkNetPacket->Read(&iEntityID,sizeof(iEntityID));
	
	Entity* pkEnt = GetEntityByID( iEntityID );
	if(!pkEnt)
		return;

	Entity* pkStatic = NULL;//pkEnt->GetStaticEntity();
	if(!pkStatic)
		return;

	vector<Entity*>	kObjects;

	kObjects.clear();
	pkStatic->GetAllEntitys(&kObjects);
	PackEntityToClient(iClient, kObjects,false);
	
}

/* Send Request for static data to server. */
void EntityManager::GetStaticData(int iEntityID)
{
	NetPacket NP;

	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	NP.Write((char) ZFGP_GETSTATICDATA);
	NP.Write(&iEntityID,sizeof(iEntityID));
	NP.Write(ZFGP_ENDOFPACKET);
	NP.TargetSetClient(0);
	m_pkNetWork->Send2(&NP);
//	m_pkNetWork->SendToClient(0, &NP);
}

// Debug / Help Functions		
void EntityManager::DisplayTree()
{
	GetSystem().Log_Create("fisklins");
	m_pkWorldEntity->PrintTree(0);
}


void EntityManager::DumpActiverPropertysToLog(char* szMsg)
{
	Logf("net", "%s : %d\n", szMsg, m_akPropertys.size() );

	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		Logf("net", "%s (%d)", (*it)->m_acName, (*it)->GetEntity()->m_iEntityID );
		if((*it)->GetEntity()->m_pkParent)
			Logf("net", " Parent Obj: %s\n", (*it)->GetEntity()->m_pkParent->m_strName.c_str() );
	}
	
}

// Message System.
void EntityManager::SendMsg(string strName, int iFrom, int iTo)
{
	GameMessage Msg;
	Msg.m_FromObject	= iFrom;
	Msg.m_ToObject		= iTo;
	Msg.m_Name			= strName;
	RouteMessage(Msg);
}


void EntityManager::RouteMessage(GameMessage& Msg)
{
	Entity*	pkObject = GetEntityByID(Msg.m_ToObject);

	if(pkObject == NULL) {
		cout << "No Valid object for message" << endl;		
		return;
		}

//			m_pkConsole->Printf("Sending Msg '%s' to %d from %d", gm.m_Name.c_str(), gm.m_ToObject, gm.m_FromObject);
	pkObject->PublishEvent(Msg);
}


// Get Strings.
char* EntityManager::GetUpdateStatusName(int eStatus)
{
	char* pkName = "";

	switch(eStatus) {
		case UPDATE_NONE:				pkName = "UPDATE_NONE";			break;
		case UPDATE_ALL: 				pkName = "UPDATE_ALL";				break;
		case UPDATE_NOCHILDS: 		pkName = "UPDATE_NOCHILDS";		break;		
//		case UPDATE_STATIC: 			 pkName = "UPDATE_STATIC";			break;
//		case UPDATE_DYNAMIC: 		 pkName = "UPDATE_DYNAMIC";		break;
//		case UPDATE_PLAYERS:			 pkName = "UPDATE_PLAYERS";		break;
//		case UPDATE_DECORATION:		 pkName = "UPDATE_DECORATION";	break;
//		case UPDATE_LIGHT: 			pkName = "UPDATE_LIGHT";			break;
		}

	return pkName;
}



char* EntityManager::GetPropertyTypeName(int iType)
{
	char* pkName = "";

	switch(iType) {
		case PROPERTY_TYPE_ALL: 		pkName = "PROPERTY_TYPE_ALL";	break;
		case PROPERTY_TYPE_NORMAL: 		pkName = "PROPERTY_TYPE_NORMAL";	break;
		case PROPERTY_TYPE_RENDER: 		pkName = "PROPERTY_TYPE_RENDER";	break;
		case PROPERTY_TYPE_PHYSIC: 		pkName = "PROPERTY_TYPE_PHYSIC";	break;
		}

	return pkName;

}

char* EntityManager::GetPropertySideName(int iSide)
{
	char* pkName = "";

	switch(iSide) {
		case PROPERTY_SIDE_ALL: 		pkName = "PROPERTY_SIDE_ALL";	break;
		case PROPERTY_SIDE_CLIENT: 		pkName = "PROPERTY_SIDE_CLIENT";	break;
		case PROPERTY_SIDE_SERVER: 		pkName = "PROPERTY_SIDE_SERVER";	break;
		}

	return pkName;


}

bool EntityManager::TestLine(vector<Entity*>* pkPPList,Vector3 kPos,Vector3 kVec)
{
	pkPPList->clear();

	for(map<int,Entity*>::iterator it=m_akEntitys.begin();it!=m_akEntitys.end();it++) 
	{	
		Vector3 c=(*it).second->GetWorldPosV() - kPos;		
		kVec.Normalize();		
		Vector3 k=kVec.Proj(c);		
		float cdis=c.Length();
		float kdis=k.Length();
		float Distance = (float) sqrt((cdis*cdis)-(kdis*kdis));
		
		
		float fRadius=(*it).second->GetRadius();
		
		//make sure theres a radius to test against  0.2 seems like a nice vallue =)
		if(fRadius < 0.2)
			fRadius = 0.2;
		
		if(Distance < fRadius)
		{			
			pkPPList->push_back((*it).second);
		}		
	}
	
	if(pkPPList->size()==0)
		return false;
	
	return true;
}


void EntityManager::RunCommand(int cmdid, const CmdArgument* kCommand) 
{ 
	string strName;
	int iTo;
			unsigned int i;
			vector<int>	kZones;

	switch(cmdid) {
		case FID_LOGOHTREE:
			DisplayTree();
			break;

		case FID_LOGACTIVEPROPERTYS:
			//DumpActiverPropertysToLog("Active propertys");

			for( i=0; i<m_pkZoneEntity->m_akChilds.size(); i++) 
			{
				int iLocalZoneID = m_pkZoneEntity->m_akChilds[i]->m_iEntityID;
				m_pkZoneEntity->m_akChilds[i]->SetUpdateStatus(UPDATE_ALL);
			}
			break;

		case FID_SENDMESSAGE:
			strName = kCommand->m_kSplitCommand[1].c_str();
			iTo = atoi(kCommand->m_kSplitCommand[2].c_str());
			SendMsg(strName, -1, iTo);
/*			gm.m_FromObject = -1;
			gm.m_ToObject	= atoi(kCommand->m_kSplitCommand[2].c_str());
			gm.m_Name		= kCommand->m_kSplitCommand[1].c_str();*/
//			m_pkConsole->Printf("Sending Msg '%s' to %d from %d", gm.m_Name.c_str(), gm.m_ToObject, gm.m_FromObject);
			//RouteMessage(gm);
			break;

/*		case FID_LOADZONES:
			LoadZones();
			break;

		case FID_SAVEZONE:
			SaveZones();
			break;
*/			
		case FID_NEWWORLD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				GetSystem().Printf("newworld [mapname]");
				break;
			}
		
			SetWorldDir(kCommand->m_kSplitCommand[1].c_str());
			Clear();
			break;
	
		case FID_LOADWORLD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				GetSystem().Printf("loadworld [load directory]");
			}
			else		
			{
				if(!LoadWorld(kCommand->m_kSplitCommand[1]))
				{
					GetSystem().Printf("Error loading , savegame does not exist?");
				}				
			}			
			break;

		case FID_SAVEWORLD:
			if(kCommand->m_kSplitCommand.size() <= 1)
			{
				GetSystem().Printf("saveworld [save directory]");
			}
			else		
			{
				if(!SaveWorld(kCommand->m_kSplitCommand[1]))
				{
					GetSystem().Printf("Error saving , directory already exist?");
				}				
			}
		
			//ForceSave();
			break;
		
		case FID_SETWORLDDIR:
			SetWorldDir(kCommand->m_kSplitCommand[1].c_str());		
			SaveZones();
			break;
					
	}	
}

void EntityManager::OwnerShip_Take(Entity* pkObj)
{
	if(!pkObj)
		return;
	pkObj->m_eRole			= NETROLE_AUTHORITY;
	pkObj->m_eRemoteRole	= NETROLE_PROXY;
}

void EntityManager::OwnerShip_Give(Entity* pkObj)
{
	if(!pkObj)
		return;
	pkObj->m_eRole			= NETROLE_PROXY;
	pkObj->m_eRemoteRole	= NETROLE_AUTHORITY;
}

void EntityManager::OwnerShip_Request(Entity* pkObj)
{
	if(pkObj == NULL)
		return;

	if(pkObj->m_eRole	== NETROLE_AUTHORITY)
		return;

	NetPacket NP;
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	NP.Write((char) ZFGP_REQOWNOBJECT);
	NP.Write(ZFGP_ENDOFPACKET);

	NetWork* net = static_cast<NetWork*>(GetSystem().GetObjectPtr("NetWork"));
	NP.TargetSetClient(ZF_NET_ALLCLIENT);
	m_pkNetWork->Send2(&NP);
//	net->SendToAllClients(&NP);
	Logf("net", " Sending Own Request for %d\n", pkObj->m_iEntityID);
	
}

void EntityManager::OwnerShip_OnRequest(Entity* pkObj)
{
	if(pkObj == NULL)
		return;

	if(pkObj->m_eRole	== NETROLE_PROXY)
		return;

	NetPacket NP;
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	NP.Write((char) ZFGP_GIVEOWNOBJECT);
	NP.Write(ZFGP_ENDOFPACKET);

//	NetWork* net = static_cast<NetWork*>(GetSystem().GetObjectPtr("NetWork"));
	NP.TargetSetClient(ZF_NET_ALLCLIENT);
	m_pkNetWork->Send2(&NP);
//	net->SendToAllClients(&NP);

	OwnerShip_Give(pkObj);
	Logf("net", " Gives away ownership of %d\n", pkObj->m_iEntityID);

}

void EntityManager::OwnerShip_OnGrant(Entity* pkObj)
{
	if(pkObj == NULL)
		return;

	OwnerShip_Take(pkObj);
	Logf("net", " This node now own %d\n", pkObj->m_iEntityID);
}

Entity* EntityManager::CloneEntity(int iNetID)
{
	Entity* pkObjOrginal = GetEntityByID(iNetID);
	if(pkObjOrginal == NULL)
		return NULL;

	Entity* pkObjClone =	CreateEntity();

	ZFVFile kFile;
	kFile.Open("tclone.dat",0,true);
	pkObjOrginal->Save(&kFile);
	kFile.Close();

	kFile.Open("tclone.dat",0,false);
	pkObjClone->Load(&kFile,false);
	pkObjClone->SetParent(pkObjOrginal->GetParent());
	kFile.Close();

	return pkObjClone;
}


/*
	Draws the Zone's to the screen as colored boxes.

		Inactive:	Red.
		Active:		Green.
		EditMode:	Blue.
eeeeeeereere*/
void EntityManager::Test_DrawZones()
{
	if(!m_bDrawZones)
		return;

	static ZMaterial* pkLine = NULL;
	if(!pkLine)
	{
		pkLine = new ZMaterial;
		pkLine->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkLine->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
		pkLine->GetPass(0)->m_iCullFace = CULL_FACE_BACK;		
		pkLine->GetPass(0)->m_bLighting = false;
		pkLine->GetPass(0)->m_bColorMaterial = true;
		pkLine->GetPass(0)->m_kVertexColor = m_pkRender->GetEditColor("inactive/zonebuild");
	}		
		
	static ZMaterial* pkMatZoneOn = NULL;
	if(!pkMatZoneOn)
	{
		pkMatZoneOn = new ZMaterial;
		pkMatZoneOn->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkMatZoneOn->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
		pkMatZoneOn->GetPass(0)->m_iCullFace = CULL_FACE_NONE;		
		pkMatZoneOn->GetPass(0)->m_bLighting = false;
		pkMatZoneOn->GetPass(0)->m_bFog = false;
		pkMatZoneOn->GetPass(0)->m_bColorMaterial = true;
		pkMatZoneOn->GetPass(0)->m_kVertexColor = m_pkRender->GetEditColor("inactive/zoneon");
	}			

	static ZMaterial* pkMatZoneOff = NULL;
	if(!pkMatZoneOff)
	{
		pkMatZoneOff = new ZMaterial;
		pkMatZoneOff->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkMatZoneOff->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
		pkMatZoneOff->GetPass(0)->m_iCullFace = CULL_FACE_NONE;			
		pkMatZoneOff->GetPass(0)->m_bLighting = false;
		pkMatZoneOff->GetPass(0)->m_bFog = false;
		pkMatZoneOff->GetPass(0)->m_bColorMaterial = true;
		pkMatZoneOff->GetPass(0)->m_kVertexColor = m_pkRender->GetEditColor("inactive/zoneoff");
	}				
	
	static ZMaterial* pkMatZoneCache = NULL;	
	if(!pkMatZoneCache)
	{
		pkMatZoneCache = new ZMaterial;
		pkMatZoneCache->GetPass(0)->m_iPolygonModeFront = LINE_POLYGON;
		pkMatZoneCache->GetPass(0)->m_iPolygonModeBack = LINE_POLYGON;
		pkMatZoneCache->GetPass(0)->m_iCullFace = CULL_FACE_NONE;			
		pkMatZoneCache->GetPass(0)->m_bFog = false;
		pkMatZoneCache->GetPass(0)->m_bLighting = false;
		pkMatZoneCache->GetPass(0)->m_bColorMaterial = true;
		pkMatZoneCache->GetPass(0)->m_kVertexColor = m_pkRender->GetEditColor("inactive/zoneunloading");
	}					
	
	//draw zones
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;

		Vector3 kMin = m_kZones[i].m_kPos - m_kZones[i].m_kSize/2;
		Vector3 kMax = m_kZones[i].m_kPos + m_kZones[i].m_kSize/2;
	
		switch(m_kZones[i].m_iStatus)
		{
			case EZS_LOADED:
				m_pkZShaderSystem->BindMaterial(pkMatZoneOn);
				m_pkRender->DrawAABB( kMin,kMax);
				break;
		
			case EZS_UNLOADED:
				m_pkZShaderSystem->BindMaterial(pkMatZoneOff);
				m_pkRender->DrawAABB( kMin,kMax);
				break;

			case EZS_CACHED:
				m_pkZShaderSystem->BindMaterial(pkMatZoneCache);
				m_pkRender->DrawAABB( kMin,kMax);
				break;								
		}
	}
	
	if(m_bDrawZoneConnections) 
	{
		m_pkZShaderSystem->BindMaterial(pkLine);		
		m_pkZShaderSystem->ClearGeometry();
	
		for(unsigned int i=0;i<m_kZones.size();i++) 
		{
			if(m_kZones[i].m_iStatus == EZS_UNUSED)
				continue;
	
			for(unsigned int j = 0 ;j< m_kZones[i].m_iZoneLinks.size();j++)
			{	
				m_pkZShaderSystem->AddLineV(m_kZones[i].m_kPos,m_kZones[m_kZones[i].m_iZoneLinks[j]].m_kPos);
			}
		}
		
		m_pkZShaderSystem->DrawGeometry(LINES_MODE);				
	}
}



Vector3 EntityManager::GetZoneCenter(int iZoneNum)
{
	return m_kZones[iZoneNum].m_kPos;
}

int EntityManager::GetNumOfZones()
{
	return m_kZones.size();
}

list<P_Track*>* EntityManager::GetTrackerList()
{
	return &m_kTrackedObjects;
}

void EntityManager::AddTracker(P_Track* kObject)
{
	//cout << "Now tracking " << kObject->GetEntity()->m_iEntityID << endl;
	m_kTrackedObjects.push_back(kObject);
}

void EntityManager::RemoveTracker(P_Track* kObject)
{
	m_kTrackedObjects.remove(kObject);
}

int EntityManager::GetNrOfTrackedObjects()
{
	return m_kTrackedObjects.size();
}

void EntityManager::ClearTrackers()
{
	m_kTrackedObjects.clear();
}

ZoneData* EntityManager::GetZone(Vector3 kPos)
{
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
		if(m_kZones[iZ].m_iStatus == EZS_UNUSED)
			continue;

		if(m_kZones[iZ].IsInside(kPos))
			return &m_kZones[iZ];
		}

	return NULL;
}


ZoneData* EntityManager::GetZone(Entity* PkObject)
{
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) 
	{
		if(m_kZones[iZ].m_iStatus == EZS_UNUSED)
			continue;
		
		if(m_kZones[iZ].IsInside(PkObject->GetWorldPosV()))
			return &m_kZones[iZ];
	}

	return NULL;
}

int EntityManager::GetZoneIndex(int iEntityId)
{
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) 
	{
		if(m_kZones[iZ].m_iStatus == EZS_UNUSED)
			continue;
		
		if(m_kZones[iZ].m_iZoneObjectID == iEntityId)	
			return iZ;
	
	}

	return -1;
}



int EntityManager::GetZoneIndex(Entity* PkObject,int iCurrentZone,bool bClosestZone)
{
	return GetZoneIndex(PkObject->GetWorldPosV(),iCurrentZone,bClosestZone);
}

int EntityManager::GetZoneIndex(Vector3 kMyPos,int iCurrentZone,bool bClosestZone)
{
	//if theres a last visited zone
	if(iCurrentZone >= 0 )
	{
		ZoneData* pkZd = GetZoneData(iCurrentZone);
		
		if(pkZd) 
		{	
			if(pkZd->m_iStatus != EZS_UNUSED)
			{
				//first check current zone
				if(m_kZones[iCurrentZone].IsInside( kMyPos ))
					return iCurrentZone;
	
	
				//check zones connected to the last visited zone
				ZoneData* pkZone = GetZoneData(iCurrentZone);
				for(unsigned int i = 0;i < pkZone->m_iZoneLinks.size();i++)
				{
					if(m_kZones[pkZone->m_iZoneLinks[i]].IsInside(kMyPos))
					{	
						//cout<<"moved to nearby zone"<<endl;
						return pkZone->m_iZoneLinks[i];						
					}
				}
			}
		}
	}
	
	//seccond go trough all zones in the world
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) 
	{
		if(m_kZones[iZ].m_iStatus == EZS_UNUSED)
			continue;
		
		if(m_kZones[iZ].IsInside(kMyPos))
		{
			return iZ;
		}
	}

	//last take the closest zone in the world
	if(bClosestZone)
	{
		float d = 999999999;
		int id = -1;
	
		for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
			if(m_kZones[iZ].m_iStatus == EZS_UNUSED)
				continue;
			
			float dis = float(m_kZones[iZ].m_kPos.DistanceTo(kMyPos));
		
			if(dis < d)
			{
				d = dis;
				id = iZ;			
			}
		}
		return id;
	}

	return -1;
}




vector<int>	EntityManager::GetActiveZoneIDs(int iTracker)
{
	vector<int>	Nisse;
	return Nisse;
}


int EntityManager::CreateZone(Vector3 kPos,Vector3 kSize)
{
	int id = GetUnusedZoneID();
	
	if(ZoneData* pkZone = GetZoneData(id))
	{
		//warnig this realy clears all zone data, make sure that the folowing code sets it up correctly again
		pkZone->Clear();
	
		pkZone->m_bNew = true;					
		pkZone->m_iStatus = EZS_UNLOADED;
		pkZone->m_kSize = kSize;
		pkZone->m_kPos = kPos; 
		pkZone->m_iZoneID = id;
	}
	else
	{
		cout<<"ERROR: EntityManager::CreateZone , could not create zone"<<endl;
		return -1; 	
	}
	
	
	UpdateZoneLinks(id);
	
	return id;

}


void EntityManager::DeleteZone(int iId)
{
/*	if(iId == -1)	return;

	if(iId >= (int) m_kZones.size())
	{
		//cout<<"that zone is invalid"<<endl;
		return;
	}
*/	
	
	cout<<"deleted zone "<<iId<<endl;
	
	//remove zone entity
	if(ZoneData* kZData = GetZoneData(iId))
	{
		if(kZData->m_pkZone)
		{
			Delete(kZData->m_pkZone);
			kZData->m_pkZone = NULL;	
		}
	}
	else
		return;
	
	//clear links
	ClearZoneLinks(iId);		
		
	//set as unused	
	m_kZones[iId].m_iStatus = EZS_UNUSED;
	
	return;	
}



bool EntityManager::LoadZones(string strSaveDir )
{
	ZFVFile kFile;
	
	if(!strSaveDir.empty())
	{
		kFile.Open((strSaveDir + "/zones.dat").c_str(),0,false);
	}
	else
	{
		if(!kFile.Open((m_kWorldDirectory + "/zones.dat").c_str(),0,false))
		{
			cout<<"Error loading zones"<<endl;						
			return false;
		}
	}		

	Clear();

	kFile.Read(&m_fSimTime,sizeof(float),1);


	int iNumOfZone;
	kFile.Read(&iNumOfZone,sizeof(int),1);

   // load latest created entityID
   kFile.Read(&m_iNextEntityID,sizeof(int),1);

	m_iNextEntityID+=50;  //evil hack

	cout<<"Nr of zones  : "<<iNumOfZone<<endl;
	cout<<"Next objectID: "<<m_iNextEntityID<<endl;
		
	ZoneData kZData;
	
	int i,zl;
	int iLink;

	for( i=0; i<iNumOfZone; i++) 
	{
		kFile.Read(&kZData.m_iStatus, 	sizeof(kZData.m_iStatus), 1);
		kFile.Read(&kZData.m_bNew, 		sizeof(kZData.m_bNew), 1);
		kFile.Read(&kZData.m_iRevision,	sizeof(kZData.m_iRevision), 1);								
		kFile.Read(&kZData.m_iZoneID, 	sizeof(kZData.m_iZoneID), 1);
		kFile.Read(&kZData.m_kSize, 		sizeof(kZData.m_kSize), 1);
		kFile.Read(&kZData.m_kPos, 		sizeof(kZData.m_kPos), 1);

		//make sure a zone is iether unused or unloaded
		if(kZData.m_iStatus != EZS_UNUSED)
			kZData.m_iStatus = EZS_UNLOADED;
		
		char temp[128];
		kFile.Read(temp, 128, 1);
		kZData.m_strEnviroment = temp;

		int iNumOfLinks;	
		kFile.Read(&iNumOfLinks, sizeof(iNumOfLinks), 1);

		for(zl=0; zl < iNumOfLinks; zl++) 
		{
			kFile.Read(&iLink, sizeof(iLink), 1);
			kZData.m_iZoneLinks.push_back(iLink);
		}
		
		kZData.m_pkZone = NULL;
		m_kZones.push_back(kZData);
		kZData.m_iZoneLinks.clear();
	}

	kFile.Close();

	
	
	return true;
}



bool EntityManager::SaveZones(string strSaveDir)
{
	string filename;

	if(strSaveDir.empty())
		filename = m_kWorldDirectory;	
	else
		filename = strSaveDir;
	
	filename+="/zones.dat";
	
	
	cout<<"saving to :"<<filename<<endl;
	
	ZFVFile kFile;
	if(!kFile.Open(filename.c_str(),0,true))
	{	
		cout<<"Could not open zone save file"<<endl;
		return false;
	}
	
	kFile.Write(&m_fSimTime,sizeof(float),1);
	
 	int iNumOfZone = m_kZones.size();
	kFile.Write(&iNumOfZone,sizeof(int),1);
   
   // save latest created entityID
   cout<<"Next id is:"<<m_iNextEntityID<<endl;
   kFile.Write(&m_iNextEntityID,sizeof(int),1);
	
	for(int i=0; i<iNumOfZone; i++) 
	{
		kFile.Write(&m_kZones[i].m_iStatus, 	sizeof(m_kZones[i].m_iStatus), 1);
		kFile.Write(&m_kZones[i].m_bNew, 		sizeof(m_kZones[i].m_bNew), 1);
		kFile.Write(&m_kZones[i].m_iRevision,	sizeof(m_kZones[i].m_iRevision), 1);										
		kFile.Write(&m_kZones[i].m_iZoneID, 	sizeof(m_kZones[i].m_iZoneID), 1);
		kFile.Write(&m_kZones[i].m_kSize, 		sizeof(m_kZones[i].m_kSize), 1);		
		kFile.Write(&m_kZones[i].m_kPos, 		sizeof(m_kZones[i].m_kPos), 1);

		
		char temp[128];
		strcpy(temp,m_kZones[i].m_strEnviroment.c_str());
		kFile.Write(temp, 128, 1);

		int iNumOfLinks = m_kZones[i].m_iZoneLinks.size();		
		kFile.Write(&iNumOfLinks, sizeof(iNumOfLinks), 1);		
		
		for(int zl=0; zl < iNumOfLinks; zl++)
			kFile.Write(&m_kZones[i].m_iZoneLinks[zl], sizeof(m_kZones[i].m_iZoneLinks[zl]), 1);
	}



	kFile.Close();

	cout<<"zones saved"<<endl;
	return true;
}

bool EntityManager::SaveTrackers(string strSaveDir)
{
	string filename;

	if(strSaveDir.empty())
		filename = m_kWorldDirectory;	
	else
		filename = strSaveDir;
	
	filename+="/trackers.dat";
	
	
	cout<<"Saving trackeres to :"<<filename<<endl;
	
	ZFVFile kFile;
	if(!kFile.Open(filename.c_str(),0,true))
	{	
		cout<<"Could not open tracker save file"<<endl;
		return false;
	}


	int iNrOfTrackers = m_kTrackedObjects.size();
	kFile.Write(&iNrOfTrackers,sizeof(iNrOfTrackers),1);

	for(list<P_Track*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) 
	{		
		int iZone = GetZoneIndex((*iT)->GetEntity(),(*iT)->GetEntity()->m_iCurrentZone,(*iT)->m_bClosestZone);				
		kFile.Write(&iZone, sizeof(iZone), 1);		
		//cout<<"Saving tracker for zone:"<<iZone<<endl;			
	}
	
	kFile.Close();
	
	cout<<"trackers saved"<<endl;
	
	return true;
}

bool EntityManager::LoadTrackers(string strSaveDir)
{
	string filename;

	if(strSaveDir.empty())
		filename = m_kWorldDirectory;	
	else
		filename = strSaveDir;
	
	filename+="/trackers.dat";
	
	
	cout<<"Loading trackers from:"<<filename<<endl;
	
	ZFVFile kFile;
	if(!kFile.Open(filename.c_str(),0,false))
	{	
		cout<<"Could not open tracker save file"<<endl;
		return false;
	}


	int iNrOfTrackers;
	kFile.Read(&iNrOfTrackers,sizeof(iNrOfTrackers),1);
	cout<<"number of trackers: "<<iNrOfTrackers<<endl;
	
	for(int i = 0;i<iNrOfTrackers;i++) 
	{		
		int iZone;
		kFile.Read(&iZone,sizeof(iZone),1);
		
		//load the zone in wich theres suppose to be a tracker
		LoadZone(iZone);
		
		//if(m_kZones[iZone].m_pkZone)
		//	m_kZones[iZone].m_pkZone->SetUpdateStatus(UPDATE_ALL);
		//cout<<"loaded tracked zone "<<iZone<<endl;
		
	}
	
	kFile.Close();
	
	cout<<"trackers loaded"<<endl;
	
	return true;
}

ZoneData* EntityManager::GetZoneData(int iID)
{
	if(iID < 0 || iID >= (int) m_kZones.size())
		return NULL;

	return &m_kZones[iID];

}

void EntityManager::LoadZone(int iId,string strSaveDir)
{	
	ZoneData* kZData = GetZoneData(iId);

	if(!kZData)
		return;

	// Check if zone is loaded. If soo return.
	if(kZData->m_pkZone)
		return;

		
				
	if(kZData->m_iStatus == EZS_UNUSED)
	{
		cout<<"WARNING: tried to load an unused zone"<<endl;
		return;
	}
		
	if(kZData->m_iStatus != EZS_UNLOADED)
	{
		cout<<"zone is not unloaded"<<endl;
		return;
	}
	
	//set status as loaded
	kZData->m_iStatus = EZS_LOADED;

	// Create Object.
	Entity* kZoneEntity = CreateEntity(false);
	kZoneEntity->m_bZone = true;
	kZData->m_pkZone = kZoneEntity;
	kZData->m_pkZone->SetParent(GetZoneEntity());	
	kZData->m_pkZone->SetName("ZoneObject");
	
	
	//load
	
	ZFVFile kFile;
	
	//if this is not set to be a new zone , i shuld try to load it
	if(!kZData->m_bNew)
	{
		char nr[10];
		IntToChar(nr,iId);
	
		kZData->m_bNew = true;
		
		
		//load from input directory if any
		if(!strSaveDir.empty())
		{
			string zonefilename(strSaveDir);
			zonefilename+="/";
			zonefilename+=nr;
			zonefilename+=".dynamic.zone";			
		
			if(kFile.Open(zonefilename.c_str(),0,false))
			{
				//cout<<"Loading zone:"<<zonefilename<<endl;				
				kZData->m_bNew = false;
			}
		}
		else			//if the zone still is not loaded try to load it from the default world directory
		{
			string zonefilename(m_kWorldDirectory);
			zonefilename+="/";
			zonefilename+=nr;
			zonefilename+=".dynamic.zone";
			
			if(kZData->m_bNew == true)
				if(kFile.Open(zonefilename.c_str(),0,false))
				{
					//cout<<"Loading zone:"<<zonefilename<<endl;								
					kZData->m_bNew = false;		
				}
		}
	}

	
	//this zone could not be loaded, or we want to create a new zone, either the case we create a new zone =)
	if(kZData->m_bNew)
	{	
	
		kZData->m_bNew = false;
		
		//link zone object
		Link(kZoneEntity);
		
		//cout<<"error loading zone, creating a new template zone"<<endl;		
		Vector3 kPos = kZData->m_kPos;
		kZoneEntity->SetLocalPosV(kPos);

		kZoneEntity->AddProperty("P_LightUpdate");	//always attach a lightupdateproperty to new zones

		SetZoneModel("",iId);		

		//set objectid in zonedata (sent to client when unloading
		kZData->m_iZoneObjectID = kZData->m_pkZone->m_iEntityID;

	}
	else	//else load zone from file
	{

		//load zone from file
		kZData->m_pkZone->Load(&kFile);
	
		//set objectid in zonedata (sent to client when unloading
		kZData->m_iZoneObjectID = kZData->m_pkZone->m_iEntityID;
	
		kFile.Close();
	}
}

void EntityManager::UnLoadZone(int iId)
{
	ZoneData* kZData = GetZoneData(iId);
	if(kZData->m_pkZone == NULL)
		return;	

			
	SaveZone(iId);
	
	Delete(kZData->m_pkZone);
	kZData->m_pkZone = NULL;

	kZData->m_iStatus = EZS_UNLOADED;		
}

void EntityManager::SaveZone(int iId,string strSaveDir )
{
	ZoneData* kZData = GetZoneData(iId);
	assert(kZData);
	if(kZData->m_pkZone == NULL)
		return;

	char nr[10];
	IntToChar(nr,iId);
	
	//setup filename
	string filename;
	
	if(strSaveDir.empty())
		filename = m_kWorldDirectory;
	else
		filename = strSaveDir;
	
	filename+="/";
	filename+=nr;
	filename+=".dynamic.zone";

	
	//cout<<"Saving zone :"<<filename<<endl;
	
	ZFVFile kFile;
	if(!kFile.Open(filename.c_str(),0,true))
	{	
		cout<<"error saving"<<endl;
		return;
	}

	kZData->m_pkZone->Save(&kFile);
	
	kFile.Close();

}


int EntityManager::GetUnusedZoneID()
{
	for(unsigned int i=0;i<m_kZones.size();i++)
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
		{	
			return i;
		}
	}

	//if none can be found create a new one
	
	ZoneData newzone;
	newzone.m_iZoneID = m_kZones.size();	
	m_kZones.push_back(newzone);
	
	
	cout<<"creating "<<newzone.m_iZoneID<<" "<<m_kZones[newzone.m_iZoneID].m_iZoneID<<endl;
	
	return newzone.m_iZoneID;
}



void EntityManager::ClearZoneLinks(int iId)
{
	//loop trough all connected zones
	for(unsigned int i = 0;i < m_kZones[iId].m_iZoneLinks.size();i++)
	{
		ZoneData* zone = GetZoneData( m_kZones[iId].m_iZoneLinks[i] );
		
		if(!zone)
			continue;
		
		//loop trough the connected zones connected zones
		for(vector<int>::iterator it=zone->m_iZoneLinks.begin();it!=zone->m_iZoneLinks.end();it++)
		{
			if((*it) == iId)
			{
				zone->m_iZoneLinks.erase(it);
				break;
			}
		}
	}
	
	//clear all links
	m_kZones[iId].m_iZoneLinks.clear();
}

bool EntityManager::IsInsideZone(Vector3 kPos,Vector3 kSize)
{
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;

		if(BoxVSBox(kPos,kSize-0.1,m_kZones[i].m_kPos,m_kZones[i].m_kSize))
			return true;
	}
	return false;
	
}

bool EntityManager::ZoneHaveNeighbour(Vector3 kPos,Vector3 kSize)
{
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;

		if(!BoxVSBox(kPos,kSize-0.1,m_kZones[i].m_kPos,m_kZones[i].m_kSize))
			if(BoxVSBox(kPos,kSize,m_kZones[i].m_kPos,m_kZones[i].m_kSize))
				return true;
	}
	return false;
}


void EntityManager::UpdateZoneLinks(int iId)
{
	ZoneData* pkZone = GetZoneData(iId);
	if(!pkZone)
		return;

	//clear all connections
	ClearZoneLinks(iId);
	
	
	Vector3 kPos = m_kZones[iId].m_kPos;
	Vector3 kSize = m_kZones[iId].m_kSize;
	
	//go trough all zones and check if they are to be connected
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;
	
		if((int)i == iId)
			continue;
			
		if(BoxVSBox(kPos,kSize ,m_kZones[i].m_kPos, m_kZones[i].m_kSize))
		{
			m_kZones[iId].m_iZoneLinks.push_back(i);
			m_kZones[i].m_iZoneLinks.push_back(iId);
		}
	}
}


bool EntityManager::BoxVSBox(Vector3 kPos1,Vector3 kSize1,Vector3 kPos2,Vector3 kSize2)
{
	
	//box 1
	float x1 = float(kSize1.x/2.0);
	float y1 = float(kSize1.y/2.0);
	float z1 = float(kSize1.z/2.0);
	
	vector<Vector3>	kTestDirs1;
	kTestDirs1.push_back(Vector3(-x1,y1,z1));
	kTestDirs1.push_back(Vector3(x1,y1,z1));
	kTestDirs1.push_back(Vector3(x1,-y1,z1));
	kTestDirs1.push_back(Vector3(-x1,-y1,z1));
	
	kTestDirs1.push_back(Vector3(-x1,y1,-z1));
	kTestDirs1.push_back(Vector3(x1,y1,-z1));
	kTestDirs1.push_back(Vector3(x1,-y1,-z1));
	kTestDirs1.push_back(Vector3(-x1,-y1,-z1));
	
	//box 2
	float x2 = float(kSize2.x/2.0);
	float y2 = float(kSize2.y/2.0);
	float z2 = float(kSize2.z/2.0);
	
	vector<Vector3>	kTestDirs2;
	kTestDirs2.push_back(Vector3(-x2,y2,z2));
	kTestDirs2.push_back(Vector3(x2,y2,z2));
	kTestDirs2.push_back(Vector3(x2,-y2,z2));
	kTestDirs2.push_back(Vector3(-x2,-y2,z2));
	
	kTestDirs2.push_back(Vector3(-x2,y2,-z2));
	kTestDirs2.push_back(Vector3(x2,y2,-z2));
	kTestDirs2.push_back(Vector3(x2,-y2,-z2));
	kTestDirs2.push_back(Vector3(-x2,-y2,-z2));

	unsigned int i;
	// box1 vs box2
	for( i = 0 ;i<kTestDirs1.size();i++)
	{
		Vector3 kPoint = kPos1 + kTestDirs1[i];
		
		if(kPoint.x < (kPos2.x - x2))	continue;		
		if(kPoint.y < (kPos2.y - y2))	continue;
		if(kPoint.z < (kPos2.z - z2))	continue;
		if(kPoint.x > (kPos2.x + x2))	continue;
		if(kPoint.y > (kPos2.y + y2))	continue;
		if(kPoint.z > (kPos2.z + z2))	continue;
		return true;
	}

	// box2 vs box1
	for( i = 0 ;i<kTestDirs2.size();i++)
	{
		Vector3 kPoint = kPos2 + kTestDirs2[i];
		
		if(kPoint.x < (kPos1.x - x1))	continue;		
		if(kPoint.y < (kPos1.y - y1))	continue;
		if(kPoint.z < (kPos1.z - z1))	continue;
		if(kPoint.x > (kPos1.x + x1))	continue;
		if(kPoint.y > (kPos1.y + y1))	continue;
		if(kPoint.z > (kPos1.z + z1))	continue;
		return true;
	}

	return false;
}


void EntityManager::SetZoneModel(const char* szName,int iId)
{
	ZoneData* zd = GetZoneData(iId);
	if(!zd)
	{
		return;
	}	
	
	if(!zd->m_pkZone)	
	{
		return;
	}	
	
	if(zd->m_iStatus == EZS_UNUSED)
		cout<<"is unused "<<szName<<endl;

	if(zd->m_iStatus != EZS_LOADED)
		cout<<"not loaded "<<szName<<endl;
		
			
	if(strlen(szName) == 0)
	{
		zd->m_pkZone->DeleteProperty("P_Mad");
		zd->m_pkZone->DeleteProperty("P_Tcs");
		zd->m_pkZone->DeleteProperty("P_PfMesh");		
		return;
	}

	//setup mad
	P_Mad* mp = (P_Mad*)zd->m_pkZone->GetProperty("P_Mad");
	
	if(!mp)
		mp = (P_Mad*)zd->m_pkZone->AddProperty("P_Mad");
		
	if(mp)
	{
		mp->SetBase(szName);
		mp->SetShadowGroup(1);
	}

	//setup pathfinding
	P_PfMesh* pkMesh = (P_PfMesh*)zd->m_pkZone->GetProperty("P_PfMesh");
	
	if(!pkMesh)
		pkMesh = (P_PfMesh*)zd->m_pkZone->AddProperty("P_PfMesh");
	
	if(pkMesh)
		pkMesh->SetMad(mp);



	//setup physics
	P_Tcs* pp = (P_Tcs*)zd->m_pkZone->GetProperty("P_Tcs");
	
	if(!pp)
		pp = (P_Tcs*)zd->m_pkZone->AddProperty("P_Tcs");	
	
	if(pp)
	{
		pp->SetPolygonTest(true);	
		pp->SetStatic(true);			
		pp->SetRefetchPolygonData();
		pp->SetGroup(0);
	}

}



void EntityManager::ForceUnload()
{	

	//loop trough all loaded zones, and unload em , to make sure that all zones is saved
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;
	
		if(m_kZones[i].m_pkZone)
			UnLoadZone(i);		
	}
	
	UpdateDelete();
	
	//after unloading , reload 	
	UpdateZoneSystem();

}

void EntityManager::ForceSave()
{
	//loop trough all loaded zones, and unload em , to make sure that all zones is saved
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;
	
		if(m_kZones[i].m_pkZone)
			SaveZone(i);		
	}	
}



void EntityManager::ResetNetUpdateFlags(int iConID)
{
	for(map<int,Entity*>::iterator it=m_akEntitys.begin();it!=m_akEntitys.end();it++) {
		(*it).second->ResetAllNetUpdateFlags(iConID);
	}
	
}


bool EntityManager::SaveWorld(string strSaveDir,bool bForce)
{
	//check if directory exist	
	if(m_pkBasicFS->DirExist(strSaveDir.c_str()))
	{
		//if we dont want to forcesave in that directory we return
		if(!bForce)
		{
			cout<<"ERROR: directory already exist"<<endl;
			return false;
		}
	}
	else
	{
		//directory does not exist , lets try to create it		
		if(!m_pkBasicFS->CreateDir(strSaveDir.c_str()))
		{
			cout<<"ERROR: could not create save directory"<<endl;
			return false;
		}
	}
	
	
	//first try to save zones
	if(!SaveZones(strSaveDir))
	{
		cout<<"ERROR: could not save zonesdata"<<endl;
		return false;
	}
	
	//save trackers
	if(!SaveTrackers(strSaveDir))
	{
		cout<<"ERROR: could not save trackers"<<endl;
		return false;
	}
	
	
	
	//now load zones and then save them in the save directory
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;
	
		//check if zone is loaded
		if(m_kZones[i].m_pkZone)
			SaveZone(i,strSaveDir); //if so save it
		else
		{
			//if zone is not loaded try to load it,
			LoadZone(i);
			
			//was the zone loaded successfully?
			if(m_kZones[i].m_pkZone)
			{
				//if so save it =)
				SaveZone(i,strSaveDir);
			}
		}
	}

	return true;
}

bool EntityManager::LoadWorld(string strLoadDir)
{
	m_strLoadDirectory = strLoadDir;

	//check if directory exist
	if(!m_pkBasicFS->DirExist(strLoadDir.c_str()))
	{
		cout<<"ERROR: save directory does not exist"<<endl;
		return false;
	}

	//check that the worldtempdirectory is clean
	//first make sure it does exist
	if(m_pkBasicFS->DirExist(m_kWorldDirectory.c_str()))
	{
		//try to remove all files in the directory

		//setup a filter, so we dont delete files that shuld not be deleted =)
		vector<string> kFilter;
		kFilter.push_back("dynamic.zone");

		vector<string> kFiles;
		m_pkBasicFS->ListDirFilter(&kFiles,kFilter,m_kWorldDirectory.c_str());

		//cout<<"going to remove these files to clean the temp directory, here goes the harddrive ;-)"<<endl;
		cout<<"Cleaning temp directory"<<endl;
		for(int i=0;i<kFiles.size();i++)
		{
			string file = m_kWorldDirectory+string("/")+kFiles[i];
			//cout<<file<<endl;
			
			if(!m_pkBasicFS->RemoveFile(file.c_str()))
			{
				cout<<"ERROR: could not remove file:"<<file<<endl;
			}			
		}	
	}
	else	
	{
		//if the temp directory didt exist create it
		if(!m_pkBasicFS->CreateDir(m_kWorldDirectory.c_str()))
		{
			cout<<"ERROR: could not create tempdirectory "<<m_kWorldDirectory<<endl;
			return false;
		}
	}


	//clear the world
	Clear();

	//now try to load the zone list
	if(!LoadZones(strLoadDir))
	{
		cout<<"ERROR: error loading zonelist"<<endl;
		return false;
	}


	//now load and save the zones (making a copy of the zones in the loaddirectory to the tempdirecotry)
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;
	
		//check if zone is loaded
		if(m_kZones[i].m_pkZone)
			cout<<"WARNING: this zone is loaded, that shuld not be possible"<<endl;
		else
		{
			//first load the zone from the savegame directory
			LoadZone(i,strLoadDir);
						
			//now unload it again to save it
			UnLoadZone(i);
		}
	}	
	
	//make sure all entitys created in the load process is deleted. this may caus problem if the load funkction is called from whitin a property etc
	UpdateDelete();
	

		
	//finaly load the tracker list
	if(!LoadTrackers(strLoadDir))
	{
		cout<<"ERROR: error loading trackers"<<endl;
		return false;
	}
			

		
	//do a zone update
//	UpdateZones();	
	UpdateZoneSystem();
	
	return true;
}


void EntityManager::UpdateZoneSystem()
{

	UpdateTrackers();
	UpdateZoneStatus();
}

void EntityManager::UpdateTrackers()
{
	Vector3 kTrackerPos;
	int iZoneIndex;	
	vector<ZoneData*>	kFloodZones;
	

	// Set All Zones as untracked
	for(int iZ=0;iZ<m_kZones.size();iZ++) 
	{
		m_kZones[iZ].m_bTracked		= false;
	}



	// For each tracker.
	for(list<P_Track*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) 
	{
		// Find Active Zone.
		set<int>			kNewActiveZones;
		
		//set initi range
		for(int iZ=0;iZ<m_kZones.size();iZ++)
			m_kZones[iZ].m_iRange							= 10000;
		
		//get trackerpos
		kTrackerPos = 	(*iT)->GetEntity()->GetWorldPosV();
			
		//get current zone
		iZoneIndex = GetZoneIndex((*iT)->GetEntity(),(*iT)->GetEntity()->m_iCurrentZone,(*iT)->m_bClosestZone);		
		if(iZoneIndex == -1)
		{
			//cout<<"ERROR: Tracker not in zone"<<endl;
			continue;
		}
		
		//start zone
		ZoneData* pkStartZone = GetZoneData(iZoneIndex);
		pkStartZone->m_iRange = 0;						
		kFloodZones.push_back(pkStartZone);

		
		// Flood Zones in rage to active.
		while(kFloodZones.size()) 
		{
			ZoneData* pkZone = kFloodZones.back();
			kFloodZones.pop_back();

			kNewActiveZones.insert(pkZone->m_iZoneID);

			pkZone->m_bTracked = true;
			int iRange = pkZone->m_iRange + 1;

//			if(iRange < m_iTrackerLOS) 
//			{
				if(kTrackerPos.DistanceTo(pkZone->m_kPos) <= float(m_iTrackerLOS))
				{
			
					for(unsigned int i=0; i<pkZone->m_iZoneLinks.size(); i++) 
					{
						ZoneData* pkOtherZone = GetZoneData(pkZone->m_iZoneLinks[i]); //				pkZone->m_pkZoneLinks[i];	//GetZoneData(pkZone->m_iZoneLinks[i]);				
	
						//if zone has already been checked continue whit the next one
						if(pkOtherZone->m_iRange <= iRange)	continue;		// Dvoid: ändrade till <= från <  , tycks snabba upp algoritmen med en faktor av ca 100000000 (pga att den lägger till samma zon flera gånger)
						
						//set new range 
						pkOtherZone->m_iRange = iRange;
											
						
						//add zone to flooded zones list
						kFloodZones.push_back(pkOtherZone);
					}				
				}
//			}
		}
		
		//find new loaded zones  , compare new actives zones whit last update to find new loaded zones
		(*iT)->m_iNewActiveZones.clear();
		set_difference(kNewActiveZones.begin(),kNewActiveZones.end(),(*iT)->m_iActiveZones.begin(),(*iT)->m_iActiveZones.end(), inserter((*iT)->m_iNewActiveZones, (*iT)->m_iNewActiveZones.begin()));
		
		//findout wich zones has been removed since last update, and add them to list to be sent to client (observer the list shuld not be cleared here, but in the code that sends the package)
		if((*iT)->m_iConnectID != -1)
			set_difference((*iT)->m_iActiveZones.begin(),(*iT)->m_iActiveZones.end(),kNewActiveZones.begin(),kNewActiveZones.end(), inserter((*iT)->m_iUnloadZones, (*iT)->m_iUnloadZones.begin()));		
		
		//save new active zones in tracker
		(*iT)->m_iActiveZones = kNewActiveZones;
	}
}

void EntityManager::UpdateZoneStatus()
{
	float fCurrentTime = m_pkZeroFps->GetEngineTime();


	for(int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus != EZS_UNUSED)
		{
			//the zone is currently tracked
			if(m_kZones[i].m_bTracked)
			{
				//zone is cached , lets activate it
				if(m_kZones[i].m_iStatus == EZS_CACHED)
				{
					//cout<<"activating cached zone "<<i<<endl;															
					m_kZones[i].m_iStatus = EZS_LOADED;					
					
					if(m_kZones[i].m_pkZone)
						m_kZones[i].m_pkZone->SetUpdateStatus(UPDATE_ALL);
					continue;
				}
			
				//zone is unloaded , lests load it
				if(m_kZones[i].m_iStatus == EZS_UNLOADED)
				{
					//cout<<"Loading zone "<<i<<endl;
					
					//m_kZones[i].m_iStatus = EZS_LOADED;					
					// zone status is set in loadzone()
					LoadZone(i);
					
					if(m_kZones[i].m_pkZone)
						m_kZones[i].m_pkZone->SetUpdateStatus(UPDATE_ALL);
						
					continue;
				}				
			}
			//the zone is not tracked
			else
			{
				//zone is loaded, set it as cached
				if(m_kZones[i].m_iStatus == EZS_LOADED)
				{
					//cout<<"zone is no longer tracked, setting as cached and starting timout "<<i<<endl;
					
					m_kZones[i].m_iStatus = EZS_CACHED;
					
					if(m_kZones[i].m_pkZone)
						m_kZones[i].m_pkZone->SetUpdateStatus(UPDATE_NONE);
						
					m_kZones[i].m_fInactiveTime = fCurrentTime;
					continue;
				}
				
				//zone is cached, wait for timeout
				if(m_kZones[i].m_iStatus == EZS_CACHED)
				{
					if( (fCurrentTime - m_kZones[i].m_fInactiveTime) > m_fZoneUnloadTime)
					{
						//cout<<"cached zone timed out, unloading "<<i<<endl;
						
						m_kZones[i].m_iStatus = EZS_UNLOADED;						
						UnLoadZone(i);		
						
						continue;
					}
				}				
			}
		}
	}
}

bool EntityManager::CallFunction(Entity* pkEntity, const char* acFunction,vector<ARG_DATA>* pkParams)
{
	//set self id before calling the funktion
	ObjectManagerLua::g_iCurrentObjectID = pkEntity->m_iEntityID;	
	
	//cout << "Calling Func: " << acFunction << endl;

	if(pkParams)
		return m_pkScript->Call(pkEntity->GetEntityScript(), (char*)acFunction,*pkParams);
	else
		return m_pkScript->Call(pkEntity->GetEntityScript(), (char*)acFunction,0,0);	
}


void EntityManager::ClearClientDeleteQueue(int iClient)
{
	while(!m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.empty())
		m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.pop();
}

void EntityManager::ClearClientsDeleteQueues()
{
	for(int i = 0;i<m_pkZeroFps->GetMaxPlayers();i++)
		ClearClientDeleteQueue(i);
}

void EntityManager::AddEntityToClientDeleteQueue(int iClient,int iEntityID)
{
	if(!m_pkNetWork->IsConnected(iClient))
	{
		cout<<"WARNING: tried to add entity to unconnected client delete list"<<endl;
		return;	
	}
	
	m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.push(iEntityID);
	
	//entity shuld not exist anymore on the client, after this
	if(Entity* pkEnt = GetEntityByID(iEntityID))
	{
		pkEnt->ResetAllNetUpdateFlagsAndChilds(iClient);		
		pkEnt->SetExistOnClient(iClient,false);	
	}
	
	cout<<"added entity:"<<iEntityID<<" to client "<<iClient<< " delete queue"<<endl;
}


void EntityManager::SendDeleteQueue(int iClient)
{
	if(m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.empty())
		return;
		

	cout<<"sending delete list to client:"<<iClient<<" size:"<<m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.size()<<endl;
		
	m_OutNP.Write((char) ZPGP_DELETELIST);
	
	while(!m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.empty())
	{
		m_OutNP.Write(m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.front());
		m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.pop();
	}
	
	m_OutNP.Write(-1);
	
	
}

void EntityManager::HandleDeleteQueue(NetPacket* pkNetPacket)
{
	int iEntityID;
	
	pkNetPacket->Read(iEntityID);
	
	while(iEntityID != -1)
	{
		if(Entity* pkEnt = GetEntityByID(iEntityID))
			if(pkEnt->m_eRole == NETROLE_PROXY)
			{
				cout<<"deleting entity:"<<iEntityID<<endl;
				Delete(iEntityID);		
			}
			else
				cout<<"WARNING: got request to delete a non proxy entity:"<<iEntityID<<endl;

		pkNetPacket->Read(iEntityID);
	}
}

void EntityManager::DeleteUnloadedZones(int iClient)
{	
	for(set<int>::iterator itActiveZone = m_pkZeroFps->m_kClient[iClient].m_iUnloadZones.begin(); itActiveZone != m_pkZeroFps->m_kClient[iClient].m_iUnloadZones.end(); itActiveZone++ ) 
	{
		AddEntityToClientDeleteQueue(iClient,m_kZones[ (*itActiveZone) ].m_iZoneObjectID);		
	}	
}


