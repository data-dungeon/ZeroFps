#include "entitymanager.h"
#include "network.h"
#include "zerofps.h"
#include "../basic/zfsystem.h"
#include "../basic/simplescript.h"
#include "../engine_systems/propertys/p_primitives3d.h"
#include "../engine_systems/propertys/p_track.h"
#include "../engine_systems/propertys/p_mad.h"
#include "../engine_systems/propertys/p_physic.h"
#include "fh.h"
#include "../engine_systems/script_interfaces/si_objectmanager.h"
#include "p_pfmesh.h"

ZoneData& ZoneData::operator=(const ZoneData &kOther) 
{
	m_pkZone				= kOther.m_pkZone;
	m_iZoneID			= kOther.m_iZoneID;
	m_kPos				= kOther.m_kPos;
	m_kSize				= kOther.m_kSize;
	m_iZoneLinks		= kOther.m_iZoneLinks;
	m_fInactiveTime	= kOther.m_fInactiveTime;
	m_bActive			= kOther.m_bActive;
	m_iRange				= kOther.m_iRange;
	return *this;
}

ZoneData::ZoneData()
{
	m_bNew = false;
	m_bUsed = false;
	m_pkZone = NULL;
	m_iZoneID = 0;
	m_kPos.Set(0,0,0);
	m_kSize.Set(0,0,0);

	m_fInactiveTime = 0;
	m_bActive = false;
	m_iRange = 0;		
	m_bUnderContruction = false;
	m_iRevision = 0;
	
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

	iNextObjectID				= 0;
	m_bUpdate					= true;
	m_iTotalNetObjectData	= 0;
	m_iNumOfNetObjects		= 0;
	m_bDrawZones				= false;
	m_bDrawZoneConnections	= false;
	m_pScriptFileHandle		= NULL;
	m_iTrackerLOS				= 3;	

	Register_Cmd("o_logtree",FID_LOGOHTREE);	
	Register_Cmd("o_dumpp",FID_LOGACTIVEPROPERTYS);	
	Register_Cmd("sendmsg",FID_SENDMESSAGE, CSYS_FLAG_SRC_ALL, "sendmsg name id",2);	

	Register_Cmd("newworld",FID_NEWWORLD, CSYS_FLAG_SRC_ALL);	
	Register_Cmd("loadworld",FID_LOADWORLD, CSYS_FLAG_SRC_ALL);	
	Register_Cmd("saveworld",FID_SAVEWORLD, CSYS_FLAG_SRC_ALL);		
	Register_Cmd("setworlddir",FID_SETWORLDDIR, CSYS_FLAG_SRC_ALL);		
	
	Register_Cmd("loadzones",FID_LOADZONES, CSYS_FLAG_SRC_ALL);	
	Register_Cmd("savezones",FID_SAVEZONE, CSYS_FLAG_SRC_ALL);	

	RegisterVariable("l_showzones", &m_bDrawZones,					CSYS_BOOL);
	RegisterVariable("l_showconn",  &m_bDrawZoneConnections,		CSYS_BOOL);
	
	RegisterVariable("l_trackerlos", &m_iTrackerLOS, CSYS_INT);	
}

bool EntityManager::StartUp()	
{
	m_pkZeroFps	=	static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));		
	m_pkNetWork	= static_cast<NetWork*>(GetSystem().GetObjectPtr("NetWork"));
	m_pkScript  = static_cast<ZFScriptSystem*>(GetSystem().GetObjectPtr("ZFScriptSystem"));

	m_fEndTimeForceNet		= m_pkZeroFps->GetEngineTime();

	m_kWorldDirectory = "../data/testmap";

	//create all base objects
	Clear();

	//setup script interface
	ObjectManagerLua::Init(this,m_pkScript);
	
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
	float fAvgObjSize;

	if(m_iNumOfNetObjects) {
		fAvgObjSize = float(m_iTotalNetObjectData / m_iNumOfNetObjects);
		}

	Logf("net", " Avg Obj Size: %f\n", fAvgObjSize);

	// Obs! Här skall resursen laddas ur, går dock inte pga timeout expire.
	//delete m_pScriptFileHandle;

}

/**	\brief	Link this to the Object manager

  This function is called by objects as they are created. It assigned a NetWorkID to the object and
  also put them in the ObjectManger.
*/
void EntityManager::Link(Entity* pkObject) 
{
	pkObject->iNetWorkID = iNextObjectID++;
	m_akObjects.push_back(pkObject);
}

/**	\brief	UnLink this from Object Manger.

  Remvoves object from the Object Manger.
*/
void EntityManager::UnLink(Entity* pkObject) 
{	
	// If i own object mark so we remove it on clients.
	//	if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
	//		m_aiNetDeleteList.push_back(pkObject->iNetWorkID);
	m_akObjects.remove(pkObject);
}

/**	\brief	Delete all objects.

	Clear all data from ObjectManger.
*/
void EntityManager::Clear()
{
	//delete all objects in world
	while(m_akObjects.begin() != m_akObjects.end())
		delete(*m_akObjects.begin());
	
	//clear all zones
	m_kZones.clear();
	
	// Recreate base objects
	CreateBaseObjects();
}

/**	\brief	Create the top level objects.

*/
void EntityManager::CreateBaseObjects()
{
	iNextObjectID = 0;
	
	//top world object parent to all objects
	m_pkWorldObject						=	new Entity();	
	m_pkWorldObject->GetName()			= "WorldObject";
	m_pkWorldObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkWorldObject->m_eRemoteRole	= NETROLE_NONE;

	//object that is parent to all zones
	m_pkZoneObject							=	new Entity();	
	m_pkZoneObject->SetParent(m_pkWorldObject);
	m_pkZoneObject->GetName()			= "ZoneObject";
	m_pkZoneObject->m_eRole				= NETROLE_AUTHORITY;
	m_pkZoneObject->m_eRemoteRole		= NETROLE_NONE;

	//object that is parent to all client objects
	m_pkClientObject						=	new Entity();	
	m_pkClientObject->SetParent(m_pkWorldObject);
	m_pkClientObject->GetName()		= "ClientObject";
	m_pkClientObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkClientObject->m_eRemoteRole	= NETROLE_NONE;

	//object that is parent to all global objects (server information etc)
	m_pkGlobalObject						=	new Entity();	
	m_pkGlobalObject->SetParent(m_pkWorldObject);
	m_pkGlobalObject->GetName()		= "GlobalObject";
	m_pkGlobalObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkGlobalObject->m_eRemoteRole	= NETROLE_NONE;

	iNextObjectID = 100000;
}



/**	\brief	Creates a new clean object.

	Creates a basic object without any propertys and all values set to defualt. 
*/
Entity* EntityManager::CreateObject()
{
	Entity* pkObj = new Entity;
	//cout << "CreateObject :" << pkObj->iNetWorkID << endl;
	return pkObj;
}

/**	\brief	Adds an object to delete qeue
*/
void EntityManager::Delete(Entity* pkObject) 
{
	if(pkObject == NULL)
		return;

	for(vector<int>::iterator it=m_aiDeleteList.begin();it!=m_aiDeleteList.end();it++) 
	{
		if(pkObject->iNetWorkID == (*it)) {
			Logf("net", "Object [%d] already in delete list\n", pkObject->iNetWorkID);
			//cout << "Object already in delete list" << endl;
			return;
		}
	}
	
	m_aiDeleteList.push_back(pkObject->iNetWorkID);
}

/**	\brief	Adds an object to delete qeue

	Walk DeleteList and delete all objects in it.
*/
void EntityManager::UpdateDelete()
{
	int iSize = m_aiDeleteList.size();

	if(m_aiDeleteList.size()==0)
		return;
	
	for(vector<int>::iterator it=m_aiDeleteList.begin();it!=m_aiDeleteList.end();it++) 
	{
		Entity* pkObject = GetObjectByNetWorkID((*it));

		if(pkObject) { // If i own object mark so we remove it on clients.
			/*if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
				m_aiNetDeleteList.push_back((*it));*/
			delete pkObject;		
			}
	}

	m_aiDeleteList.clear();

}

/*
void EntityManager::UpdateDeleteList(NetPacket* pkNetPacket)
{
	Object* pkNetSlave;
	int iObjectID;
	pkNetPacket->Read(iObjectID);

	while(iObjectID != -1) {
		//Logf("net", "Delete: Object %d\n", iObjectID);
		pkNetSlave = GetObjectByNetWorkID(iObjectID);
		if(pkNetSlave == NULL) {
			//Logf("net", " Object '%d' not found.\n", iObjectID);	
			}
		else {
			Delete(pkNetSlave);
			}
		pkNetPacket->Read(iObjectID);
		}	
}*/




/**	\brief	Run update on selected propertys.

	This function collects all propertys that fit the selected flags, sorts them if it needs to
	and then runs the update function of each of this propertys.

	
*/

void EntityManager::Update(int iType,int iSide,bool bSort)
{
	m_iUpdateFlags = iType | iSide;

/*	if(!m_bUpdate)
		if(iType != PROPERTY_TYPE_RENDER)
			return;*/

	GetPropertys(iType,iSide);
	
	m_iNrOfActivePropertys = m_akPropertys.size();
	
	m_pkZeroFps->DevPrintf("om", "OM::Update(%s, %s,%d) = %d",
		GetPropertyTypeName(iType),GetPropertySideName(iSide),bSort,m_iNrOfActivePropertys);

	if(bSort){
		stable_sort(m_akPropertys.begin(),m_akPropertys.end(),Less_Property);
	}
	
	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		(*it)->Update();
	}
}

bool EntityManager::IsUpdate(int iFlags)
{
	if(m_iUpdateFlags & iFlags)
		return true;

	return false;
}




void EntityManager::UpdateGameMessages(void)
{
	// Let Objects/Propertys handle messages
	for(list<Entity*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		(*it)->HandleMessages();
	}
}


/**	\brief	Creates a new clean object.

	Creates a basic object without any propertys and all values set to defualt. 
*/

Entity* EntityManager::CreateObjectByNetWorkID(int iNetID)
{
//	Object *pkNew = new NetSlaveObject;
	Entity *pkNew = CreateObject();

	//	Add(pkNew);
	pkNew->iNetWorkID = iNetID;
	pkNew->SetParent(m_pkWorldObject);
	pkNew->m_eRole			= NETROLE_PROXY;
	pkNew->m_eRemoteRole	= NETROLE_AUTHORITY;
	pkNew->SetUseZones(false);
	
//	Logf("net", " CreateObjectByNetWorkID( %d ).\n", iNetID);

//	pkNew->AddProperty("P_Primitives3D");
	return pkNew;
}

/**	\brief	Uses a script to create the object.

	Creates a object from a script and use it to set values and propertys. If script file
	is not found no object will be created. 
*/
Entity* EntityManager::CreateObjectFromScriptInZone(const char* acName,Vector3 kPos,int iCurrentZone)
{
	int id = GetZoneIndex(kPos,iCurrentZone,false);
	
	if(id == -1)
		return NULL;
	
	//force loading of this zone
	LoadZone(id);
	
	if(!m_kZones[id].m_pkZone)
		return NULL;
		
		
	Entity* newobj = CreateObjectFromScript(acName);
	
	if(newobj)
	{      
		//check if its a static object, in that case check if the zone is in consturction mode else delete the object 
		if(newobj->GetObjectType() == OBJECT_TYPE_STATIC)
		{
			ZoneData* zd = GetZoneData(id);
			if(zd)
			{
				if(!zd->m_bUnderContruction)
				{
					Delete(newobj);
					cout<<"ERROR: You cant add a static entity to a zone that is not underconstruction"<<endl;
					return NULL;		
				}
			}
		}
	
		newobj->SetUseZones(true);
		newobj->SetWorldPosV(kPos);	
		if(newobj->m_iCurrentZone == -1)
			cout<<"ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
	}

	return newobj;
}

Entity* EntityManager::CreateObjectFromScript(const char* acName)
{
	if(m_pScriptFileHandle)
		delete m_pScriptFileHandle;

	m_pScriptFileHandle = new ZFResourceHandle;
	if(!m_pScriptFileHandle->SetRes(acName))
	{
		printf("Failed to load object script %s\n", acName);
		return NULL;
	}
	
	//push all pointers
	ObjectManagerLua::Push();

	if(!m_pkScript->Call(m_pScriptFileHandle, "Create", 0, 0))
	{	
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
	
	ObjectManagerLua::g_pkReturnObject->m_strType	= &acName[pos];
	ObjectManagerLua::g_pkReturnObject->m_strName	= string("A ") + &acName[pos];
	ObjectManagerLua::g_pkReturnObject->m_pScriptFileHandle->SetRes(acName);
	
	Entity* pkReturnObj = ObjectManagerLua::g_pkReturnObject;
	
	//pop pointers
	ObjectManagerLua::Pop();

   pkReturnObj->m_strCreatedFromScript = acName;
	
	return pkReturnObj;
}

/*
bool EntityManager::IsA(Entity* pkObj, string strStringType)
{
	ObjectArcheType* pkAt = GetArcheType(pkObj->m_strType);
	if(!pkAt)
		return false;	// Object not created from archtype at all :(.
	
	ObjectArcheType* pkParentAt;


	bool bDone = false;
	while(bDone){
		if(pkAt->m_strName == strStringType)
			return true;

		pkParentAt = GetArcheType(pkAt->m_strParentName);
		if(!pkParentAt)
			return false;
		else
			pkAt = pkParentAt;
		}

	return false;
}*/

// Gets
void EntityManager::GetAllObjects(vector<Entity*> *pakObjects)
{
	m_pkWorldObject->GetAllObjects(pakObjects);
}

Entity* EntityManager::GetObject(const char* acName)
{
	vector<Entity*> kObjects;		
	GetAllObjects(&kObjects);
	
	for(vector<Entity*>::iterator it=kObjects.begin();it!=kObjects.end();it++) {
		if((*it)->GetName() == acName)
		{
			return (*it);
		}
	}	
	
	return NULL;
}

Entity*	EntityManager::GetObjectByNetWorkID(int iNetID)
{
	if(iNetID == -1)
		return NULL;

	for(list<Entity*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		if((*it)->iNetWorkID == iNetID)
		{	
			return (*it);
		}
	}
	

	return NULL;
}


// NetWork


void EntityManager::UpdateState(NetPacket* pkNetPacket)
{
	Entity* pkNetSlave;
	int iObjectID;
	pkNetPacket->Read(iObjectID);

	while(iObjectID != -1) {
		//Logf("net", " Object State[%d]\n", iObjectID);

		pkNetSlave = GetObjectByNetWorkID(iObjectID);
		if(pkNetSlave == NULL) {
				//Logf("net", " Object '%d' not found. Trying to create...\n", iObjectID);	
			pkNetSlave = CreateObjectByNetWorkID(iObjectID);
			}
				
		if( pkNetSlave ) {
//			g_ZFObjSys.Logf("net", " Refreshing object %d.\n", iObjectID);
			pkNetSlave->PackFrom(pkNetPacket, ZF_NET_NOCLIENT);
			if(pkNetPacket->IsReadError()) {
				printf("pkNetPacket Read Error\n"); 
				return;
				}

			pkNetPacket->Read(iObjectID);
			}
		else {
			//Logf("net", " Object '%d' not found (again) :(.\n", iObjectID);
			return;
			}
		}	
}

void EntityManager::PackToClient(int iClient, vector<Entity*> kObjects)
{
	int iPacketSize = 0;
	int iEndOfObject = -1;

	Entity* pkPackObj;

	NetPacket NP;
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	NP.Write((char) ZFGP_OBJECTSTATE);

	for(unsigned int iObj=0; iObj < kObjects.size(); iObj++)	{
		pkPackObj = kObjects[iObj];

		pkPackObj->m_iNetUpdateFlags |= m_iForceNetUpdate;

		if(pkPackObj->NeedToPack() == false)				continue;
		if(pkPackObj->m_eRole != NETROLE_AUTHORITY)		continue;

		NP.Write(pkPackObj->iNetWorkID);
		//Logf("net", "Object [%d]\n",pkPackObj->iNetWorkID );
		pkPackObj->PackTo(&NP,iClient);
		iPacketSize++;

		//Logf("net", " Size: %d\n\n",NP.m_iPos );

		if(NP.m_iPos >= 512) {
			NP.Write(iEndOfObject);
			NP.Write(ZFGP_ENDOFPACKET);
			m_pkNetWork->SendToClient(iClient, &NP);

			NP.Clear();
			NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
			NP.Write((char) ZFGP_OBJECTSTATE);

			iPacketSize = 0;
			}
		}



	NP.Write(iEndOfObject);
	NP.Write(ZFGP_ENDOFPACKET);
	m_pkNetWork->SendToClient(iClient, &NP);
}

void EntityManager::PackZoneListToClient(int iClient, set<int>& iZones)
{
		
	int iNetID;

	NetPacket NP;

	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	NP.Write((char) ZFGP_ZONELIST);

	for(set<int>::iterator itActiveZone = iZones.begin(); itActiveZone != iZones.end(); itActiveZone++ ) {
		int iZoneID = (*itActiveZone);
		iNetID = m_kZones[iZoneID].m_pkZone->iNetWorkID;

		NP.Write(iNetID);
		}

		
	NP.Write(-1);
	NP.Write(ZFGP_ENDOFPACKET);
	m_pkNetWork->SendToClient(iClient, &NP);
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
	while(iZoneID != -1) {
		kZones.push_back(iZoneID);
		pkNetPacket->Read(iZoneID);
		}
	
	for( i=0; i<m_pkZoneObject->m_akChilds.size(); i++) {
		int iLocalZoneID = m_pkZoneObject->m_akChilds[i]->iNetWorkID;
		
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
				GetStaticData(pkZone->iNetWorkID);
				printf("Need Static Ents for Zone - %d\n", pkZone->iNetWorkID);
				return;
				}
		}
}


void EntityManager::PackToClients()
{
	// If no clients we don't send anything.
	if(m_pkNetWork->GetNumOfClients() == 0)
		return;

	//Logf("net", " *** EntityManager::PackToClients() *** \n");


/*	if(m_pkZeroFps->GetEngineTime() < m_fEndTimeForceNet) {
		m_iForceNetUpdate = 0xFFFFFFFF;
		cout << "Forcing Object network updates" << endl;
		}
	else {
		m_iForceNetUpdate  = 0x0;					
		}*/

	vector<Entity*>	kObjects;
	m_iForceNetUpdate = 0xFFFFFFFF;
	NetPacket NP;
	
	// Keep it alive.
	int iNumOfObjects	= m_akObjects.size();
	int iPacketSize		= 0;
	int iEndOfObject	= -1;

	Entity* pkZone;
	int iClient;

	// Clear Active Zones for clients.
	for(iClient=0; iClient < m_pkZeroFps->m_kClient.size(); iClient++)
		m_pkZeroFps->m_kClient[iClient].m_iActiveZones.clear();
	
	// Refresh list of active Zones for each client.
	for(list<Entity*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) {
		P_Track* pkTrack = dynamic_cast<P_Track*>((*iT)->GetProperty("P_Track"));
		if(pkTrack->m_iConnectID != -1)
				m_pkZeroFps->m_kClient[pkTrack->m_iConnectID].m_iActiveZones.insert(
					pkTrack->m_iActiveZones.begin(),pkTrack->m_iActiveZones.end());
		}		


	// Client Network send.
	if(m_pkZeroFps->m_bClientMode && !m_pkZeroFps->m_bServerMode) {
		m_pkWorldObject->GetAllObjects(&kObjects);
		PackToClient(0, kObjects);
		return;
		}

	// Server Network send.
	for(iClient=0; iClient < m_pkZeroFps->m_kClient.size(); iClient++) {
		if(m_pkZeroFps->m_kClient[iClient].m_pkObject == NULL)	continue;

		// Get Ptr to clients tracker.
		//TrackProperty* pkTrack = dynamic_cast<TrackProperty*>(m_pkZeroFps->m_kClient[iClient].m_pkObject->GetProperty("TrackProperty"));

		// Pack And Sent Active Zones to client
		//PackZoneListToClient(iClient, pkTrack);
		PackZoneListToClient(iClient, m_pkZeroFps->m_kClient[iClient].m_iActiveZones);

		// Pack and Send all Client Objects
		kObjects.clear();
		m_pkClientObject->GetAllObjects(&kObjects);
		PackToClient(iClient, kObjects);

		//pack and send global objects
		kObjects.clear();
		m_pkGlobalObject->GetAllObjects(&kObjects);
		PackToClient(iClient, kObjects);

		// Loop all zones activated by client.
		for(set<int>::iterator itActiveZone = m_pkZeroFps->m_kClient[iClient].m_iActiveZones.begin(); itActiveZone != m_pkZeroFps->m_kClient[iClient].m_iActiveZones.end(); itActiveZone++ ) {
			// Get Zone and all subobjects.
			int iZoneID = (*itActiveZone);
			pkZone = m_kZones[iZoneID].m_pkZone;
			assert(pkZone);

			kObjects.clear();
			pkZone->GetAllDynamicEntitys(&kObjects);
			PackToClient(iClient, kObjects);
			}
		}

	for(list<Entity*>::iterator it = m_akObjects.begin(); it != m_akObjects.end(); it++) {
		(*it)->m_aiNetDeleteList.clear();
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


/*
void EntityManager::PackToClients()
{
	if(m_pkNetWork->GetNumOfClients() == 0)
		return;

	Logf("net", " *** EntityManager::PackToClients() *** \n");


	if(m_pkZeroFps->GetEngineTime() < m_fEndTimeForceNet) {
		m_iForceNetUpdate = 0xFFFFFFFF;
		cout << "Forcing Object network updates" << endl;
		}
	else {
		m_iForceNetUpdate  = 0x0;					
		}

	NetPacket NP;
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	NP.Write((char) ZFGP_OBJECTSTATE);

	int iNumOfObjects = m_akObjects.size();
	int iPacketSize = 0;
	int iEndOfObject = -1;

	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		//Logf("net", "Check Object [%d]\n",(*it)->iNetWorkID );

		(*it)->m_iNetUpdateFlags |= m_iForceNetUpdate;
		
		if((*it)->NeedToPack() == false)					continue;
		if((*it)->m_eRole != NETROLE_AUTHORITY)		continue;

		NP.Write((*it)->iNetWorkID);
		Logf("net", "Object [%d]\n",(*it)->iNetWorkID );
		(*it)->PackTo(&NP);
		iPacketSize++;

		Logf("net", " Size: %d\n\n",NP.m_iPos );

		if(NP.m_iPos >= 512) {
			NP.Write(iEndOfObject);
			NP.Write(ZFGP_ENDOFPACKET);
			m_pkNetWork->SendToAllClients(&NP);

			NP.Clear();
			NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
			NP.Write((char) ZFGP_OBJECTSTATE);

			iPacketSize = 0;
			}
	}

	NP.Write(iEndOfObject);
	NP.Write(ZFGP_ENDOFPACKET);
	m_pkNetWork->SendToAllClients(&NP);

	if(m_aiNetDeleteList.size() == 0)
		return;

	// Pack delete data.
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	NP.Write((char) ZFGP_DELETEOBJECT);

	cout << "Delete List Size:"  << m_aiNetDeleteList.size() << endl;

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

	m_aiNetDeleteList.clear();
}
*/

void EntityManager::StaticData(int iClient, NetPacket* pkNetPacket)
{
	int iEntityID;
	pkNetPacket->Read(&iEntityID,sizeof(iEntityID));
	
	Entity* pkEnt = GetObjectByNetWorkID( iEntityID );
	if(!pkEnt)
		return;

	Entity* pkStatic = pkEnt->GetStaticEntity();
	if(!pkStatic)
		return;

	vector<Entity*>	kObjects;
	m_iForceNetUpdate = 0xFFFFFFFF;

	kObjects.clear();
	pkStatic->GetAllObjects(&kObjects);
	PackToClient(iClient, kObjects);
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
	m_pkNetWork->SendToClient(0, &NP);
}

// Debug / Help Functions		
void EntityManager::DisplayTree()
{
	GetSystem().Log_Create("fisklins");
	m_pkWorldObject->PrintTree(0);
}


void EntityManager::DumpActiverPropertysToLog(char* szMsg)
{
	Logf("net", "%s : %d\n", szMsg, m_akPropertys.size() );

	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		Logf("net", "%s (%d)", (*it)->m_acName, (*it)->GetObject()->iNetWorkID );
		if((*it)->GetObject()->m_pkParent)
			Logf("net", " Parent Obj: %s\n", (*it)->GetObject()->m_pkParent->m_strName.c_str() );
	}
	
}

// Message System.
void EntityManager::SendMsg()
{

}

void EntityManager::RouteMessage(GameMessage& Msg)
{
	Entity*	pkObject = GetObjectByNetWorkID(Msg.m_ToObject);

	if(pkObject == NULL) {
		cout << "No Valid object for message" << endl;		
		return;
		}

	pkObject->AddGameMessage(Msg);
}


// Get Strings.
char* EntityManager::GetUpdateStatusName(int eStatus)
{
	char* pkName = "";

	switch(eStatus) {
		case UPDATE_NONE: 		pkName = "UPDATE_NONE";	break;
		case UPDATE_ALL: 		pkName = "UPDATE_ALL";	break;
		case UPDATE_STATIC: 	pkName = "UPDATE_STATIC";	break;
		case UPDATE_DYNAMIC: 	pkName = "UPDATE_DYNAMIC";	break;
//		case UPDATE_PLAYERS: 	pkName = "UPDATE_PLAYERS";	break;
//		case UPDATE_DECORATION: pkName = "UPDATE_DECORATION";	break;
//		case UPDATE_LIGHT: 		pkName = "UPDATE_LIGHT";	break;
		}

	return pkName;
}

char* EntityManager::GetObjectTypeName(int eType)
{
	char* pkName = "";

	switch(eType) {
		case OBJECT_TYPE_DYNAMIC: 		pkName = "OBJECT_TYPE_DYNAMIC";	break;
		case OBJECT_TYPE_STATIC: 		pkName = "OBJECT_TYPE_STATIC";	break;
/*		case OBJECT_TYPE_PLAYER: 		pkName = "OBJECT_TYPE_PLAYER";	break;
		case OBJECT_TYPE_STATDYN:		pkName = "OBJECT_TYPE_STATDYN";	break;
//		case OBJECT_TYPE_DECORATION: 	pkName = "OBJECT_TYPE_DECORATION";	break;*/
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

void EntityManager::GetPropertys(int iType,int iSide)
{
	m_akPropertys.clear();
	m_pkWorldObject->GetAllPropertys(&m_akPropertys,iType,iSide);
}

bool EntityManager::TestLine(vector<Entity*>* pkPPList,Vector3 kPos,Vector3 kVec)
{
	pkPPList->clear();

	for(list<Entity*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) 
	{	
		Vector3 c=(*it)->GetWorldPosV() - kPos;		
		kVec.Normalize();		
		Vector3 k=kVec.Proj(c);		
		float cdis=c.Length();
		float kdis=k.Length();
		float Distance = sqrt((cdis*cdis)-(kdis*kdis));
		
		
		float fRadius=(*it)->GetRadius();
		
		if(Distance < fRadius)
		{			
			pkPPList->push_back((*it));
		}		
	}
	
	if(pkPPList->size()==0)
		return false;
	
	return true;
}


void EntityManager::RunCommand(int cmdid, const CmdArgument* kCommand) 
{ 

	switch(cmdid) {
		case FID_LOGOHTREE:
			DisplayTree();
			break;

		case FID_LOGACTIVEPROPERTYS:
			DumpActiverPropertysToLog("Active propertys");
			break;

		case FID_SENDMESSAGE:
/*			gm.m_FromObject = -1;
			gm.m_ToObject	= atoi(kCommand->m_kSplitCommand[2].c_str());
			gm.m_Name		= kCommand->m_kSplitCommand[1].c_str();*/
//			m_pkConsole->Printf("Sending Msg '%s' to %d from %d", gm.m_Name.c_str(), gm.m_ToObject, gm.m_FromObject);
			//RouteMessage(gm);
			break;

		case FID_LOADZONES:
			LoadZones();
			break;

		case FID_SAVEZONE:
			SaveZones();
			break;
			
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
			LoadWorld(kCommand->m_kSplitCommand[1].c_str());
			break;
		
		case FID_SAVEWORLD:
			ForceSave();
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
	net->SendToAllClients(&NP);
	Logf("net", " Sending Own Request for %d\n", pkObj->iNetWorkID);
	
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

	NetWork* net = static_cast<NetWork*>(GetSystem().GetObjectPtr("NetWork"));
	net->SendToAllClients(&NP);

	OwnerShip_Give(pkObj);
	Logf("net", " Gives away ownership of %d\n", pkObj->iNetWorkID);

}

void EntityManager::OwnerShip_OnGrant(Entity* pkObj)
{
	if(pkObj == NULL)
		return;

	OwnerShip_Take(pkObj);
	Logf("net", " This node now own %d\n", pkObj->iNetWorkID);
}

Entity* EntityManager::CloneObject(int iNetID)
{
	Entity* pkObjOrginal = GetObjectByNetWorkID(iNetID);
	if(pkObjOrginal == NULL)
		return NULL;

	Entity* pkObjClone =	CreateObject();
	pkObjClone->MakeCloneOf(pkObjOrginal);
	return pkObjClone;
}

/*
void EntityManager::Test_CreateZones()
{
	

	m_kZones.clear();
	int y = 0;
	int iZonesSize = 10;
	int iZonesSide = 100;
	
	Vector3 kPos;
	Vector3 kRandOffset;

	MazeGen GaaMaze;
	GaaMaze.Load("./maze.bmp");


	for(int x=0; x<iZonesSide; x++) 
	{
		for(int z=0; z<iZonesSide; z++) 
		{
			if(GaaMaze.aaiMaze[x][z] == 1) 
			{
				
				kPos = Vector3(x*iZonesSize,y,z*iZonesSize);								
				CreateZone(kPos,Vector3(iZonesSize,iZonesSize,iZonesSize));//GetUnusedZoneID();
			}
		}
	}

//	AutoConnectZones();
//	int ispya = 2;
}*/

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

	Render* m_pkRender=static_cast<Render*>(GetSystem().GetObjectPtr("Render"));

	for(unsigned int i=0;i<m_kZones.size();i++) {
		if(!m_kZones[i].m_bUsed)
			continue;
			
		Vector3 kMin = m_kZones[i].m_kPos - m_kZones[i].m_kSize/2;
		Vector3 kMax = m_kZones[i].m_kPos + m_kZones[i].m_kSize/2;
	
		if(m_kZones[i].m_bUnderContruction)
			m_pkRender->DrawAABB( kMin,kMax, Vector3(0,0,1), 3 );

		if(m_kZones[i].m_bActive) {
			m_pkRender->DrawAABB( kMin,kMax, Vector3(1,0,0) );
			}
		else {
			m_pkRender->DrawAABB( kMin,kMax, Vector3(0,1,0) );
			}	

/*			if(m_kZones[i].m_bUnderContruction)
			m_pkRender->DrawAABB( kMin,kMax, Vector3(0,0,1) );
		else {
			}*/
	
		if(m_bDrawZoneConnections) {
			for(int j = 0 ;j< m_kZones[i].m_iZoneLinks.size();j++)
			{	
				glColor4f(0,0,1,0);
				m_pkRender->Line(m_kZones[i].m_kPos,m_kZones[m_kZones[i].m_iZoneLinks[j]].m_kPos);
			}
		}
	
	}
}

/*
void EntityManager::AutoConnectZones()
{
	Vector3 kCenterPos;
	Vector3 kCheckPos;
	ZoneData* pkZone;
	
	vector<Vector3>	kAutoConnectDirs;
	kAutoConnectDirs.push_back(Vector3(10,0,0));
	kAutoConnectDirs.push_back(Vector3(-10,0,0));
	kAutoConnectDirs.push_back(Vector3(0,10,0));
	kAutoConnectDirs.push_back(Vector3(0,-10,0));
	kAutoConnectDirs.push_back(Vector3(0,0,10));
	kAutoConnectDirs.push_back(Vector3(0,0,-10));

	// For each Zone.
	for(unsigned int i=0;i<m_kZones.size();i++) {
		kCenterPos = m_kZones[i].m_kPos;

		// For each possible zone around this one.
		for(unsigned int iDir = 0; iDir < kAutoConnectDirs.size(); iDir++) {
			kCheckPos = kCenterPos + kAutoConnectDirs[iDir];
			pkZone = GetZone(kCheckPos);
			// If a zone add a link.
			if(pkZone && (i != pkZone->m_iZoneID)) {
				m_kZones[i].m_iZoneLinks.push_back(pkZone->m_iZoneID);
			}
		}
	}
}*/

Vector3 EntityManager::GetZoneCenter(int iZoneNum)
{
	return m_kZones[iZoneNum].m_kPos;
}

int EntityManager::GetNumOfZones()
{
	return m_kZones.size();
}

list<Entity*>* EntityManager::GetTrackerList()
{
	return &m_kTrackedObjects;
}

void EntityManager::AddTracker(Entity* kObject)
{
	cout << "Now tracking " << kObject->iNetWorkID << endl;
	m_kTrackedObjects.push_back(kObject);
}

void EntityManager::RemoveTracker(Entity* kObject)
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
		if(!m_kZones[iZ].m_bUsed)
			continue;

		if(m_kZones[iZ].IsInside(kPos))
			return &m_kZones[iZ];
		}

	return NULL;
}


ZoneData* EntityManager::GetZone(Entity* PkObject)
{
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
		if(!m_kZones[iZ].m_bUsed)
			continue;
		
		if(m_kZones[iZ].IsInside(PkObject->GetWorldPosV()))
			return &m_kZones[iZ];
		}

	return NULL;
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
		//first check current zone
		if(m_kZones[iCurrentZone].IsInside( kMyPos ))
			return iCurrentZone;
	
	
		//check zones connected to the last visited zone
		ZoneData* pkZone = GetZoneData(iCurrentZone);
		for(int i = 0;i < pkZone->m_iZoneLinks.size();i++)
		{
			if(m_kZones[pkZone->m_iZoneLinks[i]].IsInside(kMyPos))
			{	
				//cout<<"moved to nearby zone"<<endl;
				return pkZone->m_iZoneLinks[i];						
			}
		}
	}

	//seccond go trough all zones in the world
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
		if(!m_kZones[iZ].m_bUsed)
			continue;
		
		if(m_kZones[iZ].IsInside(kMyPos))
		{
			return iZ;
		}
	}

	//last take the closest zone in the world
	if(bClosestZone)
	{
		float d = 99999999999;
		int id = -1;
	
		for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
			if(!m_kZones[iZ].m_bUsed)
				continue;
			
			float dis = (m_kZones[iZ].m_kPos - kMyPos).Length();
		
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


void EntityManager::UpdateZones()
{
	float fTime = m_pkZeroFps->GetGameTime();
	ZoneData* pkZone;
	ZoneData* pkStartZone;
	unsigned int iZ;


	//int iTrackerLOS = 3;

	// Set All Zones as inactive.
	for(iZ=0;iZ<m_kZones.size();iZ++) 
	{
		m_kZones[iZ].m_bActive							= false;
		m_kZones[iZ].m_fInactiveTime					= fTime;
		m_kZones[iZ].m_iRange							= 1000;
		if(m_kZones[iZ].m_pkZone)
			m_kZones[iZ].m_pkZone->GetUpdateStatus()	= UPDATE_NONE;
	}

	vector<ZoneData*>	m_kFloodZones;
	int iZoneIndex;

	//cout<<"nr of trackers:"<<m_kTrackedObjects.size()<<endl;
	// For each tracker.
	for(list<Entity*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) 
	{
		// Find Active Zone.
		P_Track* pkTrack = dynamic_cast<P_Track*>((*iT)->GetProperty("P_Track"));
		pkTrack->m_iActiveZones.clear();

		for(iZ=0;iZ<m_kZones.size();iZ++)
			m_kZones[iZ].m_iRange							= 1000;
		
		//get current zone
		iZoneIndex = GetZoneIndex((*iT),(*iT)->m_iCurrentZone,pkTrack->m_bClosestZone);
		
		if(iZoneIndex >= 0) {
			pkStartZone = &m_kZones[iZoneIndex];
			pkStartZone->m_iRange = 0;
			m_kFloodZones.push_back(pkStartZone);
		}

		// Flood Zones in rage to active.
		while(m_kFloodZones.size()) 
		{
			pkZone = m_kFloodZones.back();
			m_kFloodZones.pop_back();

			pkTrack->m_iActiveZones.insert(pkZone->m_iZoneID);

			pkZone->m_bActive = true;
			int iRange = pkZone->m_iRange + 1;

			if(iRange < m_iTrackerLOS) 
			{
				for(unsigned int i=0; i<pkZone->m_iZoneLinks.size(); i++) 
				{
					ZoneData* pkOtherZone = GetZoneData(pkZone->m_iZoneLinks[i]); //				pkZone->m_pkZoneLinks[i];	//GetZoneData(pkZone->m_iZoneLinks[i]);				

					if(pkOtherZone->m_iRange < iRange)	continue;
					pkOtherZone->m_iRange = iRange;
					if(pkOtherZone->m_iRange < m_iTrackerLOS)
					{
						m_kFloodZones.push_back(pkOtherZone);
					}
				}				
			}
		}
	}


	//loop trough all zones and load/unload them
	ZoneData* pkZoneRefresh;	
	for(unsigned int i=0; i<m_kZones.size(); i++) 
	{
		pkZoneRefresh = &m_kZones[i];

		// Zones that need to load.
		if(pkZoneRefresh->m_bActive && pkZoneRefresh->m_pkZone == NULL) 
		{
			LoadZone(pkZoneRefresh->m_iZoneID);
			//cout << "Load Zone: " << pkZoneRefresh->m_iZoneID << endl;
		}

		// Zones that need to unload
		if(pkZoneRefresh->m_bActive == false && pkZoneRefresh->m_pkZone) 
		{
			UnLoadZone(pkZoneRefresh->m_iZoneID);
			//cout << "UnLoad Zone: " << pkZoneRefresh->m_iZoneID << endl;
		}

		if(pkZoneRefresh->m_bActive)
			pkZoneRefresh->m_pkZone->GetUpdateStatus()		= UPDATE_ALL;
	}
}

void EntityManager::Zones_Refresh()
{
	for(unsigned int i=0; i<m_kZones.size(); i++) {
		// Zones that need to load.
		if(m_kZones[i].m_bActive && m_kZones[i].m_pkZone == NULL) {
			LoadZone(m_kZones[i].m_iZoneID);
			cout << "Load Zone: " << m_kZones[i].m_iZoneID << endl;
			}

		// Zones that need to unload
		if(m_kZones[i].m_bActive == false && m_kZones[i].m_pkZone) {
			UnLoadZone(m_kZones[i].m_iZoneID);
			cout << "UnLoad Zone: " << m_kZones[i].m_iZoneID << endl;
			}

	}
}


vector<int>	EntityManager::GetActiveZoneIDs(int iTracker)
{
	vector<int>	Nisse;
	return Nisse;
}

int EntityManager::CreateZone()
{
	return CreateZone(Vector3(0,0,0),Vector3(8,8,8));
}

int EntityManager::CreateZone(Vector3 kPos,Vector3 kSize)
{
	
	int id = GetUnusedZoneID();

	m_kZones[id].m_bNew = true;
	m_kZones[id].m_bUsed = true;
	m_kZones[id].m_bActive = false;	
	m_kZones[id].m_iZoneID = id;
	m_kZones[id].m_pkZone = NULL;
	m_kZones[id].m_kSize = kSize;
	m_kZones[id].m_kPos = kPos;
	m_kZones[id].m_iZoneLinks.clear();
	m_kZones[id].m_fInactiveTime = 0;
	m_kZones[id].m_iRange = 0;
	m_kZones[id].m_bUnderContruction = false;
	m_kZones[id].m_iRevision = 0;
	m_kZones[id].m_strEnviroment = "Default";
	
	UpdateZoneLinks(id);
	
	return id;
}


void EntityManager::DeleteZone(int iId)
{
	if(iId >= m_kZones.size())
	{
		//cout<<"that zone is invalid"<<endl;
		return;
	}

	m_kZones[iId].m_bUsed = false;
	m_kZones[iId].m_bActive = false;
	
	ClearZoneLinks(iId);	
	
	//unload the zone
	UnLoadZone(iId);
	
}



bool EntityManager::LoadZones()
{
	ZFVFile kFile;
	if(!kFile.Open((m_kWorldDirectory + "/zones.dat").c_str(),0,false))
	{
		cout<<"Error loading zones"<<endl;
		return false;
	}

	Clear();

	int iNumOfZone;
	kFile.Read(&iNumOfZone,sizeof(int),1);

   // load latest created entityID
   kFile.Read(&iNextObjectID,sizeof(int),1);

	cout<<"Nr of zones:"<<iNumOfZone<<endl;
	ZoneData kZData;
	kZData.m_bUsed = true;
	
	int i,zl;
	int iLink;

	for( i=0; i<iNumOfZone; i++) {
		kFile.Read(&kZData.m_bNew, sizeof(kZData.m_bNew), 1);
		kFile.Read(&kZData.m_bUnderContruction, sizeof(kZData.m_bUnderContruction), 1);						
		kFile.Read(&kZData.m_iRevision, sizeof(kZData.m_iRevision), 1);								
		kFile.Read(&kZData.m_bUsed, sizeof(kZData.m_bUsed), 1);						
		kFile.Read(&kZData.m_iZoneID, sizeof(kZData.m_iZoneID), 1);
		kFile.Read(&kZData.m_kSize, sizeof(kZData.m_kSize), 1);
		kFile.Read(&kZData.m_kPos, sizeof(kZData.m_kPos), 1);

		char temp[128];
		kFile.Read(temp, 128, 1);
		kZData.m_strEnviroment = temp;


		int iNumOfLinks;	
		kFile.Read(&iNumOfLinks, sizeof(iNumOfLinks), 1);

		for(zl=0; zl < iNumOfLinks; zl++) {
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

bool EntityManager::SaveZones()
{
	
	string filename(m_kWorldDirectory + "/zones.dat");
	
	cout<<"saving to :"<<filename<<endl;
	
	ZFVFile kFile;
	if(!kFile.Open(filename.c_str(),0,true))
	{	
		cout<<"Could not open zone save file"<<endl;
		return false;
	}
	
 	int iNumOfZone = m_kZones.size();
	kFile.Write(&iNumOfZone,sizeof(int),1);
   
   // save latest created entityID
   kFile.Write(&iNextObjectID,sizeof(int),1);
	
	for(int i=0; i<iNumOfZone; i++) 
	{

		kFile.Write(&m_kZones[i].m_bNew, sizeof(m_kZones[i].m_bNew), 1);
		kFile.Write(&m_kZones[i].m_bUnderContruction, sizeof(m_kZones[i].m_bUnderContruction), 1);								
		kFile.Write(&m_kZones[i].m_iRevision, sizeof(m_kZones[i].m_iRevision), 1);										
		kFile.Write(&m_kZones[i].m_bUsed, sizeof(m_kZones[i].m_bUsed), 1);				
		kFile.Write(&m_kZones[i].m_iZoneID, sizeof(m_kZones[i].m_iZoneID), 1);
		kFile.Write(&m_kZones[i].m_kSize, sizeof(m_kZones[i].m_kSize), 1);		
		kFile.Write(&m_kZones[i].m_kPos, sizeof(m_kZones[i].m_kPos), 1);

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

ZoneData* EntityManager::GetZoneData(int iID)
{
	if(iID < 0 || iID >= m_kZones.size())
		return NULL;

	return &m_kZones[iID];

}

void EntityManager::LoadZone(int iId)
{	
	ZoneData* kZData = GetZoneData(iId);
	assert(kZData);

	// Check if zone is loaded. If soo return.
	if(kZData->m_pkZone)
		return;

	// Create Object.
	Entity* object = new Entity;//ZoneObject;
	object->m_bZone = true;
	kZData->m_pkZone = object;
	kZData->m_pkZone->SetParent(GetZoneObject());	
	kZData->m_pkZone->GetName() = "ZoneObject";	
	
	
	//load
	char nr[10];
	IntToChar(nr,iId);
	
	string filename(m_kWorldDirectory);
	filename+="/";
	filename+=nr;
	filename+=".dynamic.zone";
	
	//cout<<"load from :"<<filename<<endl;
	
	ZFVFile kFile;
	
	if(!kZData->m_bNew)
		if(!kFile.Open(filename.c_str(),0,false))
			kZData->m_bNew = true;
	
	
	if(kZData->m_bNew)
	{	
		kZData->m_bNew = false;
		
		//cout<<"error loading zone, creating a new template zone"<<endl;
		
		Vector3 kPos = kZData->m_kPos;
		object->SetLocalPosV(kPos);
		//object->GetUpdateStatus()=UPDATE_DYNAMIC;
		object->AddProperty("P_LightUpdate");	//always attach a lightupdateproperty to new zones

//		object->AddProperty("P_PfMesh");
		SetZoneModel("data/mad/zones/emptyzone.mad",iId);

		//add static entity
		Entity* staticentity = new Entity;
		staticentity->GetName() = "StaticEntity";
		staticentity->SetParent(kZData->m_pkZone);
		staticentity->GetObjectType() = OBJECT_TYPE_STATIC;

		return;
	}
 
	kZData->m_pkZone->Load(&kFile);
	kFile.Close();
	
}

void EntityManager::UnLoadZone(int iId)
{
	ZoneData* kZData = GetZoneData(iId);
	assert(kZData);
	if(kZData->m_pkZone == NULL)
		return;	
	
	SaveZone(iId);
	
	Delete(kZData->m_pkZone);
	kZData->m_pkZone = NULL;
}

void EntityManager::SaveZone(int iId)
{
	ZoneData* kZData = GetZoneData(iId);
	assert(kZData);
	if(kZData->m_pkZone == NULL)
		return;

	char nr[10];
	IntToChar(nr,iId);
	
	string filename(m_kWorldDirectory);
	filename+="/";
	filename+=nr;
	filename+=".dynamic.zone";

	
	//cout<<"saving to :"<<filename<<endl;
	
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
	for(int i=0;i<m_kZones.size();i++)
	{
		if(!m_kZones[i].m_bUsed)
		{	
			return i;
		}
	}

	//if none can be found create a new one
	
	ZoneData newzone;

	newzone.m_iZoneID = m_kZones.size() - 1;
	
	m_kZones.push_back(newzone);
	
	return m_kZones.size() - 1;
}


bool EntityManager::LoadWorld(const char* acDir)
{
	SetWorldDir(acDir);
	
	//clear the world
	Clear();

	//load zones in acDir
	return LoadZones();
}

void EntityManager::ClearZoneLinks(int iId)
{
	//loop trough all connected zones
	for(int i = 0;i < m_kZones[iId].m_iZoneLinks.size();i++)
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
		if(!m_kZones[i].m_bUsed)
			continue;

		if(BoxVSBox(kPos,kSize-0.1,m_kZones[i].m_kPos,m_kZones[i].m_kSize))
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
		if(!m_kZones[i].m_bUsed)
			continue;
	
		if(i == iId)
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
	float x1 = (kSize1.x/2.0);
	float y1 = (kSize1.y/2.0);
	float z1 = (kSize1.z/2.0);
	
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
	float x2 = (kSize2.x/2.0);
	float y2 = (kSize2.y/2.0);
	float z2 = (kSize2.z/2.0);
	
	vector<Vector3>	kTestDirs2;
	kTestDirs2.push_back(Vector3(-x2,y2,z2));
	kTestDirs2.push_back(Vector3(x2,y2,z2));
	kTestDirs2.push_back(Vector3(x2,-y2,z2));
	kTestDirs2.push_back(Vector3(-x2,-y2,z2));
	
	kTestDirs2.push_back(Vector3(-x2,y2,-z2));
	kTestDirs2.push_back(Vector3(x2,y2,-z2));
	kTestDirs2.push_back(Vector3(x2,-y2,-z2));
	kTestDirs2.push_back(Vector3(-x2,-y2,-z2));

	int i;
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

	//setup mad
	P_Mad* mp = (P_Mad*)zd->m_pkZone->GetProperty("P_Mad");
	
	if(!mp)
		mp = (P_Mad*)zd->m_pkZone->AddProperty("P_Mad");
		
	if(mp)
		mp->SetBase(szName);

	//setup pathfinding
	P_PfMesh* pkMesh = (P_PfMesh*)zd->m_pkZone->GetProperty("P_PfMesh");
	
	if(!pkMesh)
		pkMesh = (P_PfMesh*)zd->m_pkZone->AddProperty("P_PfMesh");
	
	if(pkMesh) {
		pkMesh->SetMad(mp);
	}



/*	//setup physics
	P_Physic* pp = (P_Physic*)zd->m_pkZone->GetProperty("P_Physic");
	
	if(!pp)
		pp = (P_Physic*)zd->m_pkZone->AddProperty("P_Physic");	
	
	if(pp)
	{
		mp->SetBase(szName);
		P_PfMesh* pkMesh = (P_PfMesh*)zd->m_pkZone->GetProperty("P_PfMesh");
		if(pkMesh) {
			pkMesh->SetMad(mp);
			}
	}
*/	
}



void EntityManager::ForceUnload()
{	

	//loop trough all loaded zones, and unload em , to make sure that all zones is saved
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(!m_kZones[i].m_bUsed)
			continue;
	
		if(m_kZones[i].m_pkZone)
			UnLoadZone(i);		
	}
	
	UpdateDelete();
	
	//after unloading , reload 	
	UpdateZones();
}

void EntityManager::ForceSave()
{
	//loop trough all loaded zones, and unload em , to make sure that all zones is saved
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(!m_kZones[i].m_bUsed)
			continue;
	
		if(m_kZones[i].m_pkZone)
			SaveZone(i);		
	}	
}

void EntityManager::SetUnderConstruction(int iId)
{
	int i;

	ZoneData* zd = GetZoneData(iId);	
	if(zd)
	{
		if(zd->m_bUnderContruction)
		{
			//cout<<"Zone is already under construction"<<endl;
			return;
		}
		
		//make sure zone is loaded
		if(!zd->m_pkZone)
		{
			cout<<"cant construct unloaded zone"<<endl;
			return;
		}
		
		
		//reattach static entitys to zoneobject
		
		//first find the staticentity
		Entity* pkStaticEntity=NULL;		
		vector<Entity*> kEntitys;				
		zd->m_pkZone->GetAllObjects(&kEntitys);
		for(i=0;i<kEntitys.size();i++)
		{
			if(kEntitys[i]->GetName()=="StaticEntity")
			{
				pkStaticEntity=kEntitys[i]; 
				break;
			}		
		}
		
		//if no staticentity was found, complain
		if(!pkStaticEntity)
		{
			cout<<"Error zone has no staticentity"<<endl;
			return;
		}
		
		//find all static objects and attach them to zoneobject
		int nrofstatic=0;
		for(i=0;i<kEntitys.size();i++)
			if(kEntitys[i]->m_iObjectType == OBJECT_TYPE_STATIC)
			{
				kEntitys[i]->SetParent(zd->m_pkZone);
				nrofstatic++;
			}
		
		//update zonedata		
		zd->m_bUnderContruction = true;
		cout<<"Setting zone:"<<iId<<" in construction mode revision:"<<zd->m_iRevision<< " static entitys:"<<nrofstatic<<" dynamic entitys:"<<kEntitys.size()-nrofstatic<<endl;
		
	}
}

void EntityManager::CommitZone(int iId)
{
	int i;

	ZoneData* zd = GetZoneData(iId);	
	if(zd)
	{
		if(!zd->m_bUnderContruction)
		{
			//cout<<"Zone is not underconstruction"<<endl;
			return;
		}
		
		//make sure zone is loaded		
		if(!zd->m_pkZone)
		{
			cout<<"cant commit unloaded zone"<<endl;
			return;
		}
	
		
		//reattach static entitys to staticentity entity		
		
		
		//first find the staticentity
		Entity* pkStaticEntity=NULL;		
		vector<Entity*> kEntitys;				
		zd->m_pkZone->GetAllObjects(&kEntitys);
		for(i=0;i<kEntitys.size();i++)
		{
			if(kEntitys[i]->GetName()=="StaticEntity")
			{
				pkStaticEntity=kEntitys[i]; 
				break;
			}		
		}
		
		//if no staticentity was found, complain
		if(!pkStaticEntity)
		{
			cout<<"Error zone has no staticentity"<<endl;
			return;
		}
		
		
		//find all static objects and attach them to staticentity
		int nrofstatic=0;		
		for(i=0;i<kEntitys.size();i++)
			if(kEntitys[i]->m_iObjectType == OBJECT_TYPE_STATIC)
			{	
				kEntitys[i]->SetParent(pkStaticEntity);
				nrofstatic++;
			}
	
	
		//update zone data
		zd->m_bUnderContruction = false;
		zd->m_iRevision++;		
		cout<<"committing zone:"<<iId<<" new revision is:"<<zd->m_iRevision<<" static entitys:"<<nrofstatic<<" dynamic entitys:"<<kEntitys.size()-nrofstatic<<endl;
	}
}
