#include "entitymanager.h"
#include "network.h"
#include "zerofps.h"
#include "../basic/zfsystem.h"
#include "../basic/simplescript.h"
#include "../engine_systems/propertys/p_primitives3d.h"
#include "../engine_systems/propertys/p_track.h"
#include "../engine_systems/propertys/p_mad.h"
#include "../engine_systems/propertys/p_tcs.h"
//#include "../engine_systems/propertys/p_physic.h"
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

	iNextObjectID				= 0;
	m_iTotalNetObjectData	= 0;
	m_iNumOfNetObjects		= 0;
	m_bDrawZones				= false;
	m_bDrawZoneConnections	= false;
	m_iTrackerLOS				= 3;	
	m_iObjectDistance			= 50;
	m_fZoneUnloadTime			= 3;	
	m_iMaxZoneIO 				= 1;	

	m_pkWorldObject			= NULL;
	m_pkZoneObject				= NULL;
	m_pkClientObject			= NULL;
	m_pkGlobalObject			= NULL;

	m_fSimTime					= 0;			
	m_fSimTimeScale			= 1.0;	

	Register_Cmd("o_logtree",FID_LOGOHTREE);	
	Register_Cmd("o_dumpp",FID_LOGACTIVEPROPERTYS);	
	Register_Cmd("sendmsg",FID_SENDMESSAGE, CSYS_FLAG_SRC_ALL, "sendmsg name id",2);	

	Register_Cmd("newworld",FID_NEWWORLD, CSYS_FLAG_SRC_ALL);	
	Register_Cmd("loadworld",FID_LOADWORLD, CSYS_FLAG_SRC_ALL);	
	Register_Cmd("saveworld",FID_SAVEWORLD, CSYS_FLAG_SRC_ALL);		
	Register_Cmd("setworlddir",FID_SETWORLDDIR, CSYS_FLAG_SRC_ALL);		
	

	RegisterVariable("l_showzones",	&m_bDrawZones,					CSYS_BOOL);
	RegisterVariable("l_showconn",	&m_bDrawZoneConnections,	CSYS_BOOL);
	RegisterVariable("e_simspeed",	&m_fSimTimeScale,				CSYS_FLOAT);
	
	RegisterVariable("l_trackerlos", &m_iTrackerLOS, CSYS_INT);	
	RegisterVariable("l_zoneunloadtime", &m_fZoneUnloadTime, CSYS_FLOAT);	
	RegisterVariable("l_maxzoneio", &m_iMaxZoneIO, CSYS_INT);	
	RegisterVariable("l_objectdistance", &m_iObjectDistance, CSYS_FLOAT);		
}

bool EntityManager::StartUp()	
{
	m_pkZeroFps	=	static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));		
	m_pkNetWork	= static_cast<NetWork*>(GetSystem().GetObjectPtr("NetWork"));
	m_pkScript  = static_cast<ZFScriptSystem*>(GetSystem().GetObjectPtr("ZFScriptSystem"));
	m_pkBasicFS	=	static_cast<ZFBasicFS*>(GetSystem().GetObjectPtr("ZFBasicFS"));		
	m_pkZShaderSystem = static_cast<ZShaderSystem*>(GetSystem().GetObjectPtr("ZShaderSystem"));
	m_pkRender = static_cast<Render*>(GetSystem().GetObjectPtr("Render"));
	
	m_fEndTimeForceNet		= m_pkZeroFps->GetEngineTime();

	m_kWorldDirectory = "worldtemp";

	//create all base objects
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

	if(m_iNumOfNetObjects) {
		fAvgObjSize = float(m_iTotalNetObjectData / m_iNumOfNetObjects);
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
	vector<Entity*> kObjects;		
	GetAllObjects(&kObjects);
	
	for(vector<Entity*>::iterator it=kObjects.begin();it!=kObjects.end();it++) 
	{
		if((*it)->GetType() == czType)
		{
			return (*it);
		}
	}		
	return NULL;
}


/**	\brief	Link this to the Object manager

  This function is called by objects as they are created. It assigned a NetWorkID to the object and
  also put them in the ObjectManger.
*/
void EntityManager::Link(Entity* pkObject,int iId) 
{
	if(pkObject->m_iEntityID != -1)
	{
		if(IsLinked(pkObject))
		{
			cout<<"WARNING: Object is already linked"<<endl;
			return;	
		}
	}

	if(iId == -1)
	{
		pkObject->m_iEntityID = iNextObjectID++;
	}
	else
	{
		if(GetObjectByNetWorkID(iId))
		{
			cout<<"WARNING: "<<GetNumOfObjects()<<" Entity whit id:"<<iId<<" already exist"<<" setting new id "<<iNextObjectID<<endl;
			pkObject->m_iEntityID = iNextObjectID++;			
		}
		else	
			pkObject->m_iEntityID = iId;
	}
		
	//m_akObjects.push_back(pkObject);
	m_akEntitys[pkObject->m_iEntityID] = pkObject; 
}

bool EntityManager::IsLinked(Entity* pkObject)
{
	if(m_akEntitys.find(pkObject->m_iEntityID) == m_akEntitys.end())
		return false;
	else
		return true;

/*	for(list<Entity*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++)
		if((*it) == pkObject)
			return true;
		
	return false;*/
}

/**	\brief	UnLink this from Object Manger.

  Remvoves object from the Object Manger.
*/
void EntityManager::UnLink(Entity* pkObject) 
{	
	// If i own object mark so we remove it on clients.
	//	if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
	//		m_aiNetDeleteList.push_back(pkObject->m_iEntityID);
	//m_akObjects.remove(pkObject);
	m_akEntitys.erase(pkObject->m_iEntityID);
}

/**	\brief	Delete all objects.

	Clear all data from ObjectManger.
*/
void EntityManager::Clear()
{

	//delete all objects in world
	while(m_akEntitys.begin() != m_akEntitys.end())
		delete((*m_akEntitys.begin()).second);
	
	//m_akEntitys.clear();
	
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
	m_pkWorldObject						=	CreateObject();	
	m_pkWorldObject->SetName("WorldObject");
	m_pkWorldObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkWorldObject->m_eRemoteRole	= NETROLE_NONE;

	//object that is parent to all zones
	m_pkZoneObject							=	CreateObject();	
	m_pkZoneObject->SetParent(m_pkWorldObject);
	m_pkZoneObject->SetName("ZoneObject");
	m_pkZoneObject->m_eRole				= NETROLE_AUTHORITY;
	m_pkZoneObject->m_eRemoteRole		= NETROLE_NONE;

	//object that is parent to all client objects
	m_pkClientObject						=	CreateObject();	
	m_pkClientObject->SetParent(m_pkWorldObject);
	m_pkClientObject->SetName("ClientObject");
	m_pkClientObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkClientObject->m_eRemoteRole	= NETROLE_NONE;

	//object that is parent to all global objects (server information etc)
	m_pkGlobalObject						=	CreateObject();	
	m_pkGlobalObject->SetParent(m_pkWorldObject);
	m_pkGlobalObject->SetName("GlobalObject");
	m_pkGlobalObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkGlobalObject->m_eRemoteRole	= NETROLE_NONE;

	iNextObjectID = 100000;
}



/**	\brief	Creates a new clean object.

	Creates a basic object without any propertys and all values set to defualt. 
*/
Entity* EntityManager::CreateObject(bool bLink)
{
	Entity* pkObj = new Entity;
	
	if(bLink)
		Link(pkObj);
	
	return pkObj;
}

/**	\brief	Adds an object to delete qeue
*/
void EntityManager::Delete(int iNetworkID) 
{
   Entity* pkE = GetObjectByNetWorkID(iNetworkID);

   if ( pkE )
      Delete (pkE);
}


void EntityManager::Delete(Entity* pkObject) 
{
	if(pkObject == NULL)
		return;

	for(vector<int>::iterator it=m_aiDeleteList.begin();it!=m_aiDeleteList.end();it++) 
	{
		if(pkObject->m_iEntityID == (*it)) {
			Logf("net", "Object [%d] already in delete list\n", pkObject->m_iEntityID);
			//cout << "Object already in delete list" << endl;
			return;
		}
	}
	
	m_aiDeleteList.push_back(pkObject->m_iEntityID);
}
 
/**	\brief	Adds an object to delete qeue

	Walk DeleteList and delete all objects in it.
*/
void EntityManager::UpdateDelete()
{
//	int iSize = m_aiDeleteList.size();

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
			m_pkWorldObject->GetPropertys(&m_akPropertys,iType,iSide);
	}
	else
	{	
		if(pkRootEntity)
			pkRootEntity->GetAllPropertys(&m_akPropertys,iType,iSide);
		else
			m_pkWorldObject->GetAllPropertys(&m_akPropertys,iType,iSide);
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


/**	\brief	Creates a new clean object.

	Creates a basic object without any propertys and all values set to defualt. 
*/

Entity* EntityManager::CreateObjectByNetWorkID(int iNetID)
{
//	Object *pkNew = new NetSlaveObject;
	Entity *pkNew = CreateObject(false);

	//	Add(pkNew);
//	pkNew->m_iEntityID = iNetID;
	Link(pkNew,iNetID);
	
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
	
	if(m_kZones[id].m_iStatus != EZS_LOADED)
		return NULL;
	
	Entity* newobj = CreateObjectFromScript(acName);
	
	if(newobj)
	{      
		newobj->SetUseZones(true);
		newobj->SetWorldPosV(kPos);	
		if(newobj->m_iCurrentZone == -1)
			cout<<"Error! Tried to create a object outside zones."<<endl;
	}

	
	return newobj;
}

Entity* EntityManager::CreateObjectFromScript(const char* acName)
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
void EntityManager::GetAllObjects(vector<Entity*> *pakObjects)
{
	//m_pkWorldObject->GetAllObjects(pakObjects);
	m_pkWorldObject->GetAllEntitys(pakObjects,true);	
}

void EntityManager::GetAllObjectsInArea(vector<Entity*> *pkEntitys,Vector3 kPos,float fRadius)
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

//	cout<<"requested object:"<<iNetID<<endl;

	map<int,Entity*>::iterator it = m_akEntitys.find(iNetID);
	
	if(it != m_akEntitys.end())
		return (*it).second;
	else
		return NULL;

/*	for(list<Entity*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		if((*it)->m_iEntityID == iNetID)
		{	
			return (*it);
		}
	}


	return NULL;*/
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
		pkNetSlave = GetObjectByNetWorkID(iObjectID);
		
		//if not try to create it
		if(!pkNetSlave) 
		{
			pkNetSlave = CreateObjectByNetWorkID(iObjectID);
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
	m_OutNP.TargetSetClient(0);
		
	m_OutNP.Write((char) ZFGP_DELETEOBJECT);
	m_OutNP.Write(iEntityID);
	
	m_OutNP.Write(ZFGP_ENDOFPACKET);
	m_pkNetWork->Send2(&m_OutNP);


	//if this entity is going to be deleted on the client only, we want ot resets its flags 
	if(Entity* pkEnt = GetObjectByNetWorkID(iEntityID))
	{
		pkEnt->ResetAllNetUpdateFlagsAndChilds(iClient);
	
	}
}

void EntityManager::PackToClient(int iClient, vector<Entity*> kObjects,bool bZoneObject)
{
	int iPacketSize = 	0;
	int iEndOfObject = 	-1;
	int iSentSize = 		0;
	int iMaxPacketSize = 800;
	unsigned int iObj = 	0;	
	int iMaxSendSize = 	m_pkNetWork->GetMaxSendSize();
	Entity* pkPackObj;


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
			m_OutNP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
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
	for( i=0; i<m_pkZoneObject->m_akChilds.size(); i++) 
	{
		int iLocalZoneID = m_pkZoneObject->m_akChilds[i]->m_iEntityID;
		
		if(IsInsideVector(iLocalZoneID, kZones)) 
		{
			Delete(m_pkZoneObject->m_akChilds[i]);
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
//	NetPacket NP;
	
	// Keep it alive.
//	int iNumOfObjects	= (int) m_akEntitys.size();
//	int iPacketSize		= 0;
//	int iEndOfObject	= -1;

	unsigned int iClient;

	// Clear Active Zones for clients.
	for(iClient=0; iClient < m_pkZeroFps->m_kClient.size(); iClient++)
	{
		m_pkZeroFps->m_kClient[iClient].m_iActiveZones.clear();
		m_pkZeroFps->m_kClient[iClient].m_iUnloadZones.clear();		
	}
	
	// Refresh list of active Zones for each client.
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
		m_pkWorldObject->GetAllEntitys(&kObjects, true);

		m_OutNP.Clear();
		m_OutNP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
		m_OutNP.TargetSetClient(0);
		
		PackToClient(0, kObjects,false);
		m_OutNP.Write(ZFGP_ENDOFPACKET);
		m_pkNetWork->Send2(&m_OutNP);
		return;
	}

	// Server Network send.
	for(iClient=0; iClient < m_pkZeroFps->m_kClient.size(); iClient++) 
	{
		if(m_pkZeroFps->m_kClient[iClient].m_pkObject == NULL)	continue;
		m_OutNP.Clear();
		m_OutNP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
		m_OutNP.TargetSetClient(iClient);

		// Pack And Send unload list to client
		PackZoneListToClient(iClient, m_pkZeroFps->m_kClient[iClient].m_iUnloadZones);		
//		PackZoneListToClient(iClient, m_pkZeroFps->m_kClient[iClient].m_iActiveZones);

		// Loop and clear send data flag for those zone to this client
		for(set<int>::iterator itActiveZone = m_pkZeroFps->m_kClient[iClient].m_iUnloadZones.begin(); itActiveZone != m_pkZeroFps->m_kClient[iClient].m_iUnloadZones.end(); itActiveZone++ ) 
		{
			int iZoneID = (*itActiveZone);
			m_kZones[iZoneID].m_pkZone->ResetAllNetUpdateFlagsAndChilds( iClient );
		}

		//send all tracked object first =)
		kObjects.clear();	
		for(list<P_Track*>::iterator it = m_kTrackedObjects.begin(); it != m_kTrackedObjects.end(); it++ ) 
		{
			if((*it)->m_iConnectID == (int) iClient)
				(*it)->GetObject()->GetAllEntitys(&kObjects, true);
		}		
		PackToClient(iClient, kObjects,false);
		
		//collect objects
		kObjects.clear();		
		
		// Pack and Send all Client Objects
		m_pkClientObject->GetAllEntitys(&kObjects, true);

		//pack and send global objects
		m_pkGlobalObject->GetAllEntitys(&kObjects, true);

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
		PackToClient(iClient, kObjects,true);			//send in true to packtoclient 
		
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
	
	Entity* pkEnt = GetObjectByNetWorkID( iEntityID );
	if(!pkEnt)
		return;

	Entity* pkStatic = NULL;//pkEnt->GetStaticEntity();
	if(!pkStatic)
		return;

	vector<Entity*>	kObjects;
	m_iForceNetUpdate = 0xFFFFFFFF;

	kObjects.clear();
	pkStatic->GetAllEntitys(&kObjects);
	PackToClient(iClient, kObjects,false);
	
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
	m_pkWorldObject->PrintTree(0);
}


void EntityManager::DumpActiverPropertysToLog(char* szMsg)
{
	Logf("net", "%s : %d\n", szMsg, m_akPropertys.size() );

	for(vector<Property*>::iterator it=m_akPropertys.begin();it!=m_akPropertys.end();it++) 
	{
		Logf("net", "%s (%d)", (*it)->m_acName, (*it)->GetObject()->m_iEntityID );
		if((*it)->GetObject()->m_pkParent)
			Logf("net", " Parent Obj: %s\n", (*it)->GetObject()->m_pkParent->m_strName.c_str() );
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
	Entity*	pkObject = GetObjectByNetWorkID(Msg.m_ToObject);

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

			for( i=0; i<m_pkZoneObject->m_akChilds.size(); i++) 
			{
				int iLocalZoneID = m_pkZoneObject->m_akChilds[i]->m_iEntityID;
				m_pkZoneObject->m_akChilds[i]->SetUpdateStatus(UPDATE_ALL);
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

Entity* EntityManager::CloneObject(int iNetID)
{
	Entity* pkObjOrginal = GetObjectByNetWorkID(iNetID);
	if(pkObjOrginal == NULL)
		return NULL;

	Entity* pkObjClone =	CreateObject();

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
	cout << "Now tracking " << kObject->GetObject()->m_iEntityID << endl;
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
   kFile.Read(&iNextObjectID,sizeof(int),1);

	iNextObjectID+=50;  //evil hack

	cout<<"Nr of zones  : "<<iNumOfZone<<endl;
	cout<<"Next objectID: "<<iNextObjectID<<endl;
		
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
   cout<<"Next id is:"<<iNextObjectID<<endl;
   kFile.Write(&iNextObjectID,sizeof(int),1);
	
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
		int iZone = GetZoneIndex((*iT)->GetObject(),(*iT)->GetObject()->m_iCurrentZone,(*iT)->m_bClosestZone);				
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
	Entity* kZoneEntity = CreateObject(false);
	kZoneEntity->m_bZone = true;
	kZData->m_pkZone = kZoneEntity;
	kZData->m_pkZone->SetParent(GetZoneObject());	
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
		
		//get current zone
		iZoneIndex = GetZoneIndex((*iT)->GetObject(),(*iT)->GetObject()->m_iCurrentZone,(*iT)->m_bClosestZone);
		
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

			if(iRange < m_iTrackerLOS) 
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












/*
bool EntityManager::LoadWorld(string strWDir, string strTempWDir)
{
	SetWorldDir(strWDir);
	//SetTempWorldDir(strTempWDir);	
	
	//clear the world
	Clear();

	//load zones in acDir
	return LoadZones();
}
*/

/*
void EntityManager::SetUnderConstruction(int iId)
{
	unsigned int i;

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
		
		int nrofstatic=0;
/*		//find all static objects and attach them to zoneobject

		for(i=0;i<kEntitys.size();i++)
			if(kEntitys[i]->m_iObjectType == OBJECT_TYPE_STATIC)
			{
				kEntitys[i]->SetParent(zd->m_pkZone);
				nrofstatic++;
			}
*	
		//update zonedata		
		zd->m_bUnderContruction = true;
		cout<<"Setting zone:"<<iId<<" in construction mode revision:"<<zd->m_iRevision<< " static entitys:"<<nrofstatic<<" dynamic entitys:"<<kEntitys.size()-nrofstatic<<endl;
		
	}
}

void EntityManager::CommitZone(int iId)
{
	unsigned int i;

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
	
		// Update PFind Mesh
		P_PfMesh* pkMesh = (P_PfMesh*)zd->m_pkZone->GetProperty("P_PfMesh");
		if(pkMesh) {
			pkMesh->CalcNaviMesh();
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
		
		
		int nrofstatic=0;		
/		//find all static objects and attach them to staticentity
		for(i=0;i<kEntitys.size();i++)
			if(kEntitys[i]->m_iObjectType == OBJECT_TYPE_STATIC)
			{	
				kEntitys[i]->SetParent(pkStaticEntity);
				nrofstatic++;
			}
*
	
		//update zone data
		zd->m_bUnderContruction = false;
		zd->m_iRevision++;		
		cout<<"committing zone:"<<iId<<" new revision is:"<<zd->m_iRevision<<" static entitys:"<<nrofstatic<<" dynamic entitys:"<<kEntitys.size()-nrofstatic<<endl;
	}
}
*/

/*
void EntityManager::Zones_Refresh()
{
	for(unsigned int i=0; i<m_kZones.size(); i++) {
		// Zones that need to load.
		if(m_kZones[i].m_bActive && m_kZones[i].m_pkZone == NULL) 
		{
			LoadZone(m_kZones[i].m_iZoneID);
			cout << "Load Zone: " << m_kZones[i].m_iZoneID << endl;
		}

		// Zones that need to unload
		if(m_kZones[i].m_bActive == false && m_kZones[i].m_pkZone) 
		{
			UnLoadZone(m_kZones[i].m_iZoneID);
			cout << "UnLoad Zone: " << m_kZones[i].m_iZoneID << endl;
		}

	}
}
*/


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
int EntityManager::CreateZone()
{
	return CreateZone(Vector3(0,0,0),Vector3(8,8,8));
}
*/

/*
void EntityManager::UpdateZones()
{


	float fTime = m_pkZeroFps->GetEngineTime();
	ZoneData* pkZone;
	ZoneData* pkStartZone;
	unsigned int iZ;


	//int iTrackerLOS = 3;

	// Set All Zones as inactive.
	for(iZ=0;iZ<m_kZones.size();iZ++) 
	{
		m_kZones[iZ].m_bTracked		= false;
		m_kZones[iZ].m_iRange		= 10000;
		
		if(m_kZones[iZ].m_pkZone)
			m_kZones[iZ].m_pkZone->SetUpdateStatus(UPDATE_NONE);
	}

	vector<ZoneData*>	m_kFloodZones;
	int iZoneIndex;

	// For each tracker.
	for(list<P_Track*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) 
	{
		// Find Active Zone.
		set<int>			kNewActiveZones;
		
		for(iZ=0;iZ<m_kZones.size();iZ++)
			m_kZones[iZ].m_iRange							= 10000;
		
		//get current zone
		iZoneIndex = GetZoneIndex((*iT)->GetObject(),(*iT)->GetObject()->m_iCurrentZone,(*iT)->m_bClosestZone);
		
		if(iZoneIndex >= 0) 
		{
			pkStartZone = &m_kZones[iZoneIndex];
			pkStartZone->m_iRange = 0;
			
			
			m_kFloodZones.push_back(pkStartZone);
		}

		// Flood Zones in rage to active.
		while(m_kFloodZones.size()) 
		{
			pkZone = m_kFloodZones.back();
			m_kFloodZones.pop_back();

			kNewActiveZones.insert(pkZone->m_iZoneID);

			pkZone->m_bTracked = true;
			int iRange = pkZone->m_iRange + 1;

			if(iRange < m_iTrackerLOS) 
			{
				for(unsigned int i=0; i<pkZone->m_iZoneLinks.size(); i++) 
				{
					ZoneData* pkOtherZone = GetZoneData(pkZone->m_iZoneLinks[i]); //				pkZone->m_pkZoneLinks[i];	//GetZoneData(pkZone->m_iZoneLinks[i]);				

					//if zone has already been checked continue whit the next one
					if(pkOtherZone->m_iRange <= iRange)	continue;		// Dvoid: ändrade till <= från <  , tycks snabba upp algoritmen med en faktor av ca 100000000 (pga att den lägger till samma zon flera gånger)
					
					//set new range 
					pkOtherZone->m_iRange = iRange;
					
					
					
					//add zone to flooded zones list
					m_kFloodZones.push_back(pkOtherZone);
				}				
			}
		}
		
		//find new loaded zones  , compare new actives zones whit last update to find new loaded zones
		(*iT)->m_iNewActiveZones.clear();
		set_difference(kNewActiveZones.begin(),kNewActiveZones.end(),(*iT)->m_iActiveZones.begin(),(*iT)->m_iActiveZones.end(), inserter((*iT)->m_iNewActiveZones, (*iT)->m_iNewActiveZones.begin()));
		
		//findout wich zones has been removed since last update, and add them to list to be sent to client (observer the list shuld not be cleared here, but in the code that sends the package)
		if((*iT)->m_iConnectID != -1)
			set_difference((*iT)->m_iActiveZones.begin(),(*iT)->m_iActiveZones.end(),kNewActiveZones.begin(),kNewActiveZones.end(), inserter((*iT)->m_iUnloadZones, (*iT)->m_iUnloadZones.begin()));		
		
		//if((*iT)->m_iNewActiveZones.size() != 0) cout<<"loaded:"<<(*iT)->m_iNewActiveZones.size()<<endl;
		//if((*iT)->m_iUnloadZones.size() != 0) cout<<"unloaded:"<<(*iT)->m_iUnloadZones.size()<<endl;
				
		//save new active zones in tracker
		(*iT)->m_iActiveZones = kNewActiveZones;
	}


	//loop trough all zones and load/unload them	
	int iOperations = 0;
	ZoneData* pkZoneRefresh;	
	for(unsigned int i=0; i<m_kZones.size(); i++) 
	{
	//	cout<<"handling zone "<<m_kZones[i].m_iZoneID<<endl;
	
		pkZoneRefresh = &m_kZones[i];

		//perform max X operations per frame
		if(iOperations < m_iMaxZoneIO)
		{
			// Load / Unload zones.
			if(pkZoneRefresh->m_bTracked && pkZoneRefresh->m_pkZone == NULL) 
			{
			//	cout<<"load "<<m_kZones[i].m_iZoneID<<endl;
				
				iOperations++;			
				LoadZone(pkZoneRefresh->m_iZoneID);	
			}
	
			// Zones that need to unload
			if(pkZoneRefresh->m_bTracked == false && pkZoneRefresh->m_pkZone) 
			{
			
				//check time since zone was last active, if longer than m_fZoneUnloadTime unload it
				if( (fTime - pkZoneRefresh->m_fInactiveTime) > m_fZoneUnloadTime)
				{		
					//cout<<"unload "<<m_kZones[i].m_iZoneID<<endl;
					
					iOperations++;
					UnLoadZone(pkZoneRefresh->m_iZoneID);
				}
			}
		}
		
		//all active zones 
		if(pkZoneRefresh->m_bTracked && pkZoneRefresh->m_pkZone)
		{
		//	cout<<"setting as active "<<m_kZones[i].m_iZoneID<<endl;
			pkZoneRefresh->m_pkZone->SetUpdateStatus(UPDATE_ALL);
			
			//update zone timer
			pkZoneRefresh->m_fInactiveTime = fTime;
		}
				
/
		//dvoid object loding ...eller nått
		if(pkZoneRefresh->m_bActive)
		{
			if(pkZoneRefresh->m_pkZone)
			{
				//cout<<"range:"<<pkZoneRefresh->m_iRange<<endl;
			
				if(pkZoneRefresh->m_fDistance < m_iObjectDistance)
				//if(pkZoneRefresh->m_iRange < m_iTrackerLOS / 2)
				{
					pkZoneRefresh->m_pkZone->SetUpdateStatus(UPDATE_ALL);
					pkZoneRefresh->m_pkZone->m_bSendChilds = true;					
					//cout<<"whit childs"<<endl;					
				}
				else
				{
					pkZoneRefresh->m_pkZone->SetUpdateStatus(UPDATE_NOCHILDS);
					pkZoneRefresh->m_pkZone->m_bSendChilds = false;
					//cout<<"no childs"<<endl;
				}
			}
		}
*	
	}



	//reset all new loaded zones
	for(list<P_Track*>::iterator iT2=m_kTrackedObjects.begin();iT2!=m_kTrackedObjects.end();iT2++) 
	{
		//if therse no connection on this tracker, we cant reset anything
		if((*iT2)->m_iConnectID == -1)
			continue;
	
		for(set<int>::iterator it3 = (*iT2)->m_iNewActiveZones.begin(); it3 != (*iT2)->m_iNewActiveZones.end();it3++)
		{
			ZoneData* zd = GetZoneData((*it3));
		
			if(zd)
			{
				if(zd->m_pkZone)
					zd->m_pkZone->ResetAllNetUpdateFlagsAndChilds((*iT2)->m_iConnectID);
				//cout<<"reseting zone:"<<endl;
			}
		}
	}
}
*/


/*
char* EntityManager::GetObjectTypeName(int eType)
{
	char* pkName = "";

	switch(eType) {
//		case OBJECT_TYPE_DYNAMIC: 		pkName = "OBJECT_TYPE_DYNAMIC";	break;
//		case OBJECT_TYPE_STATIC: 		pkName = "OBJECT_TYPE_STATIC";	break;
/*		case OBJECT_TYPE_PLAYER: 		pkName = "OBJECT_TYPE_PLAYER";	break;
		case OBJECT_TYPE_STATDYN:		pkName = "OBJECT_TYPE_STATDYN";	break;
//		case OBJECT_TYPE_DECORATION: 	pkName = "OBJECT_TYPE_DECORATION";	break;*
		}

	return pkName;
}
*/








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
		//Logf("net", "Check Object [%d]\n",(*it)->m_iEntityID );

		(*it)->m_iNetUpdateFlags |= m_iForceNetUpdate;
		
		if((*it)->NeedToPack() == false)					continue;
		if((*it)->m_eRole != NETROLE_AUTHORITY)		continue;

		NP.Write((*it)->m_iEntityID);
		Logf("net", "Object [%d]\n",(*it)->m_iEntityID );
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