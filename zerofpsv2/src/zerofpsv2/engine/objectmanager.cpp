#include "objectmanager.h"
#include "network.h"
#include "zerofps.h"
#include "netslaveobject.h"
#include "../basic/zfsystem.h"
#include "../basic/simplescript.h"
#include "../engine_systems/common/zoneobject.h"
#include "../engine_systems/propertys/primitives3d.h"
#include "../engine_systems/propertys/proxyproperty.h"
#include "../engine_systems/propertys/madproperty.h"
#include "fh.h"
#include "../engine_systems/script_interfaces/si_objectmanager.h"

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






ObjectManager::ObjectManager() 
: ZFSubSystem("ObjectManager") 
{

	iNextObjectID				= 0;
	m_bUpdate					= true;
	m_iTotalNetObjectData	= 0;
	m_iNumOfNetObjects		= 0;
	m_bDrawZones				= false;
	m_pScriptFileHandle		= NULL;

	Register_Cmd("o_logtree",FID_LOGOHTREE);	
	Register_Cmd("o_dumpp",FID_LOGACTIVEPROPERTYS);	
	Register_Cmd("sendmsg",FID_SENDMESSAGE, CSYS_FLAG_SRC_ALL, "sendmsg name id",2);	

	Register_Cmd("newworld",FID_NEWWORLD, CSYS_FLAG_SRC_ALL);	
	Register_Cmd("loadworld",FID_LOADWORLD, CSYS_FLAG_SRC_ALL);	
	Register_Cmd("saveworld",FID_SAVEWORLD, CSYS_FLAG_SRC_ALL);		
	Register_Cmd("setworlddir",FID_SETWORLDDIR, CSYS_FLAG_SRC_ALL);		
	
	Register_Cmd("loadzones",FID_LOADZONES, CSYS_FLAG_SRC_ALL);	
	Register_Cmd("savezones",FID_SAVEZONE, CSYS_FLAG_SRC_ALL);	

	RegisterVariable("l_showzones", &m_bDrawZones, CSYS_BOOL);
}

bool ObjectManager::StartUp()	
{
	m_pkZeroFps	=	static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));		
	m_pkNetWork	= static_cast<NetWork*>(GetSystem().GetObjectPtr("NetWork"));
	m_pkScript = static_cast<ZFScriptSystem*>(GetSystem().GetObjectPtr("ZFScriptSystem"));

	m_fEndTimeForceNet		= m_pkZeroFps->GetEngineTime();

	m_kWorldDirectory = "../data/testmap";

	//create all base objects
	Clear();

	TESTVIM_LoadArcheTypes("zfoh.txt");

	//setup script interface
	ObjectManagerLua::Init(this,m_pkScript);
	
	return true; 
}

bool ObjectManager::ShutDown() 
{ 
	return true; 
}

bool ObjectManager::IsValid()	{ return true; }

ObjectManager::~ObjectManager() 
{
	float fAvgObjSize;

	if(m_iNumOfNetObjects) {
		fAvgObjSize = float(m_iTotalNetObjectData / m_iNumOfNetObjects);
		}

	Logf("net", " Avg Obj Size: %f\n", fAvgObjSize);

	// Obs! Här skall resursen laddas ur, går dock inte pga timeout expire.
	//delete m_pScriptFileHandle;

}

void ObjectManager::CreateBaseObjects()
{
	iNextObjectID = 0;
	
	//top world object parent to all objects
	m_pkWorldObject						=	new Object();	
	m_pkWorldObject->GetName()			= "WorldObject";
	m_pkWorldObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkWorldObject->m_eRemoteRole	= NETROLE_NONE;

	//object that is parent to all zones
	m_pkZoneObject						=	new Object();	
	m_pkZoneObject->SetParent(m_pkWorldObject);
	m_pkZoneObject->GetName()		= "ZoneObject";
	m_pkZoneObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkZoneObject->m_eRemoteRole	= NETROLE_NONE;

	//object that is parent to all client objects
	m_pkClientObject						=	new Object();	
	m_pkClientObject->SetParent(m_pkWorldObject);
	m_pkClientObject->GetName()		= "ClientObject";
	m_pkClientObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkClientObject->m_eRemoteRole	= NETROLE_NONE;

	//object that is parent to all global objects (server information etc)
	m_pkGlobalObject						=	new Object();	
	m_pkGlobalObject->SetParent(m_pkWorldObject);
	m_pkGlobalObject->GetName()		= "GlobalObject";
	m_pkGlobalObject->m_eRole			= NETROLE_AUTHORITY;
	m_pkGlobalObject->m_eRemoteRole	= NETROLE_NONE;

	iNextObjectID = 100000;

}

/**	\brief	Add's a object to the ObjectManger.

  This function is called by objects as they are created. It assigned a NetWorkID to the object and
  also put them in the ObjectManger.
*/
void ObjectManager::Add(Object* pkObject) 
{
	pkObject->iNetWorkID = iNextObjectID++;
	m_akObjects.push_back(pkObject);
}

/**	\brief	Dont use this..use Delete instead
*/
void ObjectManager::Remove(Object* pkObject) 
{	
	// If i own object mark so we remove it on clients.
	if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
		m_aiNetDeleteList.push_back(pkObject->iNetWorkID);
	m_akObjects.remove(pkObject);
}

/**	\brief	Adds an object to delete qeue
*/

void ObjectManager::Delete(Object* pkObject) 
{
	for(vector<int>::iterator it=m_aiDeleteList.begin();it!=m_aiDeleteList.end();it++) 
	{
		if(pkObject->iNetWorkID == (*it)) {
			//Logf("net", "Object [%d] already in delete list\n", pkObject->iNetWorkID);
			//cout << "Object already in delete list" << endl;
			return;
		}
	}
	
	m_aiDeleteList.push_back(pkObject->iNetWorkID);
}

/**	\brief	Adds an object to delete qeue
		\todo	Should delete all zones.

	Clear all data from ObjectManger.
*/
void ObjectManager::Clear()
{
//	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
//		delete(*it);
	
/*		if((*it)->CheckLinks(false,0) == false)
			cout << "Error in object manger" << endl;
		else
			cout << "Links ok" << endl;*/
//	} 
	
	
	//m_pkWorldObject->DeleteAllChilds();
	
	//delete all objects in world
	while(m_akObjects.begin() != m_akObjects.end())
		delete(*m_akObjects.begin());
	
	//clear all zones
	m_kZones.clear();
	
	//recreate base objects
	CreateBaseObjects();
}


/**	\brief	Run update on selected propertys.

	This function collects all propertys that fit the selected flags, sorts them if it needs to
	and then runs the update function of each of this propertys.

	
*/

void ObjectManager::Update(int iType,int iSide,bool bSort)
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

void ObjectManager::UpdateDelete()
{
/*	int i=0;
	for(vector<Object*>::iterator it2=m_akDeleteList.begin();it2!=m_akDeleteList.end();it2++) 
	{
		Object* pkObject = (*it2);
		//cout << "[" << i << "]:" << pkObject->GetName() << "," << pkObject->iNetWorkID << endl;
		i++;
	}

	int iSize = m_akDeleteList.size();

	if(m_akDeleteList.size()==0)
		return;
	
	for(vector<Object*>::iterator it=m_akDeleteList.begin();it!=m_akDeleteList.end();it++) 
	{
		Object* pkObject = (*it);
		// If i own object mark so we remove it on clients.
		if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
			m_aiNetDeleteList.push_back((*it)->iNetWorkID);
		delete (*it);		
	}

	m_akDeleteList.clear();*/

/*	int i=0;
	for(vector<int>::iterator it2=m_aiDeleteList.begin();it2!=m_aiDeleteList.end();it2++) 
	{
		
		Object* pkObject = GetObjectByNetWorkID((*it2));
		//cout << "[" << i << "]:" << pkObject->GetName() << "," << pkObject->iNetWorkID << endl;
		i++;
	}*/

	int iSize = m_aiDeleteList.size();

	if(m_aiDeleteList.size()==0)
		return;
	
	for(vector<int>::iterator it=m_aiDeleteList.begin();it!=m_aiDeleteList.end();it++) 
	{
		Object* pkObject = GetObjectByNetWorkID((*it));
		if(pkObject) { // If i own object mark so we remove it on clients.
			if(pkObject->m_eRole == NETROLE_AUTHORITY && pkObject->m_eRemoteRole == NETROLE_PROXY)
				m_aiNetDeleteList.push_back((*it));
			delete pkObject;		
			}
	}

	m_aiDeleteList.clear();

}

void ObjectManager::UpdateGameMessages(void)
{
	// Let Objects/Propertys handle messages
	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		(*it)->HandleMessages();
	}
}


/**	\brief	Creates a new clean object.

	Creates a basic object without any propertys and all values set to defualt. 
*/
Object* ObjectManager::CreateObject()
{
	Object* pkObj = new Object;
	//cout << "CreateObject :" << pkObj->iNetWorkID << endl;
	return pkObj;
}

Object* ObjectManager::CreateObjectByNetWorkID(int iNetID)
{
//	Object *pkNew = new NetSlaveObject;
	Object *pkNew = CreateObject();

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
Object* ObjectManager::CreateObjectFromScriptInZone(const char* acName,Vector3 kPos,int iCurrentZone)
{
	int id = GetZoneIndex(kPos,iCurrentZone,false);
	
	if(id == -1)
		return NULL;
	
	//force loading of this zone
	LoadZone(id);
	
	if(!m_kZones[id].m_pkZone)
		return NULL;
		
		
	Object* newobj = CreateObjectFromScript(acName);
	
	if(newobj)
	{		
		newobj->SetUseZones(true);
		newobj->SetWorldPosV(kPos);	
		if(newobj->m_iCurrentZone == -1)
			cout<<"ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
	}

	return newobj;
}

Object* ObjectManager::CreateObjectFromScript(const char* acName)
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
	
	Object* pkReturnObj = ObjectManagerLua::g_pkReturnObject;
	
	//pop pointers
	ObjectManagerLua::Pop();			
	
	return pkReturnObj;
}



/**	\brief	Creates a object from the zfoh.txt file.

	Creates a object as described in the zfoh.txt file (poor man script). If a object is
	not found with the choosen name no object will be created.
*/
Object* ObjectManager::CreateObjectByArchType(const char* acName)
{
	ObjectArcheType* pkAt = GetArcheType(string(acName));
	if(!pkAt)
		return false;

	Object* pkObj	=	CreateObject();
	pkObj->m_eRemoteRole	= NETROLE_PROXY;
	pkObj->m_eRole			= NETROLE_AUTHORITY;

	AddArchPropertys(pkObj, string(acName));

	pkObj->m_strType	= acName;
	pkObj->m_strName	= string("A ") + pkObj->m_strType;

	return pkObj;
}

bool ObjectManager::IsA(Object* pkObj, string strStringType)
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
}

// Gets
void ObjectManager::GetAllObjects(vector<Object*> *pakObjects)
{
	m_pkWorldObject->GetAllObjects(pakObjects);
}

Object* ObjectManager::GetObject(const char* acName)
{
	vector<Object*> kObjects;		
	GetAllObjects(&kObjects);
	
	for(vector<Object*>::iterator it=kObjects.begin();it!=kObjects.end();it++) {
		if((*it)->GetName() == acName)
		{
			return (*it);
		}
	}	
	
	return NULL;
}

Object*	ObjectManager::GetObjectByNetWorkID(int iNetID)
{
	if(iNetID == -1)
		return NULL;

	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) {
		if((*it)->iNetWorkID == iNetID)
			return (*it);
	}

	return NULL;
}


// NetWork
void ObjectManager::UpdateDeleteList(NetPacket* pkNetPacket)
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
}

void ObjectManager::UpdateState(NetPacket* pkNetPacket)
{
	Object* pkNetSlave;
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
			pkNetSlave->PackFrom(pkNetPacket);
			if(pkNetPacket->IsReadError())
				return;

			pkNetPacket->Read(iObjectID);
			}
		else {
			//Logf("net", " Object '%d' not found (again) :(.\n", iObjectID);
			return;
			}
		}	
}

void ObjectManager::PackToClient(int iClient, vector<Object*> kObjects)
{
	int iPacketSize = 0;
	int iEndOfObject = -1;

	Object* pkPackObj;

	NetPacket NP;
	NP.Clear();
	NP.m_kData.m_kHeader.m_iPacketType = ZF_NETTYPE_UNREL;
	NP.Write((char) ZFGP_OBJECTSTATE);

	for(int iObj=0; iObj < kObjects.size(); iObj++)	{
		pkPackObj = kObjects[iObj];

		pkPackObj->m_iNetUpdateFlags |= m_iForceNetUpdate;

		if(pkPackObj->NeedToPack() == false)				continue;
		if(pkPackObj->m_eRole != NETROLE_AUTHORITY)		continue;

		NP.Write(pkPackObj->iNetWorkID);
		Logf("net", "Object [%d]\n",pkPackObj->iNetWorkID );
		pkPackObj->PackTo(&NP);
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

void ObjectManager::PackZoneListToClient(int iClient, set<int>& iZones /*TrackProperty* pkTrack*/)
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
	for(int i=0; i<iArray.size(); i++) {
		if(iArray[i] == iVal)
			return true;
		}
	

	return false;
}

void ObjectManager::UpdateZoneList(NetPacket* pkNetPacket)
{
	int i;

	vector<int>	kZones;
	int iZoneID;

	pkNetPacket->Read(iZoneID);
	while(iZoneID != -1) {
		kZones.push_back(iZoneID);
		pkNetPacket->Read(iZoneID);
		}

/*	
	cout << "Active Zones: "; 
	for(i=0; i<kZones.size(); i++) 
		cout << kZones[i] << ", ";
	cout << endl;
*/

	for(i=0; i<m_pkZoneObject->m_akChilds.size(); i++) {
		int iLocalZoneID = m_pkZoneObject->m_akChilds[i]->iNetWorkID;
		
		if(IsInsideVector(iLocalZoneID, kZones) == false) {
			Delete(m_pkZoneObject->m_akChilds[i]);
			//cout << "Removing Zone: " << iLocalZoneID << endl;
			}
		}
}


void ObjectManager::PackToClients()
{
	// If no clients we don't send anything.
	if(m_pkNetWork->GetNumOfClients() == 0)
		return;

	Logf("net", " *** ObjectManager::PackToClients() *** \n");


/*	if(m_pkZeroFps->GetEngineTime() < m_fEndTimeForceNet) {
		m_iForceNetUpdate = 0xFFFFFFFF;
		cout << "Forcing Object network updates" << endl;
		}
	else {
		m_iForceNetUpdate  = 0x0;					
		}*/

	vector<Object*>	kObjects;
	m_iForceNetUpdate = 0xFFFFFFFF;
	NetPacket NP;
	
	// Keep it alive.
	int iNumOfObjects	= m_akObjects.size();
	int iPacketSize		= 0;
	int iEndOfObject	= -1;

	Object* pkZone;
	int iClient;

	// Clear Active Zones for clients.
	for(iClient=0; iClient < m_pkZeroFps->m_kClient.size(); iClient++)
		m_pkZeroFps->m_kClient[iClient].m_iActiveZones.clear();
	
	// Refresh list of active Zones for each client.
	for(list<Object*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) {
		TrackProperty* pkTrack = dynamic_cast<TrackProperty*>((*iT)->GetProperty("TrackProperty"));
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
			pkZone->GetAllObjects(&kObjects);
			PackToClient(iClient, kObjects);
			}
		}

	if(m_aiNetDeleteList.size() == 0)
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

	m_aiNetDeleteList.clear();
}


/*
void ObjectManager::PackToClients()
{
	if(m_pkNetWork->GetNumOfClients() == 0)
		return;

	Logf("net", " *** ObjectManager::PackToClients() *** \n");


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


// Debug / Help Functions		
void ObjectManager::DisplayTree()
{
	GetSystem().Log_Create("fisklins");
	m_pkWorldObject->PrintTree(0);
}


void ObjectManager::DumpActiverPropertysToLog(char* szMsg)
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
void ObjectManager::SendMsg()
{

}

void ObjectManager::RouteMessage(GameMessage& Msg)
{
	Object*	pkObject = GetObjectByNetWorkID(Msg.m_ToObject);

	if(pkObject == NULL) {
		cout << "No Valid object for message" << endl;		
		return;
		}

	pkObject->AddGameMessage(Msg);
}


// Get Strings.
char* ObjectManager::GetUpdateStatusName(int eStatus)
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

char* ObjectManager::GetObjectTypeName(int eType)
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

char* ObjectManager::GetPropertyTypeName(int iType)
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

char* ObjectManager::GetPropertySideName(int iSide)
{
	char* pkName = "";

	switch(iSide) {
		case PROPERTY_SIDE_ALL: 		pkName = "PROPERTY_SIDE_ALL";	break;
		case PROPERTY_SIDE_CLIENT: 		pkName = "PROPERTY_SIDE_CLIENT";	break;
		case PROPERTY_SIDE_SERVER: 		pkName = "PROPERTY_SIDE_SERVER";	break;
		}

	return pkName;


}


// Object ArcheTypes

ObjectArcheType*	ObjectManager::GetArcheType(string strName)
{
	for(list<ObjectArcheType*>::iterator it=m_akArcheTypes.begin();it!=m_akArcheTypes.end();it++) {
		if((*it)->m_strName == strName)
			return (*it);
		}

	return NULL;
	
}

void ObjectManager::AddArchPropertys(Object* pkObj, string strName)
{
	ObjectArcheType* pkAt = GetArcheType(strName);
	if(!pkAt)
		return;

	Property* pkProperty;

	AddArchPropertys(pkObj, pkAt->m_strParentName);
	for(unsigned int i=0; i<pkAt->m_kArchPropertys.size(); i++) {
		pkProperty = pkObj->AddProxyProperty(pkAt->m_kArchPropertys[i].m_strName.c_str());

		for(unsigned int j=0; j<pkAt->m_kArchPropertys[i].m_kVariables.size(); j++) {
			pkProperty = pkObj->GetProperty(pkAt->m_kArchPropertys[i].m_strName.c_str());
	
			pkProperty->SetValue(pkAt->m_kArchPropertys[i].m_kVariables[j].m_strVariable.c_str(),
				pkAt->m_kArchPropertys[i].m_kVariables[j].m_strValue.c_str());
			}
		}
	
}

void ObjectManager::TESTVIM_LoadArcheTypes(char* szFileName)
{
	SimpleScriptFile kMMScipt;

	if(kMMScipt.LoadScript(szFileName) == false) {
		cout << "Failed to load script " << szFileName << endl;
		return;
		}


	char* ucpToken;
	ucpToken = kMMScipt.GetToken();
	ObjectArcheType* pkAt = NULL;
	
	string strPropName, strPropVar, strPropValue;

	while(ucpToken)
	{
		if (!strcmp (ucpToken, "parent")) {
			if(pkAt) {
				ucpToken = kMMScipt.GetToken();
				pkAt->m_strParentName = ucpToken;
				}
		}

		if (!strcmp (ucpToken, "class")) {
			if(pkAt) {
				m_akArcheTypes.push_back(pkAt);
				pkAt = NULL;
				}

			pkAt = new ObjectArcheType;
			ucpToken = kMMScipt.GetToken();
			pkAt->m_strName = ucpToken;
			//cout << "Creating class: " << ucpToken << endl;
			}

		
		if (!strcmp (ucpToken, "set")) {
			if(pkAt) {
				ucpToken = kMMScipt.GetToken();
				strPropName = ucpToken;

				ucpToken = kMMScipt.GetToken();
				strPropVar = ucpToken;

				ucpToken = kMMScipt.GetToken();
				strPropValue = ucpToken;
	
				pkAt->SetValue(strPropName, strPropVar, strPropValue);
				//cout << "Set:  " << strPropName << "." <<  strPropVar << "=" << strPropValue << endl;
				}
			}

		if (!strcmp (ucpToken, "add")) {
			if(pkAt) {
				ucpToken = kMMScipt.GetToken();
				strPropName = ucpToken;
				pkAt->GetAddArchProperty(string (strPropName) );
				//cout << "Add:  " << strPropName << endl;
				}
			}

		ucpToken = kMMScipt.GetToken();
	}

	if(pkAt) {
		m_akArcheTypes.push_back(pkAt);
		pkAt = NULL;
		}
}

void ObjectManager::GetArchObjects(vector<string>* pkFiles, string strParentName)
{
	list<ObjectArcheType*>::iterator it;

	if(strParentName == "") {
		for(it=m_akArcheTypes.begin();it!=m_akArcheTypes.end();it++) {
			pkFiles->push_back((*it)->m_strName);
			}
		}
	else {
		for(it=m_akArcheTypes.begin();it!=m_akArcheTypes.end();it++) {
			if((*it)->m_strParentName == strParentName)
				pkFiles->push_back((*it)->m_strName);
			}
		}
}


void ObjectManager::GetPropertys(int iType,int iSide)
{
	m_akPropertys.clear();
	m_pkWorldObject->GetAllPropertys(&m_akPropertys,iType,iSide);
}

bool ObjectManager::TestLine(vector<Object*>* pkPPList,Vector3 kPos,Vector3 kVec)
{
	pkPPList->clear();

	for(list<Object*>::iterator it=m_akObjects.begin();it!=m_akObjects.end();it++) 
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


void ObjectManager::RunCommand(int cmdid, const CmdArgument* kCommand) 
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
			ForceUnload();
			break;
		
		case FID_SETWORLDDIR:
			SetWorldDir(kCommand->m_kSplitCommand[1].c_str());		
			SaveZones();
			break;
	}	

}

void ObjectManager::OwnerShip_Take(Object* pkObj)
{
	if(!pkObj)
		return;
	pkObj->m_eRole			= NETROLE_AUTHORITY;
	pkObj->m_eRemoteRole	= NETROLE_PROXY;
}

void ObjectManager::OwnerShip_Give(Object* pkObj)
{
	if(!pkObj)
		return;
	pkObj->m_eRole			= NETROLE_PROXY;
	pkObj->m_eRemoteRole	= NETROLE_AUTHORITY;
}

void ObjectManager::OwnerShip_Request(Object* pkObj)
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

void ObjectManager::OwnerShip_OnRequest(Object* pkObj)
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

void ObjectManager::OwnerShip_OnGrant(Object* pkObj)
{
	if(pkObj == NULL)
		return;

	OwnerShip_Take(pkObj);
	Logf("net", " This node now own %d\n", pkObj->iNetWorkID);
}

Object* ObjectManager::CloneObject(int iNetID)
{
	Object* pkObjOrginal = GetObjectByNetWorkID(iNetID);
	if(pkObjOrginal == NULL)
		return NULL;

	Object* pkObjClone =	CreateObject();
	pkObjClone->MakeCloneOf(pkObjOrginal);
	return pkObjClone;
}



void ObjectManager::Test_CreateZones()
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
}

void ObjectManager::Test_DrawZones()
{
	if(!m_bDrawZones)
		return;

	Render* m_pkRender=static_cast<Render*>(GetSystem().GetObjectPtr("Render"));

	for(unsigned int i=0;i<m_kZones.size();i++) {
		if(!m_kZones[i].m_bUsed)
			continue;
			
		Vector3 kMin = m_kZones[i].m_kPos - m_kZones[i].m_kSize/2;
		Vector3 kMax = m_kZones[i].m_kPos + m_kZones[i].m_kSize/2;

		if(m_kZones[i].m_bActive)
			m_pkRender->DrawAABB( kMin,kMax, Vector3(1,0,0) );
		else 
			m_pkRender->DrawAABB( kMin,kMax, Vector3(0,1,0) );
	
		for(int j = 0 ;j< m_kZones[i].m_iZoneLinks.size();j++)
		{	
			glColor4f(0,0,1,0);
			m_pkRender->Line(m_kZones[i].m_kPos,m_kZones[m_kZones[i].m_iZoneLinks[j]].m_kPos);
		}
	
	}
}

void ObjectManager::AutoConnectZones()
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
}

Vector3 ObjectManager::GetZoneCenter(int iZoneNum)
{
	return m_kZones[iZoneNum].m_kPos;
}

int ObjectManager::GetNumOfZones()
{
	return m_kZones.size();
}

list<Object*>* ObjectManager::GetTrackerList()
{
	return &m_kTrackedObjects;
}

void ObjectManager::AddTracker(Object* kObject)
{
	cout << "Now tracking " << kObject->iNetWorkID << endl;
	m_kTrackedObjects.push_back(kObject);
}

void ObjectManager::RemoveTracker(Object* kObject)
{
	m_kTrackedObjects.remove(kObject);
}

int ObjectManager::GetNrOfTrackedObjects()
{
	return m_kTrackedObjects.size();
}

void ObjectManager::ClearTrackers()
{
	m_kTrackedObjects.clear();
}

ZoneData* ObjectManager::GetZone(Vector3 kPos)
{
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
		if(!m_kZones[iZ].m_bUsed)
			continue;

		if(m_kZones[iZ].IsInside(kPos))
			return &m_kZones[iZ];
		}

	return NULL;
}


ZoneData* ObjectManager::GetZone(Object* PkObject)
{
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
		if(!m_kZones[iZ].m_bUsed)
			continue;
		
		if(m_kZones[iZ].IsInside(PkObject->GetWorldPosV()))
			return &m_kZones[iZ];
		}

	return NULL;
}

int ObjectManager::GetZoneIndex(Object* PkObject,int iCurrentZone,bool bClosestZone)
{
	return GetZoneIndex(PkObject->GetWorldPosV(),iCurrentZone,bClosestZone);
}

int ObjectManager::GetZoneIndex(Vector3 kMyPos,int iCurrentZone,bool bClosestZone)
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


void ObjectManager::UpdateZones()
{
	float fTime = m_pkZeroFps->GetGameTime();
	ZoneData* pkZone;
	ZoneData* pkStartZone;


	int iTrackerLOS = 5;

	// Set All Zones as inactive.
	for(unsigned int iZ=0;iZ<m_kZones.size();iZ++) {
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
	for(list<Object*>::iterator iT=m_kTrackedObjects.begin();iT!=m_kTrackedObjects.end();iT++) {
		// Find Active Zone.
		TrackProperty* pkTrack = dynamic_cast<TrackProperty*>((*iT)->GetProperty("TrackProperty"));
		pkTrack->m_iActiveZones.clear();
		
		//get current zone
		iZoneIndex = GetZoneIndex((*iT),(*iT)->m_iCurrentZone,pkTrack->m_bClosestZone);
		
		if(iZoneIndex >= 0) {
			pkStartZone = &m_kZones[iZoneIndex];
			pkStartZone->m_iRange = 0;
			m_kFloodZones.push_back(pkStartZone);
		}

		// Flood Zones in rage to active.
		while(m_kFloodZones.size()) {
			pkZone = m_kFloodZones.back();
			m_kFloodZones.pop_back();

			pkTrack->m_iActiveZones.insert(pkZone->m_iZoneID);

			pkZone->m_bActive = true;
			int iRange = pkZone->m_iRange + 1;

			if(iRange < iTrackerLOS) {
				for(unsigned int i=0; i<pkZone->m_iZoneLinks.size(); i++) {
					ZoneData* pkOtherZone = GetZoneData(pkZone->m_iZoneLinks[i]); //				pkZone->m_pkZoneLinks[i];	//GetZoneData(pkZone->m_iZoneLinks[i]);				

					if(pkOtherZone->m_iRange < iRange)	continue;
					pkOtherZone->m_iRange = iRange;
					if(pkOtherZone->m_iRange < iTrackerLOS)
						m_kFloodZones.push_back(pkOtherZone);
					}
				}
			}
		}


	// Age Inactive Zones.

	// If Inactive Zone to old let it die.
	ZoneData* pkZoneRefresh;

	for(unsigned int i=0; i<m_kZones.size(); i++) {
		pkZoneRefresh = &m_kZones[i];

		// Zones that need to load.
		if(pkZoneRefresh->m_bActive && pkZoneRefresh->m_pkZone == NULL) {
			LoadZone(pkZoneRefresh->m_iZoneID);
			//cout << "Load Zone: " << pkZoneRefresh->m_iZoneID << endl;
			}

		// Zones that need to unload
		if(pkZoneRefresh->m_bActive == false && pkZoneRefresh->m_pkZone) {
			UnLoadZone(pkZoneRefresh->m_iZoneID);
			//cout << "UnLoad Zone: " << pkZoneRefresh->m_iZoneID << endl;
			}

		if(pkZoneRefresh->m_bActive)
			pkZoneRefresh->m_pkZone->GetUpdateStatus()		= UPDATE_ALL;
	}
}

void ObjectManager::Zones_Refresh()
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


vector<int>	ObjectManager::GetActiveZoneIDs(int iTracker)
{
	vector<int>	Nisse;
	return Nisse;
}

int ObjectManager::CreateZone()
{
	return CreateZone(Vector3(0,0,0),Vector3(8,8,8));
}

int ObjectManager::CreateZone(Vector3 kPos,Vector3 kSize)
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
	
	UpdateZoneLinks(id);
	
	return id;
}


void ObjectManager::DeleteZone(int iId)
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



bool ObjectManager::LoadZones()
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

	cout<<"Nr of zones:"<<iNumOfZone<<endl;
	ZoneData kZData;
	kZData.m_bUsed = true;
	
	int i,zl;
	int iLink;

	for( i=0; i<iNumOfZone; i++) {
		kFile.Read(&kZData.m_bNew, sizeof(kZData.m_bNew), 1);
		kFile.Read(&kZData.m_bUsed, sizeof(kZData.m_bUsed), 1);						
		kFile.Read(&kZData.m_iZoneID, sizeof(kZData.m_iZoneID), 1);
		kFile.Read(&kZData.m_kSize, sizeof(kZData.m_kSize), 1);
		kFile.Read(&kZData.m_kPos, sizeof(kZData.m_kPos), 1);


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

bool ObjectManager::SaveZones()
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

	
	for(int i=0; i<iNumOfZone; i++) 
	{

		kFile.Write(&m_kZones[i].m_bNew, sizeof(m_kZones[i].m_bNew), 1);
		kFile.Write(&m_kZones[i].m_bUsed, sizeof(m_kZones[i].m_bUsed), 1);				
		kFile.Write(&m_kZones[i].m_iZoneID, sizeof(m_kZones[i].m_iZoneID), 1);
		kFile.Write(&m_kZones[i].m_kSize, sizeof(m_kZones[i].m_kSize), 1);		
		kFile.Write(&m_kZones[i].m_kPos, sizeof(m_kZones[i].m_kPos), 1);

		int iNumOfLinks = m_kZones[i].m_iZoneLinks.size();		
		kFile.Write(&iNumOfLinks, sizeof(iNumOfLinks), 1);		
		
		for(int zl=0; zl < iNumOfLinks; zl++)
			kFile.Write(&m_kZones[i].m_iZoneLinks[zl], sizeof(m_kZones[i].m_iZoneLinks[zl]), 1);
	}



	kFile.Close();

	cout<<"zones saved"<<endl;
	return true;
}

ZoneData* ObjectManager::GetZoneData(int iID)
{
	if(iID < 0 || iID >= m_kZones.size())
		return NULL;

	return &m_kZones[iID];

}

void ObjectManager::LoadZone(int iId)
{	
	ZoneData* kZData = GetZoneData(iId);
	assert(kZData);

	// Check if zone is loaded. If soo return.
	if(kZData->m_pkZone)
		return;

	// Create Object.
	Object* object = new Object;//ZoneObject;
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
		object->GetUpdateStatus()=UPDATE_DYNAMIC;
		object->AddProperty("LightUpdateProperty");	//always attach a lightupdateproperty to new zones

		SetZoneModel("data/mad/zones/emptyzone.mad",iId);

		return;
	}
 
	kZData->m_pkZone->Load(&kFile);
	kFile.Close();
	
}

void ObjectManager::UnLoadZone(int iId)
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

	Delete(kZData->m_pkZone);
	kZData->m_pkZone = NULL;
}


int ObjectManager::GetUnusedZoneID()
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


bool ObjectManager::LoadWorld(const char* acDir)
{
	SetWorldDir(acDir);
	
	//clear the world
	Clear();

	//load zones in acDir
	return LoadZones();
}

void ObjectManager::ClearZoneLinks(int iId)
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

bool ObjectManager::IsInsideZone(Vector3 kPos,Vector3 kSize)
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

void ObjectManager::UpdateZoneLinks(int iId)
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


bool ObjectManager::BoxVSBox(Vector3 kPos1,Vector3 kSize1,Vector3 kPos2,Vector3 kSize2)
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


void ObjectManager::SetZoneModel(const char* szName,int iId)
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

	MadProperty* mp = (MadProperty*)zd->m_pkZone->GetProperty("MadProperty");
	
	if(!mp)
	{
		mp = (MadProperty*)zd->m_pkZone->AddProperty("MadProperty");
	}
		
	if(mp)
	{
		mp->SetBase(szName);
	}
}



void ObjectManager::ForceUnload()
{	

	//loop trough all loaded zones, and unload em , to make sure that all zones is saved
	for(unsigned int i=0;i<m_kZones.size();i++) 
	{
		if(!m_kZones[i].m_bUsed)
			continue;
	
		if(m_kZones[i].m_pkZone)
			UnLoadZone(i);		
	}
	
	//after unloading , reload 	
	UpdateZones();
}
