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


/// Data about a Zone in the World.
class ZoneData
{
public:

	bool					m_bNew;
	bool					m_bUsed;
	bool					m_bUnderContruction;	
	unsigned int		m_iRevision;		
	Entity*				m_pkZone;
	int					m_iZoneID;
	Vector3				m_kPos;
	Vector3				m_kSize;
	vector<int>			m_iZoneLinks;
	string				m_strEnviroment;

	float					m_fInactiveTime;
	bool					m_bActive;
	int					m_iRange;						// Range to tracker i num of zones.

	ZoneData& operator=(const ZoneData &kOther);
	bool IsInside(Vector3 kPoint);
	ZoneData();
};

/**	\brief	SubSystem that handles all objects in the world.
Handles the mangment of all objects that make up the game world. The OM are used to
create, update and destroy objects. It also handles to operation of replicating objects
to other connected users that need to now about them.
*/

class ENGINE_API EntityManager : public ZFSubSystem{
	private:
		enum FuncId_e
		{
			FID_LOGOHTREE,
			FID_LOGACTIVEPROPERTYS,
			FID_SENDMESSAGE,
			
			FID_LOADZONES,
			FID_SAVEZONE,			
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

		//base objects
		Entity*						m_pkWorldObject;											///< Top level object.
		Entity*						m_pkZoneObject;											///< Top level object.
		Entity*						m_pkClientObject;											///< Top level object.
		Entity*						m_pkGlobalObject;											///< Top level object.
		
		//current world directory to save/load zone data to 
		string						m_kWorldDirectory;
		
//		list<Entity*>				m_akObjects;												///< List of all objects.
		map<int,Entity*>			m_akEntitys;
		
		// DELETE
		vector<int>					m_aiDeleteList;
		
		// Zones
		vector<ZoneData>			m_kZones;
		
		// Trackers
		list<P_Track*>				m_kTrackedObjects;	
		int							m_iTrackerLOS;												//tracker line of sight

		//list<ObjectDescriptor*> m_akTemplates;											///< List of templates.
			
		vector<Property*>			m_akPropertys;												///< List of Active Propertys.	
		int							m_iNrOfActivePropertys;									///> Size of akProperty list.

		int							iNextObjectID;												///< Next free object ID.
		bool							m_bUpdate;													///< Disable all updates except RENDER.	
		bool							m_bDrawZones;
		bool							m_bDrawZoneConnections;

		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void GetPropertys(int iType,int iSide);						///< Fill propery list.
//		void TESTVIM_LoadArcheTypes(char* szFileName);

		NetPacket	m_OutNP;		// Used to create/send updates to clients.

	public:
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
		void UpdateDelete();													///< Deletes objects in delete qeue	

		// Updates
		void Update(int iType,int iSide,bool bSort);					///< Run update on selected propertys.
		bool IsUpdate(int iFlags);											///< Check if the current update is of a selected type
		void UpdateGameMessages(void);									///< Update game messages.
		void SetUpdate(bool bUpdate) { m_bUpdate=bUpdate; };		
		
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
		void	GetArchObjects(vector<string>* pkFiles, string strParentName);

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
		void SendMsg();
		void RouteMessage(GameMessage& Msg);

		// Get Strings.
		char* GetUpdateStatusName(int eStatus);				
		char* GetObjectTypeName(int eType);
		char* GetPropertyTypeName(int iType);
		char* GetPropertySideName(int iSide);

		Entity* EntityManager::CloneObject(int iNetID);

		//picking of objects
		bool TestLine(vector<Entity*>* pkObList,Vector3 kPos,Vector3 kVec);
		bool BoxVSBox(Vector3 kPos1,Vector3 kSize1,Vector3 kPos2,Vector3 kSize2);
      
		
		//zone system
		int GetNumOfZones();
		void Test_DrawZones();
		void UpdateZones();
		ZoneData* GetZone(Entity* PkObject);
		int GetZoneIndex(Entity* PkObject,int iCurrentZone,bool bClosestZone);
		int GetZoneIndex(Vector3 kMyPos,int iCurrentZone,bool bClosestZone);
		ZoneData* GetZone(Vector3 kPos);
		Vector3 GetZoneCenter(int iZoneNum);
		ZoneData*	GetZoneData(int iID);
		int CreateZone();
		int CreateZone(Vector3 kPos,Vector3 kSize);
		void DeleteZone(int iId);
		int GetUnusedZoneID();		
		void Zones_Refresh();
		void ClearZoneLinks(int iId);
		void SetZoneModel(const char* szName,int iId);
		bool IsInsideZone(Vector3 kPos,Vector3 kSize);
		void UpdateZoneLinks(int iId);
		bool NewWorld();		
		void SetUnderConstruction(int iId);
		void CommitZone(int iId);

		void SetWorldDir(const char* acDir) {m_kWorldDirectory = acDir;};
		bool LoadWorld(const char* acDir);
		void ForceUnload();					//forcing unload of all loaded zones
		void ForceSave();						//forcing save of all loaded zones
		bool LoadZones();						//load zone info list
		bool SaveZones();						//save zone info list
		void LoadZone(int iId);				//load zone
		void SaveZone(int iId);				//save zone
		void UnLoadZone(int iId);			//unload zone (saves and deletes)


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

