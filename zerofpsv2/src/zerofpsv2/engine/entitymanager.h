#ifndef _ENGINE_THE_OBJECTMANAGER_H_
#define _ENGINE_THE_OBJECTMANAGER_H_

#include "entity.h"
#include "property.h"
#include <vector>
#include <list>
#include "../script/zfscript.h"

using namespace std;

class PropertyDescriptor;
class ObjectDescriptor;
class ZeroFps;
class GameMessage;
class NetWork;
class ZoneObject;
class P_Track;

enum EZoneStatus
{
	EZS_LOADED,
	EZS_UNLOADED,
	EZS_CACHED,
	EZS_UNUSED,
};

/// Data about a Zone in the World.
class ZoneData
{
public:

	int					m_iStatus;					//current status of zone

	bool					m_bNew;
	unsigned int		m_iRevision;		
	Entity*				m_pkZone;
	int					m_iZoneObjectID;
	int					m_iZoneID;
	Vector3				m_kPos;
	Vector3				m_kSize;
	vector<int>			m_iZoneLinks;
	string				m_strEnviroment;

	float					m_fInactiveTime;
	bool					m_bTracked;					//is there a tracker touching this zone
	int					m_iRange;						// Range to tracker i num of zones.
	
	bool IsInside(Vector3 kPoint);
	ZoneData();
	void Clear();
};



/**	\brief	SubSystem that handles all entity's in the world.
	 \ingroup Engine

Handles the mangment of all entity's that make up the game world. The EM are used to
create, update and destroy entity's. It also handles to operation of replicating entity's
to other connected users that need to now about them.
*/

class ENGINE_API EntityManager : public ZFSubSystem{
	private:
		enum FuncId_e
		{
			FID_LOGOHTREE,
			FID_LOGACTIVEPROPERTYS,
			FID_SENDMESSAGE,
			
			FID_NEWWORLD,
			FID_LOADWORLD,
			FID_SETWORLDDIR,
			FID_SAVEWORLD,
			
		};

		struct Property_Less : public binary_function<Property*, Property*, bool> {
			bool operator()(Property* x, Property* y) { return *x < *y; };
		} Less_Property;
	
		ZeroFps*						m_pkZeroFps;
		ZFScriptSystem* 			m_pkScript;
		ZFResourceHandle* 		m_pScriptFileHandle;
		NetWork*						m_pkNetWork;
		ZFBasicFS*					m_pkBasicFS;

		//base objects
		Entity*						m_pkWorldObject;											///< Top level entity.
		Entity*						m_pkZoneObject;											///< Top level entity.
		Entity*						m_pkClientObject;											///< Top level entity.
		Entity*						m_pkGlobalObject;											///< Top level entity.
		
		//current world directory to save/load zone data to 
		string						m_kWorldDirectory;
		
		map<int,Entity*>			m_akEntitys;
		
		// DELETE
		vector<int>					m_aiDeleteList;
		
		// Zones
		vector<ZoneData>			m_kZones;
		
		// Trackers
		list<P_Track*>				m_kTrackedObjects;	
		int							m_iTrackerLOS;												//tracker line of sight
		float							m_iObjectDistance;											//tracker line of sight		
		float							m_fZoneUnloadTime;
		int							m_iMaxZoneIO;
			
		vector<Property*>			m_akPropertys;												///< List of Active Propertys.	
		int							m_iNrOfActivePropertys;									///> Size of akProperty list.

		int							iNextObjectID;												///< Next free object ID.
		bool							m_bDrawZones;
		bool							m_bDrawZoneConnections;

		NetPacket	m_OutNP;		// Used to create/send updates to clients.		
		
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void GetPropertys(int iType,int iSide);						///< Fill propery list.

		//private zone system functions
		void UpdateTrackers();
		void UpdateZoneStatus();		
		
		void GetZones(set<int>* kZones,int iZone,Vector3 kPos,float fRadius);
		void Test_DrawZones();
		void ClearZoneLinks(int iId);
		void UpdateZoneLinks(int iId);
		
		void ForceUnload();												//forcing unload of all loaded zones
		void ForceSave();													//forcing save of all loaded zones
		bool LoadZones(string strSaveDir ="");						//load zone info list
		bool SaveZones(string strSaveDir ="");						//save zone info list
		bool SaveTrackers(string strSaveDir = "");
		bool LoadTrackers(string strSaveDir = "");
		void UnLoadZone(int iId);										//unload zone (saves and deletes)

		//void UpdateZones();
		
				
	public:
		float		m_fSimTime;				// Time in the virtual world.
		float		m_fSimTimeScale;		// How fast does the time in the sim run with respect to the real world time. 
		//float 	m_fGameFrameTime;
		float		m_fSimTimeDelta;
		float		GetSimTime()				 {	return m_fSimTime; }
		float		GetGameTime()				{	return m_fSimTime;};
		//float		GetGameFrameTime()		{	return m_fGameFrameTime;};
		float		GetSimDelta()				{	return m_fSimTimeDelta;	}

		int		m_iForceNetUpdate;					
		float		m_fEndTimeForceNet;

		int		m_iUpdateFlags;												///< Flags of active update.
		int		m_iRole;															///< I'm i a server or a client or both.

		int		m_iTotalNetObjectData;
		int		m_iNumOfNetObjects;

		EntityManager();
		~EntityManager();
		bool StartUp();
		bool ShutDown();
		bool IsValid();
	
		// Add/Remove Objects
		void Link(Entity* pkNewObject,int iId = -1);									///< Link this to the Object manager
		void UnLink(Entity* pkObject);									///< UnLink this from Object Manger.
		bool IsLinked(Entity* pkObject);
		void Clear();															///< Delete all objects.
		void CreateBaseObjects();											/// create all base objects	


		// Create 
		Entity* CreateObject(bool bLink = true);												///< Create a empty object.
		Entity* CreateObjectByNetWorkID(int iNetID);					///< Create object with selected NetworkID
		Entity* CreateObjectFromScript(const char* acName);
		Entity* CreateObjectFromScriptInZone(const char* acName,Vector3 kPos,int iCurrentZone = -1);

		// Delete
		void Delete(Entity* pkNewObject);								///< Adds an object to delete qeue
      void Delete(int iNetworkID);								      ///< Adds an object to delete qeue
		void UpdateDelete();													///< Deletes objects in delete qeue	

		// Updates
		void Update(int iType,int iSide,bool bSort);					///< Run update on selected propertys.
		bool IsUpdate(int iFlags);											///< Check if the current update is of a selected type
		void UpdateGameMessages(void);									///< Update game messages.
		
		// Gets
		Entity* GetWorldObject()	{	return m_pkWorldObject;				};
		Entity* GetZoneObject()		{	return m_pkZoneObject;					};		
		Entity* GetClientObject()	{	return m_pkClientObject;				};		
		Entity* GetGlobalObject()	{	return m_pkGlobalObject;				};				
		
		int	GetNextObjectID()		{	return iNextObjectID;					};
		int	GetNumOfObjects()		{	return m_akEntitys.size();			}
		int	GetActivePropertys() {	return m_iNrOfActivePropertys;		};
		void GetAllObjects(vector<Entity*> *pakObjects);
		Entity* GetObject(const char* acName);							///< Get a ptr to object by name
		Entity*	GetObjectByNetWorkID(int iNetID);					///< Get a ptr to object by networkID

		void GetAllObjectsInArea(vector<Entity*> *pkEntitys,Vector3 kPos,float fRadius);

		// NetWork
		void UpdateZoneList(NetPacket* pkNetPacket);
		void PackZoneListToClient(int iClient, set<int>& iZones );
		void UpdateState(NetPacket* pkNetPacket);						//Updates objects.
		void PackToClient(int iClient, vector<Entity*> kObjects,bool bZoneObject);
		void PackToClients();												//Packs and Sends to ALL clients.

		void StaticData(int iClient, NetPacket* pkNetPacket);
		void GetStaticData(int iEntityID);

		void OwnerShip_Request(Entity* pkObj);		// Use this to request ownership of a object.
		void OwnerShip_OnRequest(Entity* pkObj);	// Called when a request for ownership arrives on server.
		void OwnerShip_OnGrant(Entity* pkObj);		// Recv on client of he gets controll of a object from server.	
		void OwnerShip_Take(Entity* pkObj);		
		void OwnerShip_Give(Entity* pkObj);

		void ResetNetUpdateFlags(int iConID);

		// Debug / Help Functions		
		void DisplayTree();
		void DumpActiverPropertysToLog(char* szMsg);					///< Log all propertys in m_akPropertys.

		// Message System.
		void SendMsg(string strName, int iFrom, int iTo);
		void RouteMessage(GameMessage& Msg);

		// Get Strings.
		char* GetUpdateStatusName(int eStatus);				
		char* GetPropertyTypeName(int iType);
		char* GetPropertySideName(int iSide);

		Entity* EntityManager::CloneObject(int iNetID);

		//picking of objects
		bool TestLine(vector<Entity*>* pkObList,Vector3 kPos,Vector3 kVec);
		bool BoxVSBox(Vector3 kPos1,Vector3 kSize1,Vector3 kPos2,Vector3 kSize2);
      
		
		//zone system		
		void UpdateZoneSystem();
		void SetWorldDir(string strDir) {m_kWorldDirectory = strDir;};
		string GetWorldDir() { return m_kWorldDirectory; };
		bool SaveWorld(string strSaveDir,bool bForce=false);
		bool LoadWorld(string strLoadDir);

		ZoneData* GetZone(Vector3 kPos);
		void SetZoneModel(const char* szName,int iId);
		ZoneData*	GetZoneData(int iID);
		int GetZoneIndex(int iEntityId);
		int GetZoneIndex(Vector3 kMyPos,int iCurrentZone,bool bClosestZone);
		void DeleteZone(int iId);
		bool IsInsideZone(Vector3 kPos,Vector3 kSize);
		int CreateZone(Vector3 kPos,Vector3 kSize);
		void LoadZone(int iId,string strLoadDir = "");			//load zone
		void SaveZone(int iId,string strSaveDir = "");			//save zone
		int GetNumOfZones();
		int GetUnusedZoneID();		
		Vector3 GetZoneCenter(int iZoneNum);
		void SetTrackerLos(int iLos) { m_iTrackerLOS = iLos;};		
		int GetZoneIndex(Entity* PkObject,int iCurrentZone,bool bClosestZone);
		ZoneData* GetZone(Entity* PkObject);
		
		
		//trackers
		void AddTracker(P_Track* kObject);
		void RemoveTracker(P_Track* kObject);
		int GetNrOfTrackedObjects();
		list<P_Track*>* GetTrackerList();
		void ClearTrackers();
		vector<int>	GetActiveZoneIDs(int iTracker);	// Returns a list with zones that the tracked activates,
		

		friend class Entity;
		friend class ZeroFps;		
};

#endif

