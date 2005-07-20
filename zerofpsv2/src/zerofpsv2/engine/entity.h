#ifndef _ENGINE_ENTITY_H_
#define _ENGINE_ENTITY_H_

#pragma warning( disable : 4512) // 'class' : assignment operator could not be generated 

#include <vector>
#include <string.h>
#include <bitset>
#include "engine_x.h"
#include "network.h"
#include "../basic/quaternion.h"

using namespace std;

class		EntityManager;
class		Property;
class		PropertyFactory;

enum UpdateStatus 
{
	UPDATE_NONE				= 1,	
	UPDATE_ALL				= 2,
	UPDATE_NOCHILDS		= 4,	
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
	NETUPDATEFLAG_RELPOS =			8,		
	NETUPDATEFLAG_INTERPOLATE =	9,			
	NETUPDATEFLAG_ACC =				10,
	NETUPDATEFLAG_DELETEPROPLIST=	11,					
	NETUPDATEFLAG_ISZONE=			12,					
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
	string		m_strParameter;
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


#define MAX_NETUPDATEFLAGS	13
#define MAX_ENTITY_VARIABLENAME	64

enum EntityVariableType
{
	EVAR_DOUBLE,
	EVAR_STRING,
	EVAR_VECTOR,
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
		m_kVector   = kIn.m_kVector;
		return *this;
	}

	string					m_strName;		// Name of variable.
	EntityVariableType	m_eType;			 
	double					m_fValue;		// Value if double
	string					m_strValue;		// Value if string.
	Vector3					m_kVector;
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
		vector<string>				m_kNetDeletePropertyList;
		
		vector<EntityVariable>  m_kVariables;
		int							m_iEntityID;						///< Uniq ID for every entiy in the world
		float							m_fPriority;
		float							m_fLastSent;
		
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
		float							m_fInterPolateFactor;		
		unsigned int				m_iLastInterPolateFrame;
		
		Vector3						m_kVel;								///< Velocity of entity.
		Vector3						m_kAcc;								///< Acc of entity.
		float							m_fRadius;							///< Radius of entity.

		bool							m_bUseZones;
		int							m_iCurrentZone;
		vector<Entity*>			m_akChilds;							///< List of child entitys.
		vector<Property*>			m_akPropertys;						///< List of propertys of entity.
		
		//network
		vector<bitset<MAX_NETUPDATEFLAGS> >	m_kNetUpdateFlags;
		vector<bitset<MAX_NETUPDATEFLAGS> >	m_kNetIgnoreFlags;
		vector<bool>								m_kExistOnClient;
					
		Entity();		
		
		//network flags		
		void	SetNetUpdateFlag(int iFlagID,bool bValue);
		void	SetNetUpdateFlagAndChilds(int iFlagID,bool bValue);		
		void	SetNetUpdateFlag(int iConID,int iFlagID,bool bValue);		
		bool	GetNetUpdateFlag(int iConID,int iFlagID);					
		bool	IsAnyNetUpdateFlagTrue(int iConID);					
		void	ResetAllNetFlags();												// reset all update flags to true
		void	ResetAllNetFlags(int iConID);									// reset all update flags to true		
		void	ResetAllNetFlagsAndChilds(int iConID);						// reset all update flags to true				
		
		//called on creation
		void	SetNrOfConnections(int iConNR);

		//internal property handling
		void			AddToDeletePropertyList(const string& strName);
		void			UpdateDeletePropertyList();

		
		//property handling, or something
		void 	PropertyLost(Property* pkProp);
		
				
	public:

		EntityManager*				m_pkEntityManager;				///< Ptr to Entity manger.
  		ZeroFps*						m_pkZeroFps;						///< Ptr to zerofps. 

		NetWorkRole					m_eRole;								///< This node power on Entity.
		NetWorkRole					m_eRemoteRole;						///< Remote node power on Entity.
	
		~Entity();
		
		// Property Mangment
		Property* 	AddProperty(Property* pkNewProperty);					// Add a propyrty by ptr.
		void 			RemoveProperty(Property* pkProp);						// Remove property by pointer.
		
		Property* 	AddProperty(const char* acName);							// Create/Add a property by name.
		bool 			DeleteProperty(const char* acName);						// Remove property by name.
		Property* 	GetProperty(const char* acName);							// Returns property by name (first one only). 
		void 			GetPropertys(vector<Property*> *akPropertys,int iType,int iSide);			///< Get all propertys by flags.
		void 			GetAllPropertys(vector<Property*> *akPropertys,int iType,int iSide);		///< Used mainly for updates
		Property* 	AddProxyProperty(const char* acName);												///< Add a property if not exist.
		bool 			Update(const char* acName);															///< Run update on property 'name'.

		// Child/Parent Entity mangement.
		void 	AddChild(Entity* pkEntity);								// Set a Entity to be child to this.	
		void 	RemoveChild(Entity* pkEntity);							// Remove a child from this.
		void 	SetParent(Entity* pkEntity);								// Set the parent of this Entity.
		Entity* GetParent(){return m_pkParent;};						///< Get parent of this Entity.
		void	GetChilds(vector<Entity*>*	pkEntitys);
		bool 	HasChild(Entity* pkEntity);							
		int  	NrOfChilds();													///< Return num of childs to this Entity.
		void 	DeleteAllChilds();											// Remove all childs from this Entity.
		void 	GetAllEntitys(vector<Entity*> *pakEntitys ,bool bForceAll = false,bool bCheckSendStatus =false); // get all entitys + childs (bForceAll = dont care aout the obects update status
		


		bool AttachToZone();		
		bool AttachToZone(const Vector3& kPos);		
		bool GetZoneNeighbours(vector<Entity*>* pkZones);
		bool InActiveZone();
		
		// NetWork/Demo/Save/Load Code.
		bool IsNetWork();															// True if this Entity has any netactive propertys.
		bool HaveSomethingToSend(int iConnectionID);						// Returns true if there is anything to send for selected connection,
		void PackTo(NetPacket* pkNetPacket, int iConnectionID);		// Pack Entity.
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID);	// Unpack Entity.
		
		//exist on client flags
		void	SetExistOnClient(int iConID,bool bStatus);
		bool	GetExistOnClient(int iConID);

		//load/save
		void Load(ZFIoInterface* pkFile,bool bLoadID = true,bool bLoadChilds = true);
		void Save(ZFIoInterface* pkFile);

		// Collision / Shape.
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

		//interpolation
		void			SetInterpolate(bool bInterpolate);
		bool			GetInterpolate()						{	return m_bInterpolate;			};
		void			SetInterpolateFactor(float fIF)	{	m_fInterPolateFactor = fIF;	};	
				
		// Inlines & get/set functions
		inline bool IsZone() 								{	return m_bZone;					};
		inline int GetEntityID()							{	return m_iEntityID;				};
		inline int GetUpdateStatus()						{	return m_iUpdateStatus;			};
		inline bool GetSave()								{	return m_bSave;					};
		inline void SetSave(bool bSave)					{	m_bSave = bSave;					};
		inline string GetName()								{	return m_strName;					};
		inline string GetType()								{	return m_strType;					};
		inline bool IsType(const string& strType)		{	return m_strType == strType;	};	///< Returns true if this Entity is based on type.
		inline ZFResourceHandle* GetEntityScript()   {	return m_pScriptFileHandle;	};		
		inline Vector3 GetVel()								{	return m_kVel;						};		
		inline Vector3 GetAcc()								{	return m_kAcc;						};
		inline float GetRadius()							{	return m_fRadius;					};		
		inline int GetCurrentZone()  						{	return m_iCurrentZone;			};

		//network ignore flags
		void	SetNetIgnoreFlag(int iConID,int iFlagID,bool bValue)			{	m_kNetIgnoreFlags[iConID][iFlagID] = bValue;		};
		bool	GetNetIgnoreFlag(int iConID,int iFlagID)							{	return m_kNetIgnoreFlags[iConID][iFlagID];		};
		
		
		bool GetUseZones() 									{	return m_bUseZones;		};
		void SetUseZones(bool bUz) 						{	m_bUseZones = bUz;		};		
      void SetUpdateStatus(int iUpdateStatus);
		
		void Edit_GetDataNames(vector<string>& vkList);
		string Edit_GetDataString(const string& strName);
		void Edit_SetDataString(const string& strName, string strValue);

		// Entity Variables
		EntityVariable* CreateVar(const string& strName, EntityVariableType eType);
		EntityVariable* GetVar(const string& strName);
		void GetAllVarNames(vector<string>& vkList);
		double GetVarDouble(const string& strName);
		string GetVarString(const string& strName);
		Vector3 GetVarVector(const string& strName);
		void	 SetVarDouble(const string& strName, double fValue);
		void	 SetVarString(const string& strName, string strValue);		
		void	 SetVarVector(const string& strName, Vector3 kVec);		
		void	 AddVarDouble(const string& strName, double fValueToAdd);

		// Temporary Place For this Functions - Vim
		bool SendObjectClickEvent(const char* acType,int iCallerObject );		
		bool SendGroudClickEvent(const char* acType,Vector3 kPos,int iCallerObject);
	
		unsigned char				m_ucIcon;							// Icon, 0 = None.
		
		//friends
		friend class NetWork;
		friend class EntityManager;
		friend class Property;

		// Force class to be polymorfic.
		virtual void DoNothing() {}
};

void ENGINE_API Register_SIEntityProperty(ZeroFps* pkZeroFps);

#endif





