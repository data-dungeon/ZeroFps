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
#include "../engine_systems/propertys/p_pfmesh.h"
#include "../basic/zfbasicfs.h"



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
	m_bActive =			false;
	m_iZoneObjectID = -1;
	m_iRange = 			0;		
	m_iVersion =		1;

	m_kTestPos.Set(0,0,0);
	m_kPos.Set(0,0,0);
	m_kSize.Set(0,0,0);
	
		
	m_strEnviroment = "";
}

bool ZoneData::IsInside(const Vector3& kPoint)
{
	static Vector3 half;
	
	half.Set(m_kSize.x / 2.0,
				m_kSize.y / 2.0,
				m_kSize.z / 2.0
				);

	if(kPoint.x < (m_kTestPos.x - half.x))	return false;
	if(kPoint.y < (m_kTestPos.y - half.y))	return false;
	if(kPoint.z < (m_kTestPos.z - half.z))	return false;

	if(kPoint.x > (m_kTestPos.x + half.x))	return false;
	if(kPoint.y > (m_kTestPos.y + half.y))	return false;
	if(kPoint.z > (m_kTestPos.z + half.z))	return false;

	return true;
}

ZSSEntityManager::ZSSEntityManager() 
: ZFSubSystem("ZSSEntityManager") 
{
	// Set Default values
	m_iNextEntityID			= 0;
	m_iTotalNetEntityData	= 0;
	m_iNumOfNetEntitys		= 0;
	//m_bDrawZones				= false;
	//m_bDrawAABBTree			= false;
	//m_bDrawZoneConnections	= false;
	//m_bDrawEnviroments		= false;
	//m_iTrackerLOS				= 100;	
	//m_iObjectDistance			= 50;
	//m_fZoneUnloadTime			= 30;	
	//m_iMaxZoneIO 				= 4;	
	m_bAllowHide				= true;

	m_pkWorldEntity			= NULL;
	m_pkZoneEntity				= NULL;
	m_pkClientEntity			= NULL;
	m_pkGlobalEntity			= NULL;

	m_fSimTime					= 0;			
	//m_fSimTimeScale			= 1.0;	

	m_iSendType					= ZF_NETTYPE_REL;			// ZF_NETTYPE_UNREL	ZF_NETTYPE_REL

	m_pkSceneAABBTree			= new SceneAABBTree(this);

	// Register Variables		
	m_kbDrawEnviroments.Register(this, "l_showenvs",		"0");
	m_kbDrawZones.Register(this, "l_showzones",				"0");
	m_kbDrawZoneConnections.Register(this, "l_showconn",	"0");
	m_kfSimTimeScale.Register(this, "e_simspeed",			"1.0");
	m_kiTrackerLOS.Register(this,		"l_trackerlos",		"100");
	m_kfZoneUnloadTime.Register(this,	"l_zoneunloadtime",	"30");
	m_kiMaxZoneIO.Register(this,		"l_maxzoneio",			"4");
	m_kiObjectDistance.Register(this,	"l_objectdistance",	"50");
	m_kbDrawAABBTree.Register(this,	"l_showaabbtree",		"0");
	
	//RegisterVariable("l_showenvs",			&m_bDrawEnviroments,			CSYS_BOOL);
	//RegisterVariable("l_showzones",			&m_bDrawZones,					CSYS_BOOL);
	//RegisterVariable("l_showconn",			&m_bDrawZoneConnections,	CSYS_BOOL);
	//RegisterVariable("e_simspeed",			&m_fSimTimeScale,				CSYS_FLOAT);
	//RegisterVariable("l_trackerlos",			&m_iTrackerLOS,				CSYS_INT);	
	//RegisterVariable("l_zoneunloadtime",	&m_fZoneUnloadTime,			CSYS_FLOAT);	
	//RegisterVariable("l_maxzoneio",			&m_iMaxZoneIO,					CSYS_INT);	
	//RegisterVariable("l_objectdistance",	&m_iObjectDistance,			CSYS_FLOAT);		
	//RegisterVariable("l_showaabbtree",		&m_bDrawAABBTree,				CSYS_BOOL);		

	// Register Commands
	Register_Cmd("o_logtree",		FID_LOGOHTREE);	
	Register_Cmd("o_dumpp",			FID_LOGACTIVEPROPERTYS);	
	Register_Cmd("sendmsg",			FID_SENDMESSAGE,	CSYS_FLAG_SRC_ALL, "sendmsg name id",2);	
	Register_Cmd("newworld",		FID_NEWWORLD,		CSYS_FLAG_SRC_ALL);	
	Register_Cmd("loadworld",		FID_LOADWORLD,		CSYS_FLAG_SRC_ALL);	
	Register_Cmd("saveworld",		FID_SAVEWORLD,		CSYS_FLAG_SRC_ALL);		
	Register_Cmd("setworlddir",	FID_SETWORLDDIR,	CSYS_FLAG_SRC_ALL);		
}

bool ZSSEntityManager::StartUp()	
{
	// Get Subsystems.
	m_pkZeroFps		=	static_cast<ZSSZeroFps*>(GetSystem().GetObjectPtr("ZSSZeroFps"));		
	m_pkNetWork		=	static_cast<ZSSNetWork*>(GetSystem().GetObjectPtr("ZSSNetWork"));
	m_pkScript		=	static_cast<ZSSScriptSystem*>(GetSystem().GetObjectPtr("ZSSScriptSystem"));
	m_pkBasicFS		=	static_cast<ZSSBasicFS*>(GetSystem().GetObjectPtr("ZSSBasicFS"));		
	m_pkRender		=	static_cast<ZSSRender*>(GetSystem().GetObjectPtr("ZSSRender"));
	m_pkPropertyFactory	= static_cast<ZSSPropertyFactory*>(g_ZFObjSys.GetObjectPtr("ZSSPropertyFactory"));	
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

bool ZSSEntityManager::ShutDown() 
{ 
	Clear();

	return true; 
}

bool ZSSEntityManager::IsValid()	{ return true; }

ZSSEntityManager::~ZSSEntityManager() 
{
	if(m_pkSceneAABBTree)
		delete m_pkSceneAABBTree;

	float fAvgObjSize = -1;

	if(m_iNumOfNetEntitys) {
		fAvgObjSize = float(m_iTotalNetEntityData / m_iNumOfNetEntitys);
		}

	g_ZFObjSys.Logf("net", " Avg Obj Size: %f\n", fAvgObjSize);
	
	//LOGF("net", "apan ï¿½ lï¿½t");
	
	// Obs! Hï¿½ skall resursen laddas ur, gï¿½ dock inte pga timeout expire.
	//delete m_pScriptFileHandle;

}


void ZSSEntityManager::UpdateSimTime()
{
	//calculate new scaled sim time delta
	m_fSimTimeDelta = m_kfSimTimeScale.GetFloat() * m_pkZeroFps->GetSystemUpdateFpsDelta();
	
	//add sim time delta to current sim time
	m_fSimTime += m_fSimTimeDelta;
}


Entity* ZSSEntityManager::GetEntityByType(const char* czType)
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
  also put them in the ZSSEntityManager.
*/
void ZSSEntityManager::Link(Entity* pkEntity,int iId) 
{
	if(pkEntity->m_iEntityID != -1)
	{
		if(IsLinked(pkEntity))
		{
			cerr<<"WARNING: Entity is already linked"<<endl;
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

bool ZSSEntityManager::IsLinked(Entity* pkEntity)
{
	if(m_akEntitys.find(pkEntity->m_iEntityID) == m_akEntitys.end())
		return false;
	else
		return true;
}

/**	\brief	UnLink this from ZSSEntityManager.

  Remvoves Entity from the ZSSEntityManager.
*/
void ZSSEntityManager::UnLink(Entity* pkEntity) 
{	
	// If i own Entity mark so we remove it on clients.
	//	if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
	//		m_aiNetDeleteList.push_back(pkObject->m_iEntityID);
	//m_akObjects.remove(pkObject);
	m_akEntitys.erase(pkEntity->m_iEntityID);
}

/**	\brief	Delete all Entitys.

	Clear all data from ZSSEntityManager.
*/
void ZSSEntityManager::Clear()
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
void ZSSEntityManager::CreateBaseEntitys()
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
Entity* ZSSEntityManager::CreateEntity(bool bLink)
{
	Entity* pkObj = new Entity;
	
	if(bLink)
		Link(pkObj);
	
	return pkObj;
}

/**	\brief	Adds an Entity to delete qeue
*/
void ZSSEntityManager::Delete(int iNetworkID) 
{
   if( Entity* pkE = GetEntityByID(iNetworkID))
      Delete(pkE);
}


void ZSSEntityManager::Delete(Entity* pkEntity) 
{
	if(pkEntity == NULL)
		return;

	if(m_pkZeroFps->m_bServerMode && !m_pkZeroFps->m_bClientMode)
	{
		if(pkEntity->m_iEntityID < 0 || pkEntity->m_iEntityID > m_iNextEntityID)
		{
			ZFAssert(0,"Entity ID out of range in delete");
		}
	}

	for(vector<int>::iterator it=m_aiDeleteList.begin();it!=m_aiDeleteList.end();it++) 
	{
		if(pkEntity->m_iEntityID == (*it)) {
			//Logf("net", "Entity [%d] already in delete list\n", pkEntity->m_iEntityID);
			//cout << "Entity already in delete list" << endl;
			return;
		}
	}
	
	//add to delete list
	m_aiDeleteList.push_back(pkEntity->m_iEntityID);
	
	
	//if where the owner of this entity also add entity to clients delete lists
	if(pkEntity->m_eRole == NETROLE_AUTHORITY)
	{
		//AddEntityToAllClientDeleteQueues(pkEntity->GetEntityID());	
		
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
void ZSSEntityManager::UpdateDelete()
{
	if(!m_kPropertyDeleteList.empty())
	{
		int iSize = m_kPropertyDeleteList.size();
		for(int i = 0;i<iSize;i++)
		{
			//cout<<"deleting property "<<m_kPropertyDeleteList[i]->m_acName<<endl;
			delete m_kPropertyDeleteList[i];
		}
		
		m_kPropertyDeleteList.clear();
	}


	if(!m_aiDeleteList.empty())
	{
		for(int i=0; i<m_aiDeleteList.size(); i++)
		{
			int iEntityID = m_aiDeleteList[i];
			Entity* pkEntity = GetEntityByID(iEntityID);
	
			if(pkEntity) { // If i own object mark so we remove it on clients.
				/*if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
					m_aiNetDeleteList.push_back((*it));*/
				delete pkEntity;		
				}
		}
	
		m_aiDeleteList.clear();
	}
}


/**	\brief	Run update on selected propertys.

	This function collects all propertys that fit the selected flags, sorts them if it needs to
	and then runs the update function of each of this propertys.
*/
void ZSSEntityManager::Update(int iType,int iSide,bool bSort,Entity* pkRootEntity,bool bForceRootOnly,bool bUpdateList)
{
	if(bUpdateList)
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
			{		
				//AABBTree optimized version
 				if(iType == PROPERTY_TYPE_RENDER && iSide == PROPERTY_SIDE_CLIENT)
 					m_pkSceneAABBTree->GetRenderPropertysInFrustum(&m_akPropertys,m_pkZeroFps->GetCam()->GetFrustum());
				else		
					m_pkWorldEntity->GetAllPropertys(&m_akPropertys,iType,iSide);
			}
		}
		
	
		//sort all propertys
		if(bSort)
		{
 			stable_sort(m_akPropertys.begin(),m_akPropertys.end(),Less_Property);
// 			sort(m_akPropertys.begin(),m_akPropertys.end(),Less_Property);
// 			sort(m_akPropertys.begin(),m_akPropertys.end());
		}
	}

	//update normal propertys update counter
	if(iType & PROPERTY_TYPE_NORMAL)
		m_iNormalUpdates += m_akPropertys.size();
	
	//update render propertys update counter	
	if((iType & PROPERTY_TYPE_RENDER))
		m_iRenderUpdates += m_akPropertys.size();

	//run update in all propertys
	int iPropertys = m_akPropertys.size();
	for(int i = 0;i<iPropertys;i++)
		m_akPropertys[i]->Update();

}




bool ZSSEntityManager::IsUpdate(int iFlags)
{
	if(m_iUpdateFlags & iFlags)
		return true;

	return false;
}




void ZSSEntityManager::UpdateGameMessages(void)
{
/*	// Let Objects/Propertys handle messages
	for(list<Entity*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		(*it)->HandleMessages();
	}*/
}

Entity* ZSSEntityManager::CreateEntityByNetWorkID(int iNetID)
{
	Entity *pkNew = CreateEntity(false);

	Link(pkNew,iNetID);
	
	pkNew->SetParent(m_pkWorldEntity);
	pkNew->m_eRole			= NETROLE_PROXY;
	pkNew->m_eRemoteRole	= NETROLE_AUTHORITY;
	pkNew->SetUseZones(false);
	
	//setup interpolation
	//pkNew->SetInterpolate(false);
	pkNew->SetInterpolateFactor(7);
	
	return pkNew;
}

/**	\brief	Uses a script to create the Entity.

	Creates a Entity from a script and use it to set values and propertys. If script file
	is not found no Entity will be created. 
*/
Entity* ZSSEntityManager::CreateEntityFromScriptInZone(const char* acName,Vector3 kPos,int iCurrentZone)
{
	int id = GetZoneIndex(kPos,iCurrentZone,false);
	
	if(id == -1)
	{
		cout<<"WARNING CreateEntityFromScriptInZone: zone not found"<<endl;
		return NULL;
	}
	
	
	//force loading of this zone
	LoadZone(id);
	
	//if(m_kZones[id].m_iStatus == EZS_UNLOADED || m_kZones[id].m_iStatus == EZS_UNUSED)
	if(m_kZones[id].m_iStatus != EZS_LOADED)
	{
		cout<<"WARNING CreateEntityFromScriptInZone: zone not loaded"<<endl;		
		return NULL;
	}
	
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

Entity* ZSSEntityManager::CreateEntityFromScript(const char* acName)
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
	Entity* pkReturnObj = ObjectManagerLua::g_kScriptState.g_pkReturnObject;
	
	//setup entity
	pkReturnObj->m_strType	= &acName[pos];
	pkReturnObj->m_strName	= string("A ") + &acName[pos];
	pkReturnObj->m_pScriptFileHandle->SetRes(acName);
	
	CallFunction(pkReturnObj, "FirstRun");

	//pop pointers
	ObjectManagerLua::Pop();	
		
	return pkReturnObj;
}



// Gets
void ZSSEntityManager::GetAllEntitys(vector<Entity*> *pakEntitys)
{
	m_pkWorldEntity->GetAllEntitys(pakEntitys,true);	
}

void ZSSEntityManager::GetAllEntitysInArea(vector<Entity*> *pkEntitys,Vector3 kPos,float fRadius)
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

void ZSSEntityManager::GetZones(set<int>* pkZones,int iZone,Vector3 kPos,float fRadius)
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

Entity* ZSSEntityManager::GetEntityByName(const char* acName)
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

Entity*	ZSSEntityManager::GetEntityByID(int iNetID)
{
	if(iNetID == -1)
		return NULL;

	map<int,Entity*>::iterator it = m_akEntitys.find(iNetID);
	
	if(it != m_akEntitys.end())
		return (*it).second;
	else
		return NULL;
}

Property* ZSSEntityManager::GetPropertyFromEntityID(int iEntityID,const char* czPropertyName)
{
	if(Entity* pkEnt = GetEntityByID(iEntityID))
		return pkEnt->GetProperty(czPropertyName);
		
	return NULL;
}


// NetWork
void ZSSEntityManager::UpdateState(NetPacket* pkNetPacket)
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



void ZSSEntityManager::PackEntityToClient(int iClient, vector<Entity*>& kObjects,bool bZoneObject,int iSendSize,Entity* pkReferens)
{
	int iPacketSize = 	0;
	int iEndOfObject = 	-1;
	int iSentSize = 		0;
	int iMaxPacketSize = 800;
	unsigned int iObj = 	0;	
	
	//int iMaxSendSize = m_pkNetWork->GetMaxSendSize();
	int iMaxSendSize = iSendSize;
		
	//if max allowed sendsize is less then the package size, shrink the package
	if(bZoneObject)
		if(iMaxSendSize < iMaxPacketSize)
			 iMaxPacketSize = iMaxSendSize;

	//set packate type
	m_OutNP.Write((char) ZFGP_OBJECTSTATE);

	
	//if this is the zone object, start packing att last packed object
	if(bZoneObject)
	{
		if(m_pkNetWork->m_RemoteNodes[iClient]->m_iCurrentObject >=  kObjects.size())
			m_pkNetWork->m_RemoteNodes[iClient]->m_iCurrentObject = 0;
		
		 iObj =  m_pkNetWork->m_RemoteNodes[iClient]->m_iCurrentObject;	
	}

	UpdatePriority(kObjects,pkReferens,iClient);
	

	NetPacket kEntityNp;	
	Entity* pkPackObj = NULL;

// 	for(; iObj < kObjects.size(); iObj++)	
// 	{
	while(1)
	{

// 		pkPackObj = kObjects[iObj];
		//check priority
		pkPackObj = GetTopPriorityEntity(kObjects,iClient);

		
		
		if(!pkPackObj)
		{
			//cout<<"end of queue"<<endl;
			break;
		}
		
		
// 		//entity has something to send?
// 		if(!pkPackObj->HaveSomethingToSend(iClient))  
// 		{
// 			//cout << "No need to send object " <<pkPackObj->GetEntityID()<< endl;
// 			continue;
// 		}		
		
				
		kEntityNp.Clear();
		kEntityNp.Write(pkPackObj->m_iEntityID);
		pkPackObj->PackTo(&kEntityNp,iClient);
		iPacketSize++; 

		
		/*
		//failsafe check
		if(kEntityNp.m_iPos >=  iMaxPacketSize)
		{
			cout<<"ERROR: entity "<<pkPackObj->m_iEntityID<<" is to big to be sent. size:"<<kEntityNp.m_iPos<< " max size is:"<<iMaxPacketSize<<endl;			
			cout<<"Propertys"<<endl;
			
			vector<Property*> kProps;
			
			pkPackObj->GetPropertys(&kProps,PROPERTY_TYPE_ALL,PROPERTY_SIDE_ALL);
			
			for(int i =0;i<kProps.size();i++)
			{
				cout<<i<<string(" : ")<<kProps[i]->m_acName<<endl;
			}			
		}
		*/
		
		// If this entity will overflow the current UDP message, send it and start a new one.
		if( (m_OutNP.m_iPos + kEntityNp.m_iPos) >= iMaxPacketSize) 
		{
			iSentSize += m_OutNP.m_iPos;			//increse total amount of data sent
			
			m_OutNP.Write(iEndOfObject);
			m_OutNP.Write(ZFGP_ENDOFPACKET);
			m_OutNP.TargetSetClient(iClient);
			m_pkNetWork->Send2(&m_OutNP);

			m_OutNP.Clear();
			m_OutNP.m_kData.m_kHeader.m_iPacketType = m_iSendType;	
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
		m_pkNetWork->m_RemoteNodes[iClient]->m_iCurrentObject = iObj;		
	}
}

void ZSSEntityManager::UpdatePriority(vector<Entity*>& kObjects,Entity* pkReferens,int iClientID)
{
	float fTime = m_pkZeroFps->GetEngineTime();
	int iSize = kObjects.size();
	for(int i =0;i<iSize;i++)
	{		
		if(!kObjects[i]->HaveSomethingToSend(iClientID))
		{
			kObjects[i]->m_fPriority =  -1;		
			continue;
		}
		
		if(kObjects[i]->m_kLastSent[iClientID] == -1)
			kObjects[i]->m_kLastSent[iClientID] = fTime;
		
		kObjects[i]->m_fPriority =  0;		
 		kObjects[i]->m_fPriority += kObjects[i]->GetRadius();
  		kObjects[i]->m_fPriority += (fTime - kObjects[i]->m_kLastSent[iClientID]);
 		
 		if(pkReferens)
 		{
 			kObjects[i]->m_fPriority += Max(m_kiTrackerLOS.GetInt() - pkReferens->GetWorldPosV().DistanceTo(kObjects[i]->GetWorldPosV()),0.0) / 2.0;
 		}
	}
}

Entity* ZSSEntityManager::GetTopPriorityEntity(vector<Entity*>& kObjects,int iClientID)
{
	float fMax = -1;
	Entity* pkEnt = NULL;
	
	int iSize = kObjects.size();
	for(int i =0;i<iSize;i++)
	{
		if(kObjects[i]->m_fPriority > fMax)
		{
			fMax = kObjects[i]->m_fPriority;
			pkEnt = kObjects[i];
		}
	}


	if(pkEnt)
	{
		//reset its priority
		pkEnt->m_fPriority = -1;
		
		//update send time
		pkEnt->m_kLastSent[iClientID] = m_pkZeroFps->GetEngineTime();		
	}

	return pkEnt;
}

bool IsInsideVector(int iVal, vector<int>& iArray)
{
	for(unsigned int i=0; i<iArray.size(); i++) {
		if(iArray[i] == iVal)
			return true;
		}
	

	return false;
}



void ZSSEntityManager::PackToClients()
{
	bool bCheckSendStatus = true;
	bool bForceAll = false;

	//calculate max send size
// 	if(m_pkZeroFps->GetSyncNetwork())
// 		m_pkNetWork->SetMaxSendSize(m_pkNetWork->GetNetSpeed() / m_pkZeroFps->GetSystemFps());
// 	else	
// 		m_pkNetWork->SetMaxSendSize(m_pkNetWork->GetNetSpeed() / m_pkZeroFps->GetNetworkFps());
		

	// If no clients we don't send anything.
	if(m_pkNetWork->GetNumOfClients() == 0)
		return;

	//Logf("net", " *** ZSSEntityManager::PackToClients() *** \n");
	vector<Entity*>	kObjects;


	unsigned int iClient;
	// Clear Active Zones for clients.
	for(iClient=0; iClient < m_pkZeroFps->m_kClient.size(); iClient++)
	{
		m_pkZeroFps->m_kClient[iClient].m_kActiveZones.clear();
		m_pkZeroFps->m_kClient[iClient].m_kUnloadZones.clear();		
	}
	
	// Refresh list of active Zones for each tracker with a client.
	for(list<P_Track*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) 
	{		
		if((*iT)->m_iConnectID != -1)
		{
			//setup active zones list
			m_pkZeroFps->m_kClient[(*iT)->m_iConnectID].m_kActiveZones = (*iT)->m_kActiveZones;			
					
			//setup unload zones list
			m_pkZeroFps->m_kClient[(*iT)->m_iConnectID].m_kUnloadZones = (*iT)->m_kUnloadZones;
					
			//when object has been sent , reset list in tracker	
			(*iT)->m_kUnloadZones.clear();					
					
		}
	}		


	// Client Network send.
	if(m_pkZeroFps->m_bClientMode && !m_pkZeroFps->m_bServerMode) 
	{
		
		if(Entity* pkEnt = GetEntityByID(m_pkZeroFps->GetClientObjectID()))
		{
// 			cout<<"pos:"<<pkEnt->GetWorldPosV().x<<endl;
		
			m_OutNP.Clear();
			m_OutNP.m_kData.m_kHeader.m_iPacketType = m_iSendType;
			m_OutNP.TargetSetClient(0);
		
			kObjects.clear();
			kObjects.push_back(pkEnt);	

 			PackEntityToClient(0, kObjects,false,m_pkZeroFps->GetConnectionSpeed(),NULL);
					
			m_OutNP.Write(ZFGP_ENDOFPACKET);
			m_pkNetWork->Send2(&m_OutNP);
			
		}
	
/*		//denna koden är EEEEEEVIL , floddar servern med massa entitys,,, nåt fell nånstans men vet i fan vad :(
		kObjects.clear();	
		m_pkClientEntity->GetAllEntitys(&kObjects, bForceAll,bCheckSendStatus);

		m_OutNP.Clear();
		m_OutNP.m_kData.m_kHeader.m_iPacketType = m_iSendType;
		m_OutNP.TargetSetClient(0);
		
		cout<<"entitys to send:"<<kObjects.size()<<endl;
		
 		PackEntityToClient(0, kObjects,false,m_pkZeroFps->GetConnectionSpeed());
		m_OutNP.Write(ZFGP_ENDOFPACKET);
		m_pkNetWork->Send2(&m_OutNP);
		return;*/
	}

	// Server Network send.
	for(iClient=0; iClient < m_pkZeroFps->m_kClient.size(); iClient++) 
	{
		//if(m_pkZeroFps->m_kClient[iClient].m_pkObject == NULL)	continue;
		if(!m_pkNetWork->IsConnected(iClient))	
			continue;
		if(m_pkZeroFps->m_kClient[iClient].m_bLogin)
			continue;
		
		//connection speed
	 	int iSendSize;		
		if(m_pkZeroFps->GetSyncNetwork())
			iSendSize = int(m_pkNetWork->GetClientNetSpeed(iClient) / m_pkZeroFps->GetSystemFps());
 		else	
			iSendSize = int(m_pkNetWork->GetClientNetSpeed(iClient) / m_pkZeroFps->GetNetworkFps());		
			
		
		
		m_OutNP.Clear();
		m_OutNP.m_kData.m_kHeader.m_iPacketType = m_iSendType;	
		m_OutNP.TargetSetClient(iClient);


		// Loop and clear send data flag for those zone to this client
		for(set<int>::iterator itActiveZone = m_pkZeroFps->m_kClient[iClient].m_kUnloadZones.begin(); itActiveZone != m_pkZeroFps->m_kClient[iClient].m_kUnloadZones.end(); itActiveZone++ ) 
		{
			int iZoneID = (*itActiveZone);
			
			if(m_kZones[iZoneID].m_pkZone)
				m_kZones[iZoneID].m_pkZone->ResetAllNetFlagsAndChilds( iClient );
		}

		//send all tracked object first =)
		Entity* pkReferensEnt = NULL;		
		kObjects.clear();	
		for(list<P_Track*>::iterator it = m_kTrackedObjects.begin(); it != m_kTrackedObjects.end(); it++ ) 
		{
			if((*it)->m_iConnectID == (int) iClient)
			{
				pkReferensEnt = (*it)->GetEntity();
				(*it)->GetEntity()->GetAllEntitys(&kObjects,bForceAll,bCheckSendStatus);
			}
		}				
		PackEntityToClient(iClient, kObjects,false,iSendSize,NULL);		
		
		//clear list and start sending normal entitys
		kObjects.clear();		
		
		// Pack and Send all Client Objects
		m_pkClientEntity->GetAllEntitys(&kObjects,bForceAll,bCheckSendStatus);

		//pack and send global objects
		m_pkGlobalEntity->GetAllEntitys(&kObjects,bForceAll,bCheckSendStatus);

		// Loop all zones activated by client, featching all entitys in them
		Entity* pkZoneE;
		ZoneData* pkZoneD;
		for(set<int>::iterator itActiveZone = m_pkZeroFps->m_kClient[iClient].m_kActiveZones.begin(); itActiveZone != m_pkZeroFps->m_kClient[iClient].m_kActiveZones.end(); itActiveZone++ ) 
		{
			pkZoneD = GetZoneData((*itActiveZone));
			if(pkZoneD)
			{
				if(pkZoneD->m_iStatus != EZS_LOADED)
					continue;
				
				// Get Zone and all subobjects.
				pkZoneE = pkZoneD->m_pkZone;
				assert(pkZoneE);	
				
				pkZoneE->GetAllEntitys(&kObjects,bForceAll,bCheckSendStatus);
			}
		}
		
		//send all entitys in zones data
		PackEntityToClient(iClient, kObjects,true,iSendSize,pkReferensEnt);			//send in true to packtoclient 
		
		//pack and send clients delete list
		SendDeleteQueue(iClient);
		
		// Pack And Send unload list to client
		DeleteUnloadedZones(iClient);
		
		
		m_OutNP.Write(ZFGP_ENDOFPACKET);
		m_pkNetWork->Send2(&m_OutNP);
	}
		

	//DANGER ..this must be here..else we will have an evil propertydeletelist lekage
	for(map<int,Entity*>::iterator it = m_akEntitys.begin(); it != m_akEntitys.end(); it++) 
	{
		(*it).second->UpdateDeletePropertyList();
	}


}



// Debug / Help Functions		
void ZSSEntityManager::DisplayTree()
{
	GetSystem().Log_Create("fisklins");
	m_pkWorldEntity->PrintTree(0);
}


void ZSSEntityManager::DumpActiverPropertysToLog(char* szMsg)
{
	g_ZFObjSys.Logf("net", "%s : %d\n", szMsg, m_akPropertys.size() );

	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		g_ZFObjSys.Logf("net", "%s (%d)", (*it)->GetName(), (*it)->GetEntity()->m_iEntityID );
		if((*it)->GetEntity()->m_pkParent)
			g_ZFObjSys.Logf("net", " Parent Obj: %s\n", (*it)->GetEntity()->m_pkParent->m_strName.c_str() );
	}
	
}

// Message System.
void ZSSEntityManager::SendMsg(string strName, string strParam, int iFrom, int iTo)
{
	GameMessage Msg;
	Msg.m_FromObject	 = iFrom;
	Msg.m_ToObject		 = iTo;
	Msg.m_Name			 = strName;
	Msg.m_strParameter = strParam;
	RouteMessage(Msg);
}


void ZSSEntityManager::RouteMessage(GameMessage& Msg)
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
char* ZSSEntityManager::GetUpdateStatusName(int eStatus)
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



char* ZSSEntityManager::GetPropertyTypeName(int iType)
{
	char* pkName = "";

	switch(iType) {
		case PROPERTY_TYPE_ALL: 		pkName = "PROPERTY_TYPE_ALL";	break;
		case PROPERTY_TYPE_NORMAL: 		pkName = "PROPERTY_TYPE_NORMAL";	break;
		case PROPERTY_TYPE_RENDER: 		pkName = "PROPERTY_TYPE_RENDER";	break;
		}

	return pkName;

}

char* ZSSEntityManager::GetPropertySideName(int iSide)
{
	char* pkName = "";

	switch(iSide) {
		case PROPERTY_SIDE_ALL: 		pkName = "PROPERTY_SIDE_ALL";	break;
		case PROPERTY_SIDE_CLIENT: 		pkName = "PROPERTY_SIDE_CLIENT";	break;
		case PROPERTY_SIDE_SERVER: 		pkName = "PROPERTY_SIDE_SERVER";	break;
		}

	return pkName;


}

bool ZSSEntityManager::TestLine(vector<Entity*>* pkPPList,Vector3 kPos,Vector3 kVec)
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


void ZSSEntityManager::RunCommand(int cmdid, const ConCommandLine* kCommand) 
{ 
	string strName;
	string strParam;

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
			strParam = kCommand->m_kSplitCommand[2].c_str();
			iTo = atoi(kCommand->m_kSplitCommand[3].c_str());
			GetSystem().Printf("Sending Msg '%s' '%s' to %d from %d", 
				strName.c_str(), strParam.c_str(), iTo, -1);
			SendMsg(strName, strParam, -1, iTo);

			
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

void ZSSEntityManager::OwnerShip_Take(Entity* pkObj)
{
	if(!pkObj)
		return;
	pkObj->m_eRole			= NETROLE_AUTHORITY;
	pkObj->m_eRemoteRole	= NETROLE_PROXY;
}

void ZSSEntityManager::OwnerShip_Give(Entity* pkObj)
{
	if(!pkObj)
		return;
	pkObj->m_eRole			= NETROLE_PROXY;
	pkObj->m_eRemoteRole	= NETROLE_AUTHORITY;
}

void ZSSEntityManager::OwnerShip_Request(Entity* pkObj)
{
	if(pkObj == NULL)
		return;

	if(pkObj->m_eRole	== NETROLE_AUTHORITY)
		return;

	NetPacket NP;
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = m_iSendType;
	NP.Write((char) ZFGP_REQOWNOBJECT);
	NP.Write(ZFGP_ENDOFPACKET);

	ZSSNetWork* net = static_cast<ZSSNetWork*>(GetSystem().GetObjectPtr("ZSSNetWork"));
	NP.TargetSetClient(ZF_NET_ALLCLIENT);
	m_pkNetWork->Send2(&NP);
//	net->SendToAllClients(&NP);
	g_ZFObjSys.Logf("net", " Sending Own Request for %d\n", pkObj->m_iEntityID);
	
}

void ZSSEntityManager::OwnerShip_OnRequest(Entity* pkObj)
{
	if(pkObj == NULL)
		return;

	if(pkObj->m_eRole	== NETROLE_PROXY)
		return;

	NetPacket NP;
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = m_iSendType;
	NP.Write((char) ZFGP_GIVEOWNOBJECT);
	NP.Write(ZFGP_ENDOFPACKET);

//	NetWork* net = static_cast<NetWork*>(GetSystem().GetObjectPtr("NetWork"));
	NP.TargetSetClient(ZF_NET_ALLCLIENT);
	m_pkNetWork->Send2(&NP);
//	net->SendToAllClients(&NP);

	OwnerShip_Give(pkObj);
	g_ZFObjSys.Logf("net", " Gives away ownership of %d\n", pkObj->m_iEntityID);

}

void ZSSEntityManager::OwnerShip_OnGrant(Entity* pkObj)
{
	if(pkObj == NULL)
		return;

	OwnerShip_Take(pkObj);
	g_ZFObjSys.Logf("net", " This node now own %d\n", pkObj->m_iEntityID);
}

Entity* ZSSEntityManager::CloneEntity(int iNetID)
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

void ZSSEntityManager::DrawSceneAABBTree()
{
	if(!m_kbDrawAABBTree.GetBool())
		return;

	static Vector3 kLeaf(1,0.5,0);
	static Vector3 kCulled(0,0.5,0.5);
	static Vector3 kBranch(0.5,0.5,1);

	vector<AABBGraphInfo>	kAABBs;


	m_pkSceneAABBTree->GetAABBList(&kAABBs);

	for(int i = 0;i<kAABBs.size();i++)
	{
		if(kAABBs[i].m_bLeaf)
		{
			if(kAABBs[i].m_bInFrustum)
				m_pkRender->DrawAABB(kAABBs[i].m_kMin,kAABBs[i].m_kMax,kLeaf);
			else
				m_pkRender->DrawAABB(kAABBs[i].m_kMin,kAABBs[i].m_kMax,kCulled);
		}
		else
			m_pkRender->DrawAABB(kAABBs[i].m_kMin,kAABBs[i].m_kMax,kBranch);
	}
}

/*
	Draws the Zone's to the screen as colored boxes.

		Inactive:	Red.
		Active:		Green.
		EditMode:	Blue.
eeeeeeereere*/
void ZSSEntityManager::DrawZones()
{
	DrawZones(&m_kZones);
}


void ZSSEntityManager::DrawZones(const vector<ZoneData>* pkZoneList)
{
	if(!m_kbDrawZones.GetBool())
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
	
	static ZMaterial* pkMatText = NULL;	
	if(!pkMatText)
	{
		pkMatText = new ZMaterial;
		pkMatText->GetPass(0)->m_kTUs[0]->SetRes("text/defguifont.tga");
		pkMatText->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
		pkMatText->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
		pkMatText->GetPass(0)->m_bLighting = 				false;		
		pkMatText->GetPass(0)->m_bColorMaterial = 		true;
		pkMatText->GetPass(0)->m_kVertexColor =			Vector3(1,0,1);
		pkMatText->GetPass(0)->m_bFog = 						false;		
		pkMatText->GetPass(0)->m_bAlphaTest =				true;		
		pkMatText->GetPass(0)->m_bDepthTest = 				true;	
	}
		
	static ZGuiFont* m_pkFont=NULL;
	if(!m_pkFont)
	{
		m_pkFont = new ZGuiFont("defguifont");
		m_pkFont->Create("/data/textures/text/defguifont.fnt",-1);	
	}
		
	//draw zones
	for(unsigned int i=0;i<pkZoneList->size();i++) 
	{
		if((*pkZoneList)[i].m_iStatus == EZS_UNUSED)
			continue;

			
		Vector3 kMin = (*pkZoneList)[i].m_kPos;
		Vector3 kMax = (*pkZoneList)[i].m_kPos + (*pkZoneList)[i].m_kSize / 2;
		kMin.x -= int( (*pkZoneList)[i].m_kSize.x / 2 );
		kMin.y -= int( (*pkZoneList)[i].m_kSize.y / 2 );
		kMin.z -= int( (*pkZoneList)[i].m_kSize.z / 2 );

		kMax.x += (*pkZoneList)[i].m_kSize.x/2 - int((*pkZoneList)[i].m_kSize.x)/2;
		kMax.y += (*pkZoneList)[i].m_kSize.y/2 - int((*pkZoneList)[i].m_kSize.y)/2;
		kMax.z += (*pkZoneList)[i].m_kSize.z/2 - int((*pkZoneList)[i].m_kSize.z)/2;

		switch((*pkZoneList)[i].m_iStatus)
		{
			case EZS_LOADED:
				m_pkZShaderSystem->BindMaterial(pkMatZoneOn);
				m_pkRender->DrawAABB(kMin,kMax);
				
				//print enviroment
				if(m_kbDrawEnviroments.GetBool())
					if(!((*pkZoneList)[i].m_strEnviroment.empty()))
						m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),(*pkZoneList)[i].m_kPos,1,(*pkZoneList)[i].m_strEnviroment,pkMatText,m_pkFont,true);
				
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
	
	if(m_kbDrawZoneConnections.GetBool()) 
	{
		m_pkZShaderSystem->BindMaterial(pkLine);		
		m_pkZShaderSystem->ClearGeometry();
	
		for(unsigned int i=0;i<pkZoneList->size();i++) 
		{
			if((*pkZoneList)[i].m_iStatus == EZS_UNUSED)
				continue;
	
			for(unsigned int j = 0 ;j< (*pkZoneList)[i].m_iZoneLinks.size();j++)
			{	
				m_pkZShaderSystem->AddLineV((*pkZoneList)[i].m_kPos,(*pkZoneList)[m_kZones[i].m_iZoneLinks[j]].m_kPos);
			}
		}
		
		m_pkZShaderSystem->DrawGeometry(LINES_MODE);				
	}	
}

void ZSSEntityManager::DrawSceneGraph()
{
	static Vector3 kAABBColor(1,1,1);
	static Vector3 kConColor(1,0,0);
	
	if(!m_pkZoneEntity)
		return;
	
	
	vector<Entity*>	kEntitys;
	m_pkZoneEntity->GetAllEntitys(&kEntitys,true,false);
	
	int iSize = kEntitys.size();
	for(int i = 0;i<iSize;i++)
	{
// 		m_pkRender->DrawAABB(kEntitys[i]->GetWorldPosV()+kEntitys[i]->m_kAABBMin,kEntitys[i]->GetWorldPosV()+kEntitys[i]->m_kAABBMax,kAABBColor);	
		
		if(kEntitys[i]->GetParent())
		{
			m_pkRender->Line(kEntitys[i]->GetWorldPosV(),kEntitys[i]->GetParent()->GetWorldPosV(),kConColor);
		}
		
	}
}

Vector3 ZSSEntityManager::GetZoneCenter(int iZoneNum)
{
	return m_kZones[iZoneNum].m_kPos;
}

int ZSSEntityManager::GetNumOfZones()
{
	return m_kZones.size();
}

list<P_Track*>* ZSSEntityManager::GetTrackerList()
{
	return &m_kTrackedObjects;
}

void ZSSEntityManager::AddTracker(P_Track* kObject)
{
	//cout << "Now tracking " << kObject->GetEntity()->m_iEntityID << endl;
	m_kTrackedObjects.push_back(kObject);
}

void ZSSEntityManager::RemoveTracker(P_Track* kObject)
{
	m_kTrackedObjects.remove(kObject);
}

int ZSSEntityManager::GetNrOfTrackedObjects()
{
	return m_kTrackedObjects.size();
}

void ZSSEntityManager::ClearTrackers()
{
	m_kTrackedObjects.clear();
}

ZoneData* ZSSEntityManager::GetZone(const Vector3& kPos)
{
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
		if(m_kZones[iZ].m_iStatus == EZS_UNUSED)
			continue;

		if(m_kZones[iZ].IsInside(kPos))
			return &m_kZones[iZ];
		}

	return NULL;
}


ZoneData* ZSSEntityManager::GetZone(Entity* PkObject)
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

int ZSSEntityManager::GetZoneIndex(int iEntityId)
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


int ZSSEntityManager::GetZoneIndex(const Vector3& kMyPos,int iCurrentZone,bool bClosestZone)
{
	//if theres a last visited zone
	if(ZoneData* pkZd = GetZoneData(iCurrentZone)) 
	{	
		if(pkZd->m_iStatus != EZS_UNUSED)
		{
			//first check current zone
			if(pkZd->IsInside( kMyPos ))
				return iCurrentZone;	

			//check zones connected to the last visited zone
			for(unsigned int i = 0;i < pkZd->m_iZoneLinks.size();i++)
			{
				if(m_kZones[pkZd->m_iZoneLinks[i]].IsInside(kMyPos))
				{	
					//cout<<"moved to nearby zone"<<endl;
					return pkZd->m_iZoneLinks[i];						
				}
			}
		}
	}

	
	//seccond go trough all zones in the world
	//cout<<"entity was not in current zone ("<<iCurrentZone<<") nor a current zones neighbor, doing slow search"<<endl;		
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) 
	{
		if(m_kZones[iZ].m_iStatus == EZS_UNUSED)
			continue;
		
		if(m_kZones[iZ].IsInside(kMyPos))
			return iZ;
	}

	//last take the closest zone in the world
	if(bClosestZone)
	{
		float d = 999999999;
		int id = -1;
	
		for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
			if(m_kZones[iZ].m_iStatus == EZS_UNUSED)
				continue;
			
			float dis = float(m_kZones[iZ].m_kTestPos.DistanceTo(kMyPos));
		
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




vector<int>	ZSSEntityManager::GetActiveZoneIDs(int iTracker)
{
	vector<int>	Nisse;
	return Nisse;
}


int ZSSEntityManager::CreateZone(const Vector3& kPos,const Vector3& kSize)
{
	//check if for collissions
	if(IsInsideZone(kPos, kSize))
		return -1;			

	int id = GetUnusedZoneID();
	
	if(ZoneData* pkZone = GetZoneData(id))
	{
		//warnig this really clears all zone data, make sure that the following code sets it up correctly again
		pkZone->Clear();
	
		pkZone->m_bNew = true;					
		pkZone->m_iStatus = EZS_UNLOADED;
		pkZone->m_kSize = kSize;
		pkZone->m_kPos = kPos; 
		pkZone->m_iZoneID = id;
		pkZone->m_iVersion = 1;
		pkZone->m_kTestPos.x = kPos.x + ( (kSize.x / 2) - int(kSize.x / 2) );
		pkZone->m_kTestPos.y = kPos.y + ( (kSize.y / 2) - int(kSize.y / 2) );
		pkZone->m_kTestPos.z = kPos.z + ( (kSize.z / 2) - int(kSize.z / 2) );
	}
	else
	{
		cout<<"ERROR: ZSSEntityManager::CreateZone , could not create zone"<<endl;
		return -1; 	
	}
	
	
	UpdateZoneLinks(id);
	
	return id;

}


void ZSSEntityManager::DeleteZone(int iId)
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



bool ZSSEntityManager::LoadZones(string strSaveDir )
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

	//cout<<"Nr of zones  : " << iNumOfZone<<endl;
	//cout<<"Next objectID: " << m_iNextEntityID<<endl;
		
	ZoneData kZData;
	
	int i,zl;
	int iLink;
	int iVersion;

	for( i=0; i<iNumOfZone; i++) 
	{
		kFile.Read(&kZData.m_iStatus, 	sizeof(kZData.m_iStatus), 1);
		kFile.Read(&kZData.m_bNew, 		sizeof(kZData.m_bNew), 1);
		kFile.Read(&iVersion,			sizeof(iVersion), 1);
		kFile.Read(&kZData.m_iZoneID, 	sizeof(kZData.m_iZoneID), 1);
		kFile.Read(&kZData.m_kSize, 	sizeof(kZData.m_kSize), 1);
		kFile.Read(&kZData.m_kPos, 		sizeof(kZData.m_kPos), 1);
		
		
		if ( iVersion > 0 )
			kFile.Read(&kZData.m_kTestPos, 		sizeof(kZData.m_kTestPos), 1);
		else
			kZData.m_kTestPos = kZData.m_kPos;

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



bool ZSSEntityManager::SaveZones(string strSaveDir)
{
	string filename;

	if(strSaveDir.empty())
		filename = m_kWorldDirectory;	
	else
		filename = strSaveDir;
	
	filename+="/zones.dat";
	
	
	//cout<<"saving to :"<<filename<<endl;
	
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
   //cout<<"Next id is:"<<m_iNextEntityID<<endl;
   kFile.Write(&m_iNextEntityID,sizeof(int),1);
	
	
	for(int i=0; i<iNumOfZone; i++) 
	{
		kFile.Write(&m_kZones[i].m_iStatus, 	sizeof(m_kZones[i].m_iStatus), 1);
		kFile.Write(&m_kZones[i].m_bNew, 		sizeof(m_kZones[i].m_bNew), 1);
		kFile.Write(&m_kZones[i].m_iVersion,	sizeof(m_kZones[i].m_iVersion), 1);										
		kFile.Write(&m_kZones[i].m_iZoneID, 	sizeof(m_kZones[i].m_iZoneID), 1);
		kFile.Write(&m_kZones[i].m_kSize, 		sizeof(m_kZones[i].m_kSize), 1);		
		kFile.Write(&m_kZones[i].m_kPos, 		sizeof(m_kZones[i].m_kPos), 1);
		kFile.Write(&m_kZones[i].m_kTestPos, 	sizeof(m_kZones[i].m_kTestPos), 1);
		

		char temp[128];
		strcpy(temp,m_kZones[i].m_strEnviroment.c_str());
		kFile.Write(temp, 128, 1);

		int iNumOfLinks = m_kZones[i].m_iZoneLinks.size();		
		kFile.Write(&iNumOfLinks, sizeof(iNumOfLinks), 1);		
		
		for(int zl=0; zl < iNumOfLinks; zl++)
			kFile.Write(&m_kZones[i].m_iZoneLinks[zl], sizeof(m_kZones[i].m_iZoneLinks[zl]), 1);
	}



	kFile.Close();

	//cout<<"zones saved"<<endl;
	return true;
}

bool ZSSEntityManager::SaveGlobalEntitys(string strSaveDir )
{
	string filename;

	if(strSaveDir.empty())
		filename = m_kWorldDirectory;	
	else
		filename = strSaveDir;
	
	filename+="/globals.dat";

	//cout<<"saving global entitys"<<endl;
	
	ZFVFile kFile;
	if(!kFile.Open(filename.c_str(),0,true))
	{	
		cout<<"Could not open global entitys save file"<<endl;
		return false;
	}	
	
	m_pkGlobalEntity->Save(&kFile);
	
	kFile.Close();
	
	return true;
}

bool ZSSEntityManager::LoadGlobalEntitys(string strSaveDir )
{
	string filename;

	if(strSaveDir.empty())
		filename = m_kWorldDirectory;	
	else
		filename = strSaveDir;
	
	filename+="/globals.dat";

	//cout<<"loading global entitys"<<endl;
	
	ZFVFile kFile;
	if(!kFile.Open(filename.c_str(),0,false))
	{	
		cout<<"Could not open global entitys save file"<<endl;
		return false;
	}	
	
	m_pkGlobalEntity->Load(&kFile,false,true);
	
	kFile.Close();	
	
	return true;
}


bool ZSSEntityManager::SaveTrackers(string strSaveDir)
{
	string filename;

	if(strSaveDir.empty())
		filename = m_kWorldDirectory;	
	else
		filename = strSaveDir;
	
	filename+="/trackers.dat";
	
	
	//cout<<"Saving trackeres to :"<<filename<<endl;
	
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
		int iZone = GetZoneIndex((*iT)->GetEntity()->GetWorldPosV(),(*iT)->GetEntity()->m_iCurrentZone,(*iT)->m_bClosestZone);				
		kFile.Write(&iZone, sizeof(iZone), 1);		
		//cout<<"Saving tracker for zone:"<<iZone<<endl;			
	}
	
	kFile.Close();
	
	//cout<<"trackers saved"<<endl;
	
	return true;
}

bool ZSSEntityManager::LoadTrackers(string strSaveDir)
{
	string filename;

	if(strSaveDir.empty())
		filename = m_kWorldDirectory;	
	else
		filename = strSaveDir;
	
	filename+="/trackers.dat";
	
	
	//cout<<"Loading trackers from:"<<filename<<endl;
	
	ZFVFile kFile;
	if(!kFile.Open(filename.c_str(),0,false))
	{	
		cout<<"Could not open tracker save file"<<endl;
		return false;
	}


	int iNrOfTrackers;
	kFile.Read(&iNrOfTrackers,sizeof(iNrOfTrackers),1);
	//cout<<"number of trackers: "<<iNrOfTrackers<<endl;
	
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
	
	//cout<<"trackers loaded"<<endl;
	
	return true;
}

ZoneData* ZSSEntityManager::GetZoneData(int iID)
{
	if(iID < 0 || iID >= (int) m_kZones.size())
		return NULL;

	return &m_kZones[iID];

}

void ZSSEntityManager::LoadZone(int iId,string strSaveDir)
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
	kZoneEntity->m_iCurrentZone = iId;						//hack fï¿½ att snabbt kunna se villken zon zonentityt tillhï¿½
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

void ZSSEntityManager::UnLoadZone(int iId)
{
	ZoneData* kZData = GetZoneData(iId);
	if(kZData->m_pkZone == NULL)
		return;	

			
	SaveZone(iId);
	
	Delete(kZData->m_pkZone);
	kZData->m_pkZone = NULL;

	kZData->m_iStatus = EZS_UNLOADED;		
}

void ZSSEntityManager::SaveZone(int iId,string strSaveDir )
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


int ZSSEntityManager::GetUnusedZoneID()
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
	
	
	//cout<<"creating "<<newzone.m_iZoneID<<" "<<m_kZones[newzone.m_iZoneID].m_iZoneID<<endl;
	
	return newzone.m_iZoneID;
}



void ZSSEntityManager::ClearZoneLinks(int iId)
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

bool ZSSEntityManager::IsInsideZone(const Vector3& kPos,const Vector3& kSize)
{
	Vector3 kOddSize;
	kOddSize.x = (kSize.x/2 - int(kSize.x/2));
	kOddSize.y = (kSize.y/2 - int(kSize.y/2));
	kOddSize.z = (kSize.z/2 - int(kSize.z/2));

	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;

		if(BoxVSBox(kPos+kOddSize,kSize-0.1,m_kZones[i].m_kTestPos,m_kZones[i].m_kSize))
			return true;
	}
	return false;
	
}

bool ZSSEntityManager::ZoneHaveNeighbour(const Vector3& kPos,const Vector3& kSize)
{
	Vector3 kOddSize, kOddSize2;
	kOddSize.x = (kSize.x/2 - int(kSize.x/2));
	kOddSize.y = (kSize.y/2 - int(kSize.y/2));
	kOddSize.z = (kSize.z/2 - int(kSize.z/2));

	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;

		kOddSize.x = (kSize.x/2 - int(kSize.x/2));
		kOddSize.y = (kSize.y/2 - int(kSize.y/2));
		kOddSize.z = (kSize.z/2 - int(kSize.z/2));

		if(!BoxVSBox(kPos+kOddSize,kSize-0.1,m_kZones[i].m_kPos,m_kZones[i].m_kSize))
			if(BoxVSBox(kPos+kOddSize,kSize,m_kZones[i].m_kPos,m_kZones[i].m_kSize))
				return true;
	}
	return false;
}


void ZSSEntityManager::UpdateZoneLinks(int iId)
{
	ZoneData* pkZone = GetZoneData(iId);
	if(!pkZone)
		return;

	//clear all connections
	ClearZoneLinks(iId);
	
	
	Vector3 kPos = m_kZones[iId].m_kTestPos;
	Vector3 kSize = m_kZones[iId].m_kSize;

	//go trough all zones and check if they are to be connected
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(m_kZones[i].m_iStatus == EZS_UNUSED)
			continue;
	
		if((int)i == iId)
			continue;
			
		if(BoxVSBox(kPos,kSize ,m_kZones[i].m_kTestPos, m_kZones[i].m_kSize))
		{
			m_kZones[iId].m_iZoneLinks.push_back(i);
			m_kZones[i].m_iZoneLinks.push_back(iId);
		}
	}
}


bool ZSSEntityManager::BoxVSBox(Vector3 kPos1,Vector3 kSize1,Vector3 kPos2,Vector3 kSize2)
{
	static Vector3 kSizeCopy1;
	static Vector3 kSizeCopy2;
	
	kSizeCopy1 = kSize1 / 2.0;
	kSizeCopy2 = kSize2 / 2.0;

	if( (kPos2.x - kSizeCopy2.x ) > (kPos1.x + kSizeCopy1.x ) )
		return false;
		
	if( (kPos2.x + kSizeCopy2.x ) < (kPos1.x - kSizeCopy1.x ) )
		return false;
		
	if( (kPos2.z - kSizeCopy2.z ) > (kPos1.z + kSizeCopy1.z ) )
		return false;

	if( (kPos2.z + kSizeCopy2.z ) < (kPos1.z - kSizeCopy1.z ) )
		return false;

	if( (kPos2.y - kSizeCopy2.y ) > (kPos1.y + kSizeCopy1.y ) )
		return false;	
		
	if( (kPos2.y + kSizeCopy2.y ) < (kPos1.y - kSizeCopy1.y ) )
		return false;


	return true;			

	
// 	//box 1
// 	float x1 = float(kSize1.x/2.0);
// 	float y1 = float(kSize1.y/2.0);
// 	float z1 = float(kSize1.z/2.0);
// 	
// 	vector<Vector3>	kTestDirs1;
// 	kTestDirs1.push_back(Vector3(-x1,y1,z1));
// 	kTestDirs1.push_back(Vector3(x1,y1,z1));
// 	kTestDirs1.push_back(Vector3(x1,-y1,z1));
// 	kTestDirs1.push_back(Vector3(-x1,-y1,z1));
// 	
// 	kTestDirs1.push_back(Vector3(-x1,y1,-z1));
// 	kTestDirs1.push_back(Vector3(x1,y1,-z1));
// 	kTestDirs1.push_back(Vector3(x1,-y1,-z1));
// 	kTestDirs1.push_back(Vector3(-x1,-y1,-z1));
// 	
// 	//box 2
// 	float x2 = float(kSize2.x/2.0);
// 	float y2 = float(kSize2.y/2.0);
// 	float z2 = float(kSize2.z/2.0);
// 	
// 	vector<Vector3>	kTestDirs2;
// 	kTestDirs2.push_back(Vector3(-x2,y2,z2));
// 	kTestDirs2.push_back(Vector3(x2,y2,z2));
// 	kTestDirs2.push_back(Vector3(x2,-y2,z2));
// 	kTestDirs2.push_back(Vector3(-x2,-y2,z2));
// 	
// 	kTestDirs2.push_back(Vector3(-x2,y2,-z2));
// 	kTestDirs2.push_back(Vector3(x2,y2,-z2));
// 	kTestDirs2.push_back(Vector3(x2,-y2,-z2));
// 	kTestDirs2.push_back(Vector3(-x2,-y2,-z2));
// 
// 	unsigned int i;
// 	// box1 vs box2
// 	for( i = 0 ;i<kTestDirs1.size();i++)
// 	{
// 		Vector3 kPoint = kPos1 + kTestDirs1[i];
// 		
// 		if(kPoint.x < (kPos2.x - x2))	continue;		
// 		if(kPoint.y < (kPos2.y - y2))	continue;
// 		if(kPoint.z < (kPos2.z - z2))	continue;
// 		if(kPoint.x > (kPos2.x + x2))	continue;
// 		if(kPoint.y > (kPos2.y + y2))	continue;
// 		if(kPoint.z > (kPos2.z + z2))	continue;
// 		return true;
// 	}
// 
// 	// box2 vs box1
// 	for( i = 0 ;i<kTestDirs2.size();i++)
// 	{
// 		Vector3 kPoint = kPos2 + kTestDirs2[i];
// 		
// 		if(kPoint.x < (kPos1.x - x1))	continue;		
// 		if(kPoint.y < (kPos1.y - y1))	continue;
// 		if(kPoint.z < (kPos1.z - z1))	continue;
// 		if(kPoint.x > (kPos1.x + x1))	continue;
// 		if(kPoint.y > (kPos1.y + y1))	continue;
// 		if(kPoint.z > (kPos1.z + z1))	continue;
// 		return true;
// 	}
// 
// 	return false;
}


void ZSSEntityManager::SetZoneModel(const char* szName,int iId)
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
		
			
	if(zd->m_pkZone->GetProperty("P_Heightmap"))
	{
		cout<<"cant set model on a heightmap zone"<<endl;
		return;
	}
			
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
// 	P_PfMesh* pkMesh = (P_PfMesh*)zd->m_pkZone->GetProperty("P_PfMesh");
// 	
// 	if(!pkMesh)
// 		pkMesh = (P_PfMesh*)zd->m_pkZone->AddProperty("P_PfMesh");
// 	
// 	if(pkMesh)
// 		pkMesh->SetMad(mp);



	//setup physics
	P_Tcs* pp = (P_Tcs*)zd->m_pkZone->GetProperty("P_Tcs");
	
	if(!pp)
		pp = (P_Tcs*)zd->m_pkZone->AddProperty("P_Tcs");	
	
	if(pp)
	{
		//pp->SetPolygonTest(true);	
		pp->SetTestType(E_MESH);
		pp->SetStatic(true);			
		pp->SetRefetchPolygonData();
		pp->SetGroup(0);
	}

}



void ZSSEntityManager::ForceUnload()
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

void ZSSEntityManager::ForceSave()
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



void ZSSEntityManager::ResetNetUpdateFlags(int iConID)
{
	for(map<int,Entity*>::iterator it=m_akEntitys.begin();it!=m_akEntitys.end();it++) {
		(*it).second->ResetAllNetFlags(iConID);
	}
}


bool ZSSEntityManager::SaveWorld(string strSaveDir,bool bForce)
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
	
	//save global entity
	if(!SaveGlobalEntitys(strSaveDir))
	{
		cout<<"ERROR: could not save global entity"<<endl;
		return false;
	}	
	
	//save zone list
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

bool ZSSEntityManager::LoadWorld(string strLoadDir)
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
			
	//load global entity
	if(!LoadGlobalEntitys(strLoadDir))
	{
		cout<<"WARNING: could not load global entity"<<endl;
		//return false;
	}	
		
	//do a zone update
//	UpdateZones();	
	UpdateZoneSystem();
	
	return true;
}


#include "inputhandle.h"
void ZSSEntityManager::UpdateZoneSystem()
{
// 	if(m_pkZeroFps->m_pkApp->m_pkInputHandle->Pressed(KEY_P))
// 		m_pkSceneAABBTree->RebuildTree();


// 	StartProfileTimer("s__ZoneSystem");	
	UpdateTrackers();
	UpdateZoneStatus();
// 	StopProfileTimer("s__ZoneSystem");	
}

void ZSSEntityManager::UpdateTrackers()
{
	static Vector3 kTrackerPos;
	static int iZoneIndex;	
	static vector<ZoneData*>	kFloodZones;
	static set<int>				kNewActiveZones;
	static P_Track*				pkTracker;
	
	static int iZones;
	static int iZoneLinks;
	static ZoneData* pkZone;
	static ZoneData* pkOtherZone;
	
	// Set All Zones as untracked
	iZones = m_kZones.size();
	for(int iZ=0;iZ<iZones;iZ++) 
	{
		pkZone = &m_kZones[iZ];
		
		pkZone->m_bTracked	= false;
		pkZone->m_bActive		= false;
	}


	// For each tracker.
	for(list<P_Track*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) 
	{
		pkTracker = *iT;
	
		// Find Active Zone.
		kNewActiveZones.clear();
		
		//set initi range
		iZones = m_kZones.size();
		for(int iZ=0;iZ<iZones;iZ++)
			m_kZones[iZ].m_iRange = 10000;
		
		//get trackerpos
		kTrackerPos = 	pkTracker->GetEntity()->GetWorldPosV();
			
		//get current zone
		iZoneIndex = GetZoneIndex(pkTracker->GetEntity()->GetWorldPosV(),pkTracker->m_iCurrentTrackedZone,pkTracker->m_bClosestZone);		
		pkTracker->m_iCurrentTrackedZone = iZoneIndex;
		
		if(iZoneIndex == -1)
		{
			//cout<<"ERROR: Tracker not in zone"<<endl;
			continue;
		}
		
		//start zone
		ZoneData* pkStartZone = GetZoneData(iZoneIndex);
		pkStartZone->m_iRange = 0;						
		
		if( pkTracker->m_bOneZoneOnly )
		{
			//add the one andonly zone to new active zones
			kNewActiveZones.insert(iZoneIndex);
			pkStartZone->m_bTracked = true;
			pkStartZone->m_bActive = true;
		}
		else
		{			
			//add the first zone to flood zones (search will start from here)
			kFloodZones.push_back(pkStartZone);						
			
			// Flood Zones in rage to active.
			while(!kFloodZones.empty()) 
			{
				pkZone = kFloodZones.back();
				kFloodZones.pop_back();
	
				kNewActiveZones.insert(pkZone->m_iZoneID);
	
				pkZone->m_bTracked = true;
				int iRange = pkZone->m_iRange + 1;
	
				if(kTrackerPos.DistanceTo(pkZone->m_kPos) <= float(m_kiTrackerLOS.GetInt()))
				{
					pkZone->m_bActive = true;
				
					iZoneLinks = pkZone->m_iZoneLinks.size();
					for(unsigned int i=0; i<iZoneLinks; i++) 
					{
						pkOtherZone = GetZoneData(pkZone->m_iZoneLinks[i]); //				pkZone->m_pkZoneLinks[i];	//GetZoneData(pkZone->m_iZoneLinks[i]);				
	
						//if zone has already been checked continue whit the next one
						if(pkOtherZone->m_iRange <= iRange)	continue;		// Dvoid: ï¿½drade till <= frï¿½ <  , tycks snabba upp algoritmen med en faktor av ca 100000000 (pga att den lï¿½ger till samma zon flera gï¿½ger)
						
						//set new range 
						pkOtherZone->m_iRange = iRange;
											
						
						//add zone to flooded zones list
						kFloodZones.push_back(pkOtherZone);
					}				
				}
			}
		}
		
					
		//find new loaded zones  , compare new actives zones whit last update to find new loaded zones
		pkTracker->m_kNewActiveZones.clear();
		set_difference(kNewActiveZones.begin(),kNewActiveZones.end(),pkTracker->m_kActiveZones.begin(),pkTracker->m_kActiveZones.end(), inserter(pkTracker->m_kNewActiveZones, pkTracker->m_kNewActiveZones.begin()));
		
		//findout wich zones has been removed since last update, and add them to list to be sent to client (observer the list shuld not be cleared here, but in the code that sends the package)
		if(pkTracker->m_iConnectID != -1)
			set_difference(pkTracker->m_kActiveZones.begin(),pkTracker->m_kActiveZones.end(),kNewActiveZones.begin(),kNewActiveZones.end(), inserter(pkTracker->m_kUnloadZones, pkTracker->m_kUnloadZones.begin()));		
		
		//save new active zones in tracker
		pkTracker->m_kActiveZones = kNewActiveZones;
	}
}

void ZSSEntityManager::UpdateZoneStatus()
{
	static int iZones;
	static ZoneData* pkZone;
	static float fCurrentTime;
	
	
	int iIoOps = 0;
	fCurrentTime = m_pkZeroFps->GetEngineTime();

	
	iZones = m_kZones.size();
	for(int i=0;i<iZones;i++) 
	{
		pkZone = &m_kZones[i];
	
		if(pkZone->m_iStatus != EZS_UNUSED)
		{
			//the zone is currently tracked
			if(pkZone->m_bTracked)
			{
				//zone is cached , lets activate it
				if(pkZone->m_iStatus == EZS_CACHED)
				{
					//cout<<"activating cached zone "<<i<<endl;															
					pkZone->m_iStatus = EZS_LOADED;					
					
					if(pkZone->m_pkZone)
						pkZone->m_pkZone->SetUpdateStatus(UPDATE_ALL);
					continue;
				}
			
				//zone is unloaded , lests load it
				if(pkZone->m_iStatus == EZS_UNLOADED)
				{
					if(iIoOps >= m_kiMaxZoneIO.GetInt())
						continue;
					iIoOps++;
					
					// zone status is set in loadzone()
					LoadZone(i);
					
					if(pkZone->m_pkZone)
						pkZone->m_pkZone->SetUpdateStatus(UPDATE_ALL);
						
					continue;
				}				
			}
			//the zone is not tracked
			else
			{
				//zone is loaded, set it as cached
				if(pkZone->m_iStatus == EZS_LOADED)
				{
					//cout<<"zone is no longer tracked, setting as cached and starting timout "<<i<<endl;
					
					pkZone->m_iStatus = EZS_CACHED;
					
					if(pkZone->m_pkZone)
						pkZone->m_pkZone->SetUpdateStatus(UPDATE_NONE);
						
					pkZone->m_fInactiveTime = fCurrentTime;
					continue;
				}
				
				//zone is cached, wait for timeout
				if(pkZone->m_iStatus == EZS_CACHED)
				{
					if( (fCurrentTime - pkZone->m_fInactiveTime) > m_kfZoneUnloadTime.GetFloat())
					{						
						if(iIoOps >= m_kiMaxZoneIO.GetInt())
							continue;

						iIoOps++;
					
						//cout<<"cached zone timed out, unloading "<<i<<endl;
						
						pkZone->m_iStatus = EZS_UNLOADED;						
						UnLoadZone(i);		
						
						continue;
					}
				}				
			}
		}
	}
}

bool ZSSEntityManager::CallFunction(Entity* pkEntity, const char* acFunction,vector<ScriptFuncArg>* pkParams)
{
	//cout << "Calling Func: " << acFunction << endl;
	if(pkEntity->m_bZone)
		return false;

			
	ObjectManagerLua::Push();
	//set self id before calling the funktion
	ObjectManagerLua::g_kScriptState.g_iCurrentObjectID = pkEntity->m_iEntityID;	
	bool bRes;

	
	if(pkParams)
		bRes = m_pkScript->Call(pkEntity->GetEntityScript(), (char*)acFunction,*pkParams);
	else
		bRes =  m_pkScript->Call(pkEntity->GetEntityScript(), (char*)acFunction,0,0);
	
	ObjectManagerLua::Pop();
	return bRes;
}


void ZSSEntityManager::ClearClientDeleteQueue(int iClient)
{
	while(!m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.empty())
		m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.pop();
}

void ZSSEntityManager::ClearClientsDeleteQueues()
{
	for(int i = 0;i<m_pkZeroFps->GetMaxPlayers();i++)
		ClearClientDeleteQueue(i);
}

void ZSSEntityManager::AddEntityToClientDeleteQueue(int iClient,int iEntityID)
{
	if(!m_pkNetWork->IsConnected(iClient))
	{
		//cout<<"WARNING: tried to add entity to unconnected client delete list"<<endl;
		return;	
	}
	
	m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.push(iEntityID);
	
	//entity shuld not exist anymore on the client, after this
	if(Entity* pkEnt = GetEntityByID(iEntityID))
	{
		//cout<<"entity exist"<<endl;
		pkEnt->ResetAllNetFlagsAndChilds(iClient);		
// 		pkEnt->SetExistOnClient(iClient,false);	
	}
	
	//cout<<"added entity:"<<iEntityID<<" to client "<<iClient<< " delete queue"<<endl;
}

void ZSSEntityManager::AddEntityToAllClientDeleteQueues(int iEntityID)
{
	if(Entity* pkEntity = GetEntityByID(iEntityID))
	{	
		for(int i = 0;i<m_pkZeroFps->GetMaxPlayers();i++)
		{
			//only add to clients who is suppose to have the entity			
//			if(pkEntity->GetExistOnClient(i))
//			{			
				AddEntityToClientDeleteQueue(i,iEntityID);
//			}
		}	
	}
}


void ZSSEntityManager::SendDeleteQueue(int iClient)
{
	if(m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.empty())
		return;
		
	//cout<<"sending delete list to client:"<<iClient<<" size:"<<m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.size()<<endl;
		
	m_OutNP.Write((char) ZPGP_DELETELIST);
	
	int iID;
	while(!m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.empty())
	{
		iID = m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.front();
			
		/*	
		//get entity
		if(Entity* pkEnt = GetEntityByID(iID))
		{
			//does it exist on the client? if not skip this one
			if(!pkEnt->GetExistOnClient(iClient))
			{
				//pop this entity from queue
				m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.pop();
				continue;
			}
				
			//clear existance 
			pkEnt->ResetAllNetUpdateFlagsAndChilds(iClient);		
			pkEnt->SetExistOnClient(iClient,false);	
		}
		*/
	
		//send delete
		m_OutNP.Write(iID);
		
		//pop this entity from queue
		m_pkZeroFps->m_kClient[iClient].m_kDeleteQueue.pop();
	}
	
	m_OutNP.Write(-1);
	
	
}

void ZSSEntityManager::HandleDeleteQueue(NetPacket* pkNetPacket)
{
	int iEntityID;
	
	pkNetPacket->Read(iEntityID);
	
	while(iEntityID != -1)
	{
		if(Entity* pkEnt = GetEntityByID(iEntityID))
		{
			if(pkEnt->m_eRole == NETROLE_PROXY)
			{
				//cout<<"deleting entity:"<<iEntityID<<endl;
				Delete(iEntityID);		
			}
			else
				cout<<"WARNING: got request to delete a non proxy entity:"<<iEntityID<<endl;
		}
		
		pkNetPacket->Read(iEntityID);
	}
}

void ZSSEntityManager::DeleteUnloadedZones(int iClient)
{	
	for(set<int>::iterator itActiveZone = m_pkZeroFps->m_kClient[iClient].m_kUnloadZones.begin(); itActiveZone != m_pkZeroFps->m_kClient[iClient].m_kUnloadZones.end(); itActiveZone++ ) 
	{
		AddEntityToClientDeleteQueue(iClient,m_kZones[ (*itActiveZone) ].m_iZoneObjectID);		
	}	
}


void ZSSEntityManager::RecreateEntitys(const string& strType)
{
	//loop trough all zones
	for(int i = 0;i<m_kZones.size();i++)
	{
		//is this zone used?
		if(m_kZones[i].m_iStatus != EZS_UNUSED)
		{
			//load zone if not loaded
			if(m_kZones[i].m_iStatus == EZS_UNLOADED)
				LoadZone(i);
				
			if(m_kZones[i].m_iStatus == EZS_CACHED)
				m_kZones[i].m_iStatus = EZS_LOADED;
				
			//get all entitys in zone
			vector<Entity*>	kEntitys;
			m_kZones[i].m_pkZone->GetAllEntitys(&kEntitys,true);

			//loop all entitys
			for(int j = 0;j<kEntitys.size();j++)
			{
				//matching type
				if(kEntitys[j]->GetType() == strType)
				{
					//try creating entity from script
					if(Entity* pkNew = CreateEntityFromScript(kEntitys[j]->m_pScriptFileHandle->GetRes().c_str()))
					{
						m_pkSystem->Printf( (string("Recreating entityID: ") + IntToString(kEntitys[j]->GetEntityID())).c_str() );
						
						pkNew->SetUseZones(kEntitys[j]->GetUseZones());
						pkNew->SetParent(kEntitys[j]->GetParent());						
						pkNew->SetRelativeOri(kEntitys[j]->GetRelativeOri());
						pkNew->SetLocalPosV(kEntitys[j]->GetLocalPosV());
						pkNew->SetLocalRotM(kEntitys[j]->GetLocalRotM());
						
						if(pkNew->GetLocalPosV() != kEntitys[j]->GetLocalPosV())
							cout<<"Error setting pos"<<endl;
						
						Delete(kEntitys[j]);						
					}
					else
					{
						m_pkSystem->Printf( (string("Could not recreate entityID: ") + IntToString(kEntitys[j]->GetEntityID())).c_str() );
					}
				}
			}
		}
	}
}




/*
void ZSSEntityManager::PackZoneListToClient(int iClient, set<int>& iZones)
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
*/


/*
void ZSSEntityManager::UpdateZoneList(NetPacket* pkNetPacket)
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
//	LOGSIZE("ZSSEntityManager::ZoneList", kZones.size() * 4 + 4);


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
		}*
}














void ZSSEntityManager::UpdateRenderPropertys(Entity* pkRootEntity,bool bForceRootOnly,bool bUpdateList)
{
	m_iUpdateFlags = PROPERTY_TYPE_RENDER | PROPERTY_SIDE_CLIENT;	
	
	if(bUpdateList)
	{	
		m_akPropertys.clear();	
		
		//get propertys
		if(bForceRootOnly)
		{
			if(pkRootEntity)
				pkRootEntity->GetRenderPropertys(&m_akPropertys,true);
			else
				m_pkWorldEntity->GetRenderPropertys(&m_akPropertys,true);
		}
		else
		{	
			if(pkRootEntity)
				pkRootEntity->GetRenderPropertys(&m_akPropertys);
			else
				m_pkWorldEntity->GetRenderPropertys(&m_akPropertys);
		}
	
		//sort
		sort(m_akPropertys.begin(),m_akPropertys.end(),Less_Property);
	}		
	
		
	//update render propertys update counter	
	m_iRenderUpdates += m_akPropertys.size();

	//run update in all propertys
	int iPropertys = m_akPropertys.size();
	for(int i = 0;i<iPropertys;i++)
		m_akPropertys[i]->Update();

}*/