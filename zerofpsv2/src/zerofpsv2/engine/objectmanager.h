#ifndef _ENGINE_THE_OBJECTMANAGER_H_
#define _ENGINE_THE_OBJECTMANAGER_H_

#include "object.h"
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
class TrackProperty;


/// Data about a Zone in the World.
class ZoneData
{
public:

	bool					m_bNew;
	bool					m_bUsed;
	Object*				m_pkZone;
	int					m_iZoneID;
	Vector3				m_kPos;
	Vector3				m_kSize;
	vector<int>			m_iZoneLinks;

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

class ENGINE_API ObjectManager : public ZFSubSystem{
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
		Object*						m_pkWorldObject;											///< Top level object.
		Object*						m_pkZoneObject;											///< Top level object.
		Object*						m_pkClientObject;											///< Top level object.
		Object*						m_pkGlobalObject;											///< Top level object.
		
		//current world directory to save/load zone data to 
		string						m_kWorldDirectory;
		
		// Object ArcheTypes
		list<ObjectArcheType*>	m_akArcheTypes;						///< List of all object Archetypes.

		list<Object*>				m_akObjects;									///< List of all objects.
		
		// DELETE
		vector<int>					m_aiDeleteList;
//		vector<int>					m_aiNetDeleteList;
		
		// Zones
		vector<ZoneData>			m_kZones;
		
		// Trackers
		list<Object*> 				m_kTrackedObjects;	
		int							m_iTrackerLOS;					//tracker line of sight

		list<ObjectDescriptor*> m_akTemplates;							///< List of templates.
		
		vector<Property*>			m_akPropertys;									///< List of Active Propertys.	
		int							m_iNrOfActivePropertys;						///> Size of akProperty list.

		int							iNextObjectID;													///< Next free object ID.
		bool							m_bUpdate;														///< Disable all updates except RENDER.	
		bool							m_bDrawZones;

		void RunCommand(int cmdid, const CmdArgument* kCommand);
		void GetPropertys(int iType,int iSide);						///< Fill propery list.
		void TESTVIM_LoadArcheTypes(char* szFileName);
		ObjectArcheType* GetArcheType(string strName);				///< Get ptr to AT. NULL if not found.
		void AddArchPropertys(Object* pkObj, string strName);


	public:
		int		m_iForceNetUpdate;					
		float		m_fEndTimeForceNet;

		int		m_iUpdateFlags;												///< Flags of active update.
		int		m_iRole;															///< I'm i a server or a client or both.

		int		m_iTotalNetObjectData;
		int		m_iNumOfNetObjects;

		ObjectManager();
		~ObjectManager();
	
		// Add/Remove Objects
		void Link(Object* pkNewObject);									///< Link this to the Object manager
		void UnLink(Object* pkObject);									///< UnLink this from Object Manger.
		void Clear();															///< Delete all objects.
		void CreateBaseObjects();											/// create all base objects	

		// Create 
		Object* CreateObject();												///< Create a empty object.
//		Object* CreateObject(const char* acName);						///< Create object from template.
		Object* CreateObjectByNetWorkID(int iNetID);					///< Create object with selected NetworkID
		Object* CreateObjectByArchType(const char* acName);		///< Create object from archtype
		Object* CreateObjectFromScript(const char* acName);
		Object* CreateObjectFromScriptInZone(const char* acName,Vector3 kPos,int iCurrentZone = -1);

		// Delete
		void Delete(Object* pkNewObject);								///< Adds an object to delete qeue
		void UpdateDelete();													///< Deletes objects in delete qeue	
//		void UpdateDeleteList(NetPacket* pkNetPacket);

		// Updates
		void Update(int iType,int iSide,bool bSort);					///< Run update on selected propertys.
		void UpdateGameMessages(void);									///< Update game messages.
		void SetUpdate(bool bUpdate) { m_bUpdate=bUpdate; };		


		// Arch types
		bool IsA(Object* pkObj, string strStringType);

		// Gets
		Object* GetWorldObject()	{	return m_pkWorldObject;				};
		Object* GetZoneObject()		{	return m_pkZoneObject;					};		
		Object* GetClientObject()	{	return m_pkClientObject;				};		
		Object* GetGlobalObject()	{	return m_pkGlobalObject;				};				
		
		int	GetNumOfObjects()		{	return m_akObjects.size();			}
		int	GetActivePropertys() {	return m_iNrOfActivePropertys;	};
		void GetAllObjects(vector<Object*> *pakObjects);
		Object* GetObject(const char* acName);							///< Get a ptr to object by name
		Object*	GetObjectByNetWorkID(int iNetID);					///< Get a ptr to object by networkID
		void	GetArchObjects(vector<string>* pkFiles, string strParentName);

		// NetWork
		void UpdateZoneList(NetPacket* pkNetPacket);
		void PackZoneListToClient(int iClient, set<int>& iZones /* TrackProperty* pkTrack*/ );
		void UpdateState(NetPacket* pkNetPacket);						//Updates objects.
		void PackToClient(int iClient, vector<Object*> kObjects);
		void PackToClients();												//Packs and Sends to ALL clients.

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

		Object* ObjectManager::CloneObject(int iNetID);

		//picking of objects
		bool TestLine(vector<Object*>* pkObList,Vector3 kPos,Vector3 kVec);

		void OwnerShip_Request(Object* pkObj);		// Use this to request ownership of a object.
		void OwnerShip_OnRequest(Object* pkObj);	// Called when a request for ownership arrives on server.
		void OwnerShip_OnGrant(Object* pkObj);		// Recv on client of he gets controll of a object from server.	
		void OwnerShip_Take(Object* pkObj);		
		void OwnerShip_Give(Object* pkObj);
		
		bool StartUp();
		bool ShutDown();
		bool IsValid();

		//zone system
		int GetNumOfZones();
		void Test_CreateZones();
		void Test_DrawZones();
		void UpdateZones();
		ZoneData* GetZone(Object* PkObject);
		int GetZoneIndex(Object* PkObject,int iCurrentZone,bool bClosestZone);
		int GetZoneIndex(Vector3 kMyPos,int iCurrentZone,bool bClosestZone);

		ZoneData* GetZone(Vector3 kPos);
		void AutoConnectZones();
		Vector3 GetZoneCenter(int iZoneNum);
	
		//trackers
		void AddTracker(Object* kObject);
		void RemoveTracker(Object* kObject);
		int GetNrOfTrackedObjects();
		list<Object*>* GetTrackerList();
		void ClearTrackers();
		vector<int>	GetActiveZoneIDs(int iTracker);	// Returns a list with zones that the tracked activates,
		
		
		void SetWorldDir(const char* acDir) {m_kWorldDirectory = acDir;};
		bool LoadWorld(const char* acDir);
				
		void ForceUnload();					//forcing unload of all loaded zones
		void ForceSave();						//forcing save of all loaded zones
		
		bool LoadZones();						//load zone info list
		bool SaveZones();						//save zone info list
		void LoadZone(int iId);				//load zone
		void SaveZone(int iId);				//save zone
		void UnLoadZone(int iId);			//unload zone (saves and deletes)
		
		ZoneData*	GetZoneData(int iID);
		int CreateZone();
		int CreateZone(Vector3 kPos,Vector3 kSize);
		void DeleteZone(int iId);
		int GetUnusedZoneID();		
		bool NewWorld();		
		void Zones_Refresh();
		void ClearZoneLinks(int iId);

		void SetZoneModel(const char* szName,int iId);
	
		bool IsInsideZone(Vector3 kPos,Vector3 kSize);
		void UpdateZoneLinks(int iId);

		//box vs vox test
		bool BoxVSBox(Vector3 kPos1,Vector3 kSize1,Vector3 kPos2,Vector3 kSize2);

		friend class Object;
		friend class ZeroFps;		
};

#endif


//		void AddTracker(Object* kObject);
//		void RemoveTracker(Object* kObject);
//		int GetNrOfTrackedObjects();
//		list<Object*>* GetTrackerList();
//		void ClearTrackers();

//		void UpdateZones();
//		void EnableZone(int xp,int zp,Vector3 &kPos);
//		Object* GetClosestZone(Vector3 &kPos);

//		void DrawZones();




