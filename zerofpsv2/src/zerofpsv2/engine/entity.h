#ifndef _ENGINE_ENTITY_H_
#define _ENGINE_ENTITY_H_

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include <vector>
#include <list>
#include <string.h>
#include <bitset>
#include "engine_x.h"
#include "property.h"
#include "propertyfactory.h"
#include "network.h"
#include "../basic/quaternion.h"
#include "../basic/zfvfs.h"
#include "../script/zfscript.h"

using namespace std;

class		EntityManager;
//struct	CollisionData;

enum UpdateStatus 
{
	UPDATE_NONE				= 1,	
	UPDATE_ALL				= 2,
	UPDATE_NOCHILDS		= 4,	
//	UPDATE_STATIC			= 4,		//not used
//	UPDATE_DYNAMIC 		= 8,		//not used
//	UPDATE_NOCHILDS		= 16,
};


enum NetUpdateFlags
{
	NETUPDATEFLAG_POS =				0,
	NETUPDATEFLAG_ROT =				1,
	NETUPDATEFLAG_VEL =				2,	
	NETUPDATEFLAG_PARENT =			3,	
	NETUPDATEFLAG_RADIUS =			4,	
	NETUPDATEFLAG_NAME =				5,		
	NETUPDATEFLAG_TYPE =				6,			
	NETUPDATEFLAG_UPDATESTATUS =	7,			
	NETUPDATEFLAG_DELETE =			8,		
	NETUPDATEFLAG_RELPOS =			9,		
	NETUPDATEFLAG_INTERPOLATE =	10,			
	NETUPDATEFLAG_ACC =				11,				
};

/*
	Describe how a entity works in network.
*/
enum NetWorkRole
{
	NETROLE_NONE,			// Entity don't need to exist on remote nodes.
	NETROLE_PROXY,			// Entity is a rep
	NETROLE_AUTHORITY,	// This is the master Entity.
};

/// GameMessages is messages that can be sent between Entity in the game.
class GameMessage
{
public:
	int			m_FromObject;		///< ID of Entity that sent message.
	int			m_ToObject;			///< ID of target Entity.
	string		m_Name;				///< Name of GameMessage.
};

class ENGINE_API PropertyDescriptor
{
	public:
		string m_kName;
		ZFMemPackage m_kData;
};

class ENGINE_API PropertyValue
{
public:
	PropertyValue();
	~PropertyValue();
	
	PropertyValue(const PropertyValue &Other);
	PropertyValue& operator=(const PropertyValue &Other);

	string	m_strVariable;
	string	m_strValue;
};


// Net Flags for Network updates
#define OBJ_NETFLAG_POS		1
#define OBJ_NETFLAG_ROT		2
#define OBJ_NETFLAG_DEL		4

#define MAX_NETUPDATEFLAGS	12

#define MAX_ENTITY_VARIABLENAME	64

enum EntityVariableType
{
	EVAR_DOUBLE,
	EVAR_STRING,
};

/**	\brief	A EntityVariable is a variable saved in a entity with a name and a value.

*/
class EntityVariable
{
public:
	EntityVariable& operator=(const EntityVariable& kIn)
	{
		m_strName	= kIn.m_strName;
		m_eType		= kIn.m_eType;
		m_fValue		= kIn.m_fValue;
		m_strValue	= kIn.m_strValue;
		return *this;
	}

	string					m_strName;		// Name of variable.
	EntityVariableType	m_eType;			 
	double					m_fValue;		// Value if double
	string					m_strValue;		// Value if string.
};


/**	\brief	Entity's for things in game 
	 \ingroup Engine

A entity is the lowest level of something that exist in the gameworld. All things like
players, npcs, weapons, explosions, buildings and the ground are entity's. Entity don't
have to be something visible that one can see, things like mission goals and triggers
are also entity's. All entity's are handled by the EntityManger.

All entity's in the game have somethings that are common for all of them. The are all
stored in the EM, the all have a type, a ID, a name and some other things. The things 
that make each and every Entity diffrent are the type of properties they have. 
*/
class ENGINE_API Entity 
{
	private:
		Entity*						m_pkParent;							///< Parent Entity. NULL If None
		vector<GameMessage>		m_kGameMessages;					///< Messages that are waiting to be handled by this Entity.
		vector<int>					m_aiNetDeleteList;				

		vector<EntityVariable>  m_kVariables;
		int							m_iEntityID;						///< Uniq ID for every entiy in the world

	protected:
		enum HAVE_DATA				//used in m_kGotData
		{
			WORLD_ROT_M,
			WORLD_ORI_M,
			LOCAL_ORI_M,
			WORLD_ROT_Q,	
			LOCAL_ROT_Q,
			WORLD_ROT_V,
			LOCAL_ROT_V,
			
			WORLD_POS_V,		
		};
	
		PropertyFactory*			m_pkPropertyFactory;				///< Ptr to property factory.			
		ZFResourceHandle*			m_pScriptFileHandle;				///< Handle to script used to create this entity if any.
		
		/**	\brief	Entity type name.
			
			This is the name of the type of Entity (defults to Entity). For entity's created from scripts this is
			set to the name of the script.
			*/
		string						m_strType;							
		
		/**	\brief	Entity Name.
			
			The name of the entity. This is not unik so many entity can (and will) have the same name. The name will
			be set to 'A' + m_strType by defualt.
			*/
		string						m_strName;
		
	
		bool							m_bZone;								///< True if this entity is a Zone.
		bool							m_bSave;								///< True if this entity should save to disk.
	
		bool							m_bSendChilds;						// Should childs be sent to clients?
		int							m_iUpdateStatus;					
	
		// Rotation & Position.
		bool							m_bRelativeOri;					///< True if this entity transform is in the frame of its parent.
		bitset<8>					m_kGotOrientationData;							

		/*	This is the Position and rotation of the Entity. It is stored as a Vector for position and a Matrix for
			rotation. Local contains the Entitys transform and World is to total transform including any parent. */
		Vector3						m_kLocalPosV;						///< Local position. important
		Matrix3						m_kLocalRotM;						///< Local rotation important
		Vector3						m_kWorldPosV;						///< World position.
		Matrix3						m_kWorldRotM;						///< World rotation
	
		Matrix4						m_kWorldOriM;
		Matrix4						m_kLocalOriM;
		Quaternion					m_kWorldRotQ;
		Quaternion					m_kLocalRotQ;
		Vector3						m_kWorldRotV;
		Vector3						m_kLocalRotV;		

		Vector3						m_kILocalPosV;

		bool							m_bFirstSetPos;		
		bool							m_bInterpolate;					//if this is false, GetIWorldPos vill return non inpterpolatet cordinats

		Vector3						m_kVel;								///< Velocity of entity.
		Vector3						m_kAcc;								///< Acc of entity.
		float							m_fRadius;							///< Radius of entity.

		bool							m_bUseZones;
		int							m_iCurrentZone;
		vector<Entity*>			m_akChilds;							///< List of child entitys.
		vector<Property*>			m_akPropertys;						///< List of propertys of entity.
		
		//network
		vector<bitset <MAX_NETUPDATEFLAGS> >	m_kNetUpdateFlags;
					
		Entity();				
		void	SetNetUpdateFlag(int iFlagID,bool bValue);
		void	SetNetUpdateFlagAndChilds(int iFlagID,bool bValue);		
		void	SetNetUpdateFlag(int iConID,int iFlagID,bool bValue);		
		bool	GetNetUpdateFlag(int iConID,int iFlagID);					
		bool	IsAnyNetUpdateFlagTrue(int iConID);					
		void	ResetAllNetUpdateFlags();												// reset all update flags to true
		void	ResetAllNetUpdateFlags(int iConID);									// reset all update flags to true		
		void	ResetAllNetUpdateFlagsAndChilds(int iConID);						// reset all update flags to true				
		void	SetNrOfConnections(int iConNR);
		
	public:

		EntityManager*				m_pkEntityManager;				///< Ptr to Entity manger.
  		ZeroFps*						m_pkZeroFps;						///< Ptr to zerofps. 

		NetWorkRole					m_eRole;								///< This node power on Entity.
		NetWorkRole					m_eRemoteRole;						///< Remote node power on Entity.
      string                  m_strCreatedFromScript;			// which script the Entity was created from. used when splitting items
	
		~Entity();
		
		// Entity Type Handling
		bool IsA(string strStringType);									///< Returns true if this Entity is based on type.

		// Property Mangment
		Property* AddProperty(Property* pkNewProperty);				// Add a propyrty by ptr.
		Property* AddProperty(const char* acName);					// Create/Add a property by name.
		void 	RemoveProperty(Property* pkProp);						// Remove property by pointer.
		bool 	DeleteProperty(const char* acName);						// Remove property by name.
		void 	PropertyLost(Property* pkProp);
		Property* GetProperty(const char* acName);					// Returns property by name (first one only). 
		void 	GetPropertys(vector<Property*> *akPropertys,int iType,int iSide);			///< Get all propertys by flags.
		void 	GetAllPropertys(vector<Property*> *akPropertys,int iType,int iSide);		///< Used mainly for updates
		Property* AddProxyProperty(const char* acName);				///< Add a property if not exist.
		bool 	Update(const char* acName);								///< Run update on property 'name'.

		// Child/Parent Entity mangement.
		void 	AddChild(Entity* pkEntity);								// Set a Entity to be child to this.	
		void 	RemoveChild(Entity* pkEntity);							// Remove a child from this.
		void 	SetParent(Entity* pkEntity);								// Set the parent of this Entity.
		Entity* GetParent(){return m_pkParent;};						///< Get parent of this Entity.
		bool 	HasChild(Entity* pkEntity);							
		int  	NrOfChilds();													///< Return num of childs to this Entity.
		void 	DeleteAllChilds();											// Remove all childs from this Entity.
		void 	GetAllEntitys(vector<Entity*> *pakEntitys ,bool bForceAll = false,bool bCheckSendStatus =false); // get all entitys + childs (bForceAll = dont care aout the obects update status
		
		void 	AddToDeleteList(int iId);
		void 	UpdateDeleteList();


		bool AttachToZone();		
		bool AttachToZone(const Vector3& kPos);		
		bool GetZoneNeighbours(vector<Entity*>* pkZones);
		
		// NetWork/Demo/Save/Load Code.
		bool IsNetWork();															// True if this Entity has any netactive propertys.
		bool HaveSomethingToSend(int iConnectionID);						// Returns true if there is anything to send for selected connection,
		void PackTo(NetPacket* pkNetPacket, int iConnectionID);		// Pack Entity.
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID);	// Unpack Entity.

		

		void Load(ZFIoInterface* pkFile,bool bLoadID = true,bool bLoadChilds = true);
		void Save(ZFIoInterface* pkFile);

		void StaticData(int iClient, NetPacket* pkNetPacket);
		void GetStaticData(int iEntityID);

		// Collision / Shape.
		void Touch(Collision* pkCol);								///< Run touch on all properys of this Entity.
		void Touch(int iId);
		void ZoneChange(int iCurrent,int iNew);				//callback when Entity changes zone

		// Game Messages
		void	AddGameMessage(GameMessage& Msg);
		void	HandleMessages();
		void	PublishEvent(GameMessage& Msg);

		// Debug
		void PrintTree(int pos);									///< Debug: Prints Entity tree from Entity.
		bool CheckLinks(bool bCheckChilds, int iPos);		// Checks that parent/child links are ok. 

		// Orientation  
		bool			GetRelativeOri()				{	return m_bRelativeOri;		};
		void			SetRelativeOri(bool bRO);
		
		Matrix3		GetLocalRotM();					
		Matrix3		GetWorldRotM();
		Matrix4		GetWorldOriM();								//parent * rot * pos
		Matrix4		GetLocalOriM();
		Vector3		GetWorldRotV();
		Vector3		GetLocalRotV();		

		Vector3		GetLocalPosV();
		Vector3		GetWorldPosV();
		Vector3		GetIWorldPosV();
		
		//set oritentation data
		void			SetLocalRotM(const Matrix3& kNewRot);
		void			SetLocalRotM(const Matrix4& kNewRot);
		void			SetLocalRotV(const Vector3&);
		void			SetLocalPosV(const Vector3&);
		void			SetWorldPosV(const Vector3&);
		void			SetWorldRotV(Vector3 kRot);
		void			RotateLocalRotV(const Vector3& kRot);

		void			ResetGotData()							{	m_kGotOrientationData.reset();	}
		void			ResetChildsGotData();

		void			SetVel(const Vector3& kVel);
		void			SetAcc(const Vector3& kAcc);		
		void			SetName(string strName);
		void			SetType(string strType);		
		void			SetRadius(float fRadius);

		void			SetInterpolate(bool bInterpolate);
		
		// Inlines & get/set functions
		inline bool IsZone() 								{	return m_bZone;			};
		inline int GetEntityID()							{	return m_iEntityID;		};
		inline int GetUpdateStatus()						{	return m_iUpdateStatus;	};
		inline bool& GetSave()								{	return m_bSave;			};
		inline string GetName()								{	return m_strName;			};
		inline string GetType()								{	return m_strType;			};
		inline ZFResourceHandle* GetEntityScript()   {	return m_pScriptFileHandle;};
		
		inline Vector3 GetVel()								{	return m_kVel;				};		
		inline Vector3 GetAcc()								{	return m_kAcc;				};
		inline float GetRadius()							{	return m_fRadius;			};		
		inline Vector3* GetVelPointer()					{	return &m_kVel;			};		
		inline Vector3* GetAccPointer()					{	return &m_kAcc;			};
		inline float* GetRadiusPointer()					{	return &m_fRadius;		};		
		inline int GetCurrentZone()  						{	return m_iCurrentZone;	};

		bool GetUseZones() 									{	return m_bUseZones;		};
		void SetUseZones(bool bUz) 						{	m_bUseZones = bUz;		};		
      void SetUpdateStatus(int iUpdateStatus);
		

		// Entity Variables
		EntityVariable* CreateVar(const string& strName, EntityVariableType eType);
		EntityVariable* GetVar(const string& strName);
		void GetAllVarNames(vector<string>& vkList);
		double GetVarDouble(const string& strName);
		string GetVarString(const string& strName);
		void	 SetVarDouble(const string& strName, double fValue);
		void	 SetVarString(const string& strName, string strValue);		
		void	 AddVarDouble(const string& strName, double fValueToAdd);

		
		//friends
		friend class NetWork;
		friend class EntityManager;
		friend class Property;

		// Force class to be polymorfic.
		virtual void DoNothing() {}
};

#endif





