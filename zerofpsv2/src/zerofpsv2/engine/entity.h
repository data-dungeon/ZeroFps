#ifndef _ENGINE_OBJECT_H_
#define _ENGINE_OBJECT_H_

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
struct	CollisionData;

enum UpdateStatus 
{
	UPDATE_NONE				= 1,	
	UPDATE_ALL				= 2,
	UPDATE_STATIC			= 4,
	UPDATE_DYNAMIC 		= 8,
};

enum ObjectType 
{
	OBJECT_TYPE_DYNAMIC,			// Full update, Full Collision
	OBJECT_TYPE_STATIC,	
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
	
};

/*
	Describe how a object works in network.
*/
enum NetWorkRole
{
	NETROLE_NONE,			// Object don't need to exist on remote nodes.
	NETROLE_PROXY,			// Object is a rep
	NETROLE_AUTHORITY,	// This is the master object.
};

/// GameMessages is messages that can be sent between objects in the game.
class GameMessage
{
public:
	int			m_FromObject;		///< ID of object that sent message.
	int			m_ToObject;			///< ID of target object.
	string		m_Name;				///< Name of GameMessage.
};

class ENGINE_API PropertyDescriptor{
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

#define MAX_NETUPDATEFLAGS	10

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





/**	\brief	Game Object for things in game 

A Object is the lowest level of something that exist in the gameworld. All things like
players, npcs, weapons, explosions, buildings and the ground are objects. Objects don't
have to be something visible that one can see, things like mission goals and triggers
are also objects. All objects are handled by the ObjectManger.

All objects in the game have somethings that are common for all of them. The are all
stored in the OM, the all have a type, a ID, a name and some other things. The things that make
each and every object diffrent are the type of properties they have. 
*/

class ENGINE_API Entity 
{
	private:
		Entity*						m_pkParent;							///< Parent Object. NULL If None
		vector<GameMessage>		m_kGameMessages;					///< Messages that are waiting to be handled by this object.
		vector<int>					m_aiNetDeleteList;				

		vector<EntityVariable>	m_kVariables;

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
			
		ZFResourceHandle*			m_pScriptFileHandle;				///< Handle to script used to create this object if any.
		
		/**	\brief	Object type name.
			
			This is the name of the type of object (defults to Object). For objects created from scripts this is
			set to the name of the script and for zfoh.txt it sets i to the name of the archobject.
			*/
		string						m_strType;							
		
		/**	\brief	Object Name.
			
			The name of the object. This is not unik so many objects can (and will) have the same name. The name will
			be set to 'A' + m_strType by defualt.
			*/
		string						m_strName;
		
	
		bool							m_bZone;
		bool							m_bSave;								///< True if this object should save to disk.
	
		int							m_iNetUpdateFlags;				///< Network flags for what needs to be updated to clients.					
		int							m_iUpdateStatus;					
		ObjectType					m_iObjectType;						
	
		// Rotation & Position.
		bool							m_bRelativeOri;					///< True if this object transform is in the frame of its parent.
		bitset<7>					m_kGotData;							

		Vector3						m_kLocalPosV;						///< Local position. important
		Vector3						m_kWorldPosV;						///< World position.

		Matrix4						m_kLocalRotM;						///< Local rotation important
		Matrix4						m_kWorldRotM;
		Matrix4						m_kWorldOriM;
		Matrix4						m_kLocalOriM;
		Quaternion					m_kWorldRotQ;
		Quaternion					m_kLocalRotQ;
		Vector3						m_kWorldRotV;
		Vector3						m_kLocalRotV;		

		Vector3						m_kILocalPosV;

		bool							m_bFirstSetPos;		

		Vector3						m_kVel;								///< Velocity of object.
		Vector3						m_kAcc;								///< Acc of object.
		float							m_fRadius;							///< Radius of object.

		bool							m_bUseZones;
		int							m_iCurrentZone;
		vector<Entity*>			m_akChilds;							///< List of child objects.
		vector<Property*>			m_akPropertys;						///< List of propertys of object.
		
		//network
		vector<bitset <MAX_NETUPDATEFLAGS> >	m_kNetUpdateFlags;
					
		Entity();				
		void	SetNetUpdateFlag(int iFlagID,bool bValue);
		void	SetNetUpdateFlagAndChilds(int iFlagID,bool bValue);		
		void	SetNetUpdateFlag(int iConID,int iFlagID,bool bValue);		
		bool	GetNetUpdateFlag(int iConID,int iFlagID);					
		bool	IsAnyNetUpdateFlagTrue(int iConID);					
		void	ResetAllNetUpdateFlags();											//reset all update flags to true
		void	ResetAllNetUpdateFlags(int iConID);											//reset all update flags to true		
		void	ResetAllNetUpdateFlagsAndChilds(int iConID);											//reset all update flags to true				
		void	SetNrOfConnections(int iConNR);
		
	public:
		bool							m_bHavePlayedSound;				///< Litet test bara...

		int							iNetWorkID;							///< ID used by network state code.
		EntityManager*				m_pkObjectMan;						///< Ptr to object manger.
  		ZeroFps*						m_pkFps;								///< Ptr to zerofps. 

	
		NetWorkRole					m_eRole;							///< This node power on object.
		NetWorkRole					m_eRemoteRole;						///< Remote node power on object.
		bool							m_bIsNetWork;

      string                  m_strCreatedFromScript; // which script the object was created from. used when splitting items

		~Entity();
		
		// Object Type Handling
		bool IsA(string strStringType);								///< Returns true if this object is based on type.

		Property* AddProperty(Property* pkNewProperty);			// Add a propyrty by ptr.
		Property* AddProperty(const char* acName);				// Create/Add a property by name.
		void RemoveProperty(Property* pkProp);						// Remove property by pointer.
		bool DeleteProperty(const char* acName);					// Remove property by name.
		void PropertyLost(Property* pkProp);

		Property* GetProperty(const char* acName);				// Returns property by name (first one only). 
		void GetPropertys(vector<Property*> *akPropertys,int iType,int iSide);			///< Get all propertys by flags.
		void GetAllPropertys(vector<Property*> *akPropertys,int iType,int iSide);		///< Used mainly for updates
		Property* AddProxyProperty(const char* acName);		///< Add a property if not exist.
		bool Update(const char* acName);							///< Run update on property 'name'.

		// Child/Parent object mangement.
		void AddChild(Entity* pkObject);							// Set a object to be child to this.	
		void RemoveChild(Entity* pkObject);						// Remove a child from this.
		void SetParent(Entity* pkObject);						// Set the parent of this object.
		Entity* GetParent(){return m_pkParent;};				///< Get parent of this object.
		bool HasChild(Entity* pkObject);							
		int NrOfChilds();												///< Return num of childs to this object.
		void DeleteAllChilds();										// Remove all childs from this object.
		void GetAllObjects(vector<Entity*> *pakObjects);			// Return this + all childs.
		void GetAllDynamicEntitys(vector<Entity*> *pakObjects);	// Return this + all childs.
		Entity* GetStaticEntity();
		
		void AddToDeleteList(int iId);
		void UpdateDeleteList();

		
		bool AttachToZone();		
		bool AttachToZone(Vector3 kPos);		

		bool GetUseZones() {return m_bUseZones;};
		void SetUseZones(bool bUz) {m_bUseZones = bUz;};		

		// NetWork/Demo/Save/Load Code.
		bool IsNetWork();															// True if this object has any netactive propertys.
		bool NeedToPack();														// Returns true if there is any netactive properys in object
		bool HaveSomethingToSend(int iConnectionID);						// Returns true if there is anything to send for selected connection,
		void PackTo(NetPacket* pkNetPacket, int iConnectionID);		// Pack Object.
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID);	// Unpack Object.

		

		void Load(ZFIoInterface* pkFile,bool bLoadID = true);
		void Save(ZFIoInterface* pkFile);

		void StaticData(int iClient, NetPacket* pkNetPacket);
		void GetStaticData(int iEntityID);

		// Collision / Shape.
		float GetBoundingRadius();									///< Get radius of collision object or radius 1.0 if none found.
		void Touch(Collision* pkCol);								///< Run touch on all properys of this object.
		void Touch(int iId);
		void ZoneChange(int iCurrent,int iNew);			//callback when object changes zone

		// Game Messages
		void	AddGameMessage(GameMessage& Msg);
		void	HandleMessages();
		void	RouteMessage(GameMessage& Msg);

		// Debug
		void PrintTree(int pos);									///< Debug: Prints object tree from object.
		bool CheckLinks(bool bCheckChilds, int iPos);		// Checks that parent/child links are ok. 

		// Orientation  
		bool			GetRelativeOri()				{	return m_bRelativeOri;		};
		void			SetRelativeOri(bool bRO);
		
		Matrix4		GetLocalRotM();					
		Matrix4		GetWorldRotM();
		Matrix4		GetWorldOriM();								//parent * rot * pos
		Matrix4		GetLocalOriM();
		Vector3		GetWorldRotV();
		Vector3		GetLocalRotV();		

		Vector3		GetLocalPosV();
		Vector3		GetWorldPosV();
		Vector3		GetIWorldPosV();
		
		//set oritentation data
		void			SetLocalRotM(Matrix4 kNewRot);
		void			SetLocalRotV(Vector3);
		void			SetLocalPosV(Vector3);
		void			SetWorldPosV(Vector3);
		void			SetWorldRotV(Vector3);
		void			RotateLocalRotV(Vector3 kRot);

		void			ResetGotData(){m_kGotData.reset();};
		void			ResetChildsGotData();

		void			SetVel(Vector3 kVel);
		void			SetName(string strName);
		void			SetType(string strType);		
		void			SetRadius(float fRadius);

		// Inlines
		inline int &GetUpdateStatus()				{	return m_iUpdateStatus;	};
		inline ObjectType &GetObjectType()		{	return m_iObjectType;	};
		inline bool& GetSave()						{	return m_bSave;			};
		inline string GetName()					{	return m_strName;			};
		inline string GetType()					{	return m_strType;			};
		inline ZFResourceHandle* GetObjectScript()  { return m_pScriptFileHandle;};
		
		inline Vector3 GetVel()					{	return m_kVel;				};		
		inline Vector3& GetAcc()					{	return m_kAcc;				};
		inline float GetRadius()					{	return m_fRadius;			};		
		inline Vector3* GetVelPointer()			{	return &m_kVel;			};		
		inline Vector3* GetAccPointer()			{	return &m_kAcc;			};
		inline float* GetRadiusPointer()			{	return &m_fRadius;		};		
		inline EntityManager *GetObjectMan()	{	return m_pkObjectMan;	};				

      void SetUpdateStatus(int iUpdateStatus);
		
		void MakeCloneOf(Entity* pkOrginal);
		
		float GetI();

		// Entity Variables
		EntityVariable* CreateVar(string& strName, EntityVariableType eType);
		EntityVariable* GetVar(string& strName);

		double GetVarDouble(string& strName);
		string GetVarString(string& strName);
		void	 SetVarDouble(string& strName, double fValue);
		void	 SetVarString(string& strName, string strValue);
		

		friend class EntityManager;
		friend class Property;

		// Force class to be polymorfic.
		virtual void DoNothing() {}
};

#endif





