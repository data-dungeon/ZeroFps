#ifndef _ENGINE_THE_ENTITYMANAGER_H_
#define _ENGINE_THE_ENTITYMANAGER_H_

#include "entity.h"
#include "property.h"
#include <vector>
#include <queue>
#include <list>
#include <set>
#include "../script/zfscript.h"
#include "propertyfactory.h"

using namespace std;

class ZSSZeroFps;
class GameMessage;
class ZSSNetWork;
class P_Track;

enum EZoneStatus
{
	EZS_LOADED,
	EZS_UNLOADED,
	EZS_CACHED,
	EZS_UNUSED,
};

/// Data about a Zone in the World.
class ENGINE_API ZoneData
{
public:

	unsigned int		m_iStatus;					//current status of zone
	int					m_iVersion;
	bool					m_bNew;
	Entity*				m_pkZone;
	int					m_iZoneObjectID;
	int					m_iZoneID;
	Vector3				m_kPos;
	Vector3				m_kSize;
	Vector3				m_kTestPos; // zones with odd sizes must be offseted in some tests
	vector<int>			m_iZoneLinks;
	string				m_strEnviroment;

	float					m_fInactiveTime;
	bool					m_bTracked;					//is there a tracker touching this zone
	bool					m_bActive;
	int					m_iRange;						// Range to tracker i num of zones.
	
	bool IsInside(const Vector3& kPoint);
	ZoneData();
	void Clear();
};



/**	\brief	SubSystem that handles all entity's in the world.
	 \ingroup Engine

Handles the mangment of all entity's that make up the game world. The EM are used to
create, update and destroy entity's. It also handles to operation of replicating entity's
to other connected users that need to now about them.
*/

class ENGINE_API ZSSEntityManager : public ZFSubSystem
{
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

		//sort funktion for property sorting
		struct Property_Less : public binary_function<Property*, Property*, bool> 
		{
			bool operator()(Property* x, Property* y) { return *x < *y; };
		} Less_Property;
	
		
		//some system pointers
		ZSSZeroFps*					m_pkZeroFps;
		ZSSNetWork*				m_pkNetWork;
		ZSSBasicFS*				m_pkBasicFS;
		ZShaderSystem*			m_pkZShaderSystem;
		ZSSRender*				m_pkRender;
		ZSSPropertyFactory*	m_pkPropertyFactory;	
		
		// Base Entitys
		Entity*					m_pkWorldEntity;											///< Top level entity.
		Entity*					m_pkZoneEntity;											///< Top level entity.
		Entity*					m_pkClientEntity;											///< Top level entity.
		Entity*					m_pkGlobalEntity;											///< Top level entity.
		
		//current world directory to save/load zone data to 
		string					m_kWorldDirectory;
		string					m_strLoadDirectory;		
		
		vector<Property*>		m_akPropertys;												///< List of Active Propertys.	
		int						m_iNormalUpdates;
		int						m_iRenderUpdates;
		
		//entity stuff
		map<int,Entity*>		m_akEntitys;									//hash map of entitys, for fast entity trough ID access				
		int						m_iNextEntityID;								///< Next free Entity ID.		
		vector<int>				m_aiDeleteList;								// contains object that are going to be deleted
		vector<Property*>		m_kPropertyDeleteList;		
						
		// Network 	
		float						m_fEndTimeForceNet;
		int						m_iTotalNetEntityData;
		int						m_iNumOfNetEntitys;
		NetPacket				m_OutNP;							// Used to create/send updates to clients.		
		

		//other strange variables =D
		int						m_iUpdateFlags;												///< Flags of active update.
		int						m_iRole;															///< I'm i a server or a client or both.

		// Zones
		vector<ZoneData>		m_kZones;
		
		//debug
		bool						m_bDrawZones;						//shuld zones be drawed
		bool						m_bDrawEnviroments;
		bool						m_bDrawZoneConnections;			//shuld connection betwen zones be drawed
		
		// sim time system
		float		m_fSimTime;				// Time in the virtual world.
		float		m_fSimTimeScale;		// How fast does the time in the sim run with respect to the real world time. 		
		float		m_fSimTimeDelta;
		
				
		// Trackers
		list<P_Track*>				m_kTrackedObjects;	
		int							m_iTrackerLOS;												//tracker line of sight
		float							m_iObjectDistance;											//tracker line of sight		
		float							m_fZoneUnloadTime;
		int							m_iMaxZoneIO;
			

		
		void RunCommand(int cmdid, const CmdArgument* kCommand);

		//network
		void ClearClientDeleteQueue(int iClient);
		void ClearClientsDeleteQueues();		
		void SendDeleteQueue(int iClient);
		
		//check for unloaded zones
		void DeleteUnloadedZones(int iClient);
		
		//private zone system functions
		void UpdateTrackers();
		void UpdateZoneStatus();		
		
		void GetZones(set<int>* kZones,int iZone,Vector3 kPos,float fRadius);
		void ClearZoneLinks(int iId);
		void UpdateZoneLinks(int iId);
		
		void ForceUnload();												//forcing unload of all loaded zones
		void ForceSave();													//forcing save of all loaded zones
		bool LoadZones(string strSaveDir ="");						//load zone info list
		bool SaveZones(string strSaveDir ="");						//save zone info list
		bool SaveTrackers(string strSaveDir = "");
		bool SaveGlobalEntitys(string strSaveDir = "");
		bool LoadGlobalEntitys(string strSaveDir = "");
		bool LoadTrackers(string strSaveDir = "");
		void UnLoadZone(int iId);										//unload zone (saves and deletes)
		
		//system time 
		void	UpdateSimTime();											//this is called every system update, to calculate the new sim time
				
		int	m_iSendType;												// Packet used to send entitymanger data.

	public:
		set<string>				m_kEditorHide;													///< List of entity types hidden when in editor mode.		

		ZSSScriptSystem* 		m_pkScript;
		
		bool						m_bAllowHide;

		ZSSEntityManager();
		~ZSSEntityManager();
		bool StartUp();
		bool ShutDown();
		bool IsValid();
	
		//system time
		float		GetSimTime()						{	return m_fSimTime; 			};
		float		GetSimDelta()						{	return m_fSimTimeDelta;		};
		void		SetTimeScale(float fScale)		{	m_fSimTimeScale = fScale;	};
		float		GetTimeScale()						{	return m_fSimTimeScale;		};
		
		// Add/Remove Entitys
		void Link(Entity* pkNewEntity,int iId = -1);					///< Link this to the Entity manager
		void UnLink(Entity* pkEntity);									///< UnLink this from Entity Manger.
		bool IsLinked(Entity* pkEntity);
		void Clear();															///< Delete all Entity.
		void CreateBaseEntitys();											/// create all base Entitys	


		// Create 
		Entity* CreateEntity(bool bLink = true);						///< Create a empty Entity.
		Entity* CreateEntityByNetWorkID(int iNetID);					///< Create Entity with selected NetworkID
		Entity* CreateEntityFromScript(const char* acName);
		Entity* CreateEntityFromScriptInZone(const char* acName,Vector3 kPos,int iCurrentZone = -1);

		// Delete
		void Delete(Entity* pkEntity);									///< Adds an Entity to delete qeue
      void Delete(int iNetworkID);								      ///< Adds an Entity to delete qeue
		void UpdateDelete();													///< Deletes Entity in delete qeue	

		// Updates
		void 	Update(int iType,int iSide,bool bSort,Entity* pkRootEntity = NULL,bool bForceRootOnly = false,bool bUpdateList = true);				///< Run update on selected propertys.
		bool 	IsUpdate(int iFlags);											///< Check if the current update is of a selected type
		void 	UpdateGameMessages(void);									///< Update game messages.
		int 	GetCurrentUpdateFlags()	{	return m_iUpdateFlags;	};
		
		// Gets
		Entity* GetWorldEntity()	{	return m_pkWorldEntity;				};
		Entity* GetZoneEntity()		{	return m_pkZoneEntity;				};		
		Entity* GetClientEntity()	{	return m_pkClientEntity;			};		
		Entity* GetGlobalEntity()	{	return m_pkGlobalEntity;			};				
		
		int	GetNextEntityID()		{	return m_iNextEntityID;				};
		int	GetNumOfEntitys()		{	return int(m_akEntitys.size());	}
		
		int	GetNormalUpdates() 			{	return m_iNormalUpdates;	}
		int	GetRenderUpdates() 			{	return m_iRenderUpdates;	}
		void  ResetUpdateCounters()		{	m_iNormalUpdates = 0;
														m_iRenderUpdates = 0;		}
		
		void 		GetAllEntitys(vector<Entity*> *pakEntity);
		Entity*	GetEntityByType(const char* czType);
		Entity* 	GetEntityByName(const char* acName);							///< Get a ptr to Entity by name
		Entity*	GetEntityByID(int iNetID);						///< Get a ptr to Entity by networkID

		Property* GetPropertyFromEntityID(int iEntityID,const char* czPropertyName);
		
		void GetAllEntitysInArea(vector<Entity*> *pkEntitys,Vector3 kPos,float fRadius);

		
		// NetWork
		void UpdateZoneList(NetPacket* pkNetPacket);
		void UpdateState(NetPacket* pkNetPacket);												//Updates Entity.
		void HandleDeleteQueue(NetPacket* pkNetPacket);		
		void PackEntityToClient(int iClient, vector<Entity*>& kEntitys,bool bZoneObject,int iSendSize,Entity* pkReferens);
		void PackToClients();																		//Packs and Sends to ALL clients.
		
		void UpdatePriority(vector<Entity*>& kEntitys,Entity* pkReferens,int iClientID);
		Entity* GetTopPriorityEntity(vector<Entity*>& kObjects,int iClientID);

		void OwnerShip_Request(Entity* pkObj);		// Use this to request ownership of a Entity.
		void OwnerShip_OnRequest(Entity* pkObj);	// Called when a request for ownership arrives on server.
		void OwnerShip_OnGrant(Entity* pkObj);		// Recv on client of he gets controll of a Entity from server.	
		void OwnerShip_Take(Entity* pkObj);		
		void OwnerShip_Give(Entity* pkObj);

		void ResetNetUpdateFlags(int iConID);

		//network delete
		void AddEntityToClientDeleteQueue(int iClient,int iEntityID);
		void AddEntityToAllClientDeleteQueues(int iEntityID);		
		
		// Debug / Help Functions		
		void DisplayTree();
		void DumpActiverPropertysToLog(char* szMsg);					///< Log all propertys in m_akPropertys.
		void DrawZones();
		void DrawZones(const vector<ZoneData>* pkZoneList);
		void DrawSceneGraph();
		
		// Message System.
		void SendMsg(string strName, string strParam, int iFrom, int iTo);
		void RouteMessage(GameMessage& Msg);

		// Get Strings.
		char* GetUpdateStatusName(int eStatus);				
		char* GetPropertyTypeName(int iType);
		char* GetPropertySideName(int iSide);

		Entity* CloneEntity(int iNetID);

		//picking of Entitys
		bool TestLine(vector<Entity*>* pkObList,Vector3 kPos,Vector3 kVec);
		bool BoxVSBox(Vector3 kPos1,Vector3 kSize1,Vector3 kPos2,Vector3 kSize2);
      
		
		//zone system		
		void 		UpdateZoneSystem();
		void 		SetWorldDir(string strDir) 							{	m_kWorldDirectory = strDir;	};
		string 	GetWorldDir() 												{	return m_kWorldDirectory;		};
		string	GetLoadDir()												{  return m_strLoadDirectory;   };
		bool 		SaveWorld(string strSaveDir,bool bForce=false);
		bool 		LoadWorld(string strLoadDir);

		ZoneData* 	GetZone(const Vector3& kPos);
		void 			SetZoneModel(const char* szName,int iId);
		ZoneData*	GetZoneData(int iID);
		int 			GetZoneIndex(int iEntityId);
		int 			GetZoneIndex(const Vector3& kMyPos,int iCurrentZone,bool bClosestZone);
		void 			DeleteZone(int iId);
		bool 			IsInsideZone(const Vector3& kPos,const Vector3& kSize);
		bool 			ZoneHaveNeighbour(const Vector3& kPos,const Vector3& kSize);
		int 			CreateZone(const Vector3& kPos,const Vector3& kSize);
		void 			LoadZone(int iId,string strLoadDir = "");			//load zone
		void 			SaveZone(int iId,string strSaveDir = "");			//save zone
		int 			GetNumOfZones();
		int 			GetUnusedZoneID();		
		Vector3 		GetZoneCenter(int iZoneNum);
		void 			SetTrackerLos(int iLos) 							{	m_iTrackerLOS = iLos;			};		
		ZoneData* 	GetZone(Entity* PkEntity);
		void			SetMaxZoneIO(int iMaxIO)							{	m_iMaxZoneIO = iMaxIO;			};
		
		void			RecreateEntitys(const string& strType);

		
		//trackers
		void 					AddTracker(P_Track* kObject);
		void 					RemoveTracker(P_Track* kObject);
		int 					GetNrOfTrackedObjects();
		list<P_Track*>* 	GetTrackerList();
		void 					ClearTrackers();
		vector<int>			GetActiveZoneIDs(int iTracker);	// Returns a list with zones that the tracked activates,
		
		bool CallFunction(Entity* pkEntity, const char* acFunction,vector<ScriptFuncArg>* pkParams = NULL);

		friend class Entity;
		friend class ZSSZeroFps;		
};

#endif

