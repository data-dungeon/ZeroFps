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

class		ObjectManager;
struct	CollisionData;

enum UpdateStatus 
{
	UPDATE_NONE				= 1,	
	UPDATE_ALL				= 2,
	UPDATE_STATIC			= 4,
	UPDATE_DYNAMIC 		= 8,
	UPDATE_PLAYERS 		= 16,
	UPDATE_STATDYN 		= 32,
	UPDATE_DECORATION		= 64,
	UPDATE_LIGHT			= 128,
};

enum ObjectType 
{
	OBJECT_TYPE_DYNAMIC,			// Full update, Full Collision
	OBJECT_TYPE_STATIC,	
	OBJECT_TYPE_PLAYER,			// Unused
	OBJECT_TYPE_STATDYN,	
	OBJECT_TYPE_DECORATION,
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

class ENGINE_API ObjectDescriptor{
	public:
		string m_kName;
		
		Vector3	m_kPos;									// Position of object in world.
		Vector3	m_kRot;									// Rotation of object in world.
		Vector3	m_kVel;									// Velocity of object.
		Vector3	m_kAcc;									// Acc of object.
		
		float		m_fRadius;
		
		bool		m_bSave;
		ObjectType	m_iObjectType;
		
		list<PropertyDescriptor*> m_acPropertyList;		
		
		~ObjectDescriptor();
		void Clear();
		bool SaveToFile(ZFVFile* pkFile);
		bool LoadFromFile(ZFVFile* pkFile);		
		void SaveToMem(ZFMemPackage* pkPackage);
		void LoadFromMem(ZFMemPackage* pkPackage);		
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
		// Assign

class ENGINE_API PropertyArcheType
{
public:
	PropertyArcheType();
	~PropertyArcheType();

	string						m_strName;
	vector<PropertyValue>	m_kVariables;

	PropertyArcheType(const PropertyArcheType &Other);
	PropertyArcheType& operator=(const PropertyArcheType &Other);

};

class ENGINE_API ObjectArcheType
{
private:
	void AddArchProperty(string strArchPropertys);
	PropertyArcheType*	GetArchProperty(string strArchPropertys);

public:
	string	m_strName;
	string	m_strParentName;

	vector<PropertyArcheType>	m_kArchPropertys;

	void SetValue(string strProperty, string strVariable, string Value);
	PropertyArcheType*	GetAddArchProperty(string strArchPropertys);

	ObjectArcheType();
	~ObjectArcheType();

	ObjectArcheType(const ObjectArcheType &Other);
	ObjectArcheType& operator=(const ObjectArcheType &Other);
};

// Net Flags for Network updates
#define OBJ_NETFLAG_POS		1
#define OBJ_NETFLAG_ROT		2


/// Game Object for things in game 
class ENGINE_API Object 
{
	friend class Property;
	
	private:
		Object*						m_pkParent;							///< Parent Object. NULL If None
		vector<GameMessage>		m_kGameMessages;					///< Messages that are waiting to be handled by this object.

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
	
		PropertyFactory*			m_pkPropertyFactory;				// Ptr to property factory.
		ZeroFps*						m_pkFps;								// Ptr to zerofps. // CHANGE TO ObjectManger?
	
		
		ZFResourceHandle*			m_pScriptFileHandle;			//handle for the objects create script if any
		
		string						m_strName;								///< Object name
		string						m_strType;							///< Object type name.
	
		bool							m_bSave;								///< True if this object should save to disk.
	
		int							m_iNetUpdateFlags;				///< Network flags for what needs to be updated to clients.					
		int							m_iUpdateStatus;					
		ObjectType					m_iObjectType;						
	
		// Rotation & Position.
		bool							m_bRelativeOri;					///< True if this object transform is in the frame of its parent.
		bitset<7>					m_kGotData;							

		Vector3						m_kLocalPosV;	//important	///< Local position.
		Vector3						m_kWorldPosV;						///< World position.

		Matrix4						m_kLocalRotM;	//important	///< Local rotation
		Matrix4						m_kWorldRotM;
		Matrix4						m_kWorldOriM;
		Matrix4						m_kLocalOriM;
		Quaternion					m_kWorldRotQ;
		Quaternion					m_kLocalRotQ;
		Vector3						m_kWorldRotV;
		Vector3						m_kLocalRotV;		

		Vector3						m_kVel;								///< Velocity of object.
		Vector3						m_kAcc;								///< Acc of object.
		float							m_fRadius;							///< Radius of object.

		int*							m_piDecorationStep;
		


		vector<Object*>			m_akChilds;							///< List of child objects.
		vector<Property*>			m_akPropertys;						///< List of propertys of object.
		
		Object();				
		
	public:
		int							iNetWorkID;							///< ID used by network state code.
		ObjectManager*				m_pkObjectMan;						///< Ptr to object manger.
	
		NetWorkRole					m_eRole;								///< This node power on object.
		NetWorkRole					m_eRemoteRole;						///< Remote node power on object.

		~Object();
		
		// Object Type Handling
		bool IsA(string strStringType);								///< Returns true if this object is based on type.

		// Property Mangement Code.

		Property* AddProperty(Property* pkNewProperty);		///< Add a propyrty by ptr.
		Property* AddProperty(const char* acName);			///< Create/Add a property by name.
		void RemoveProperty(Property* pkProp);					///< Remove property by pointer.
		bool DeleteProperty(const char* acName);				///< Remove property by name.
		void PropertyLost(Property* pkProp);

		Property* GetProperty(const char* acName);											///< Returns property by name (first one only). 
		void GetPropertys(vector<Property*> *akPropertys,int iType,int iSide);			///< Get all propertys by flags.
		void GetAllPropertys(vector<Property*> *akPropertys,int iType,int iSide);		///< Used mainly for updates

		Property* AddProxyProperty(const char* acName);		///< Add a property if not exist.
		bool Update(const char* acName);							///< Run update on property 'name'.

		// Child/Parent object mangement.
		void AddChild(Object* pkObject);							///< Set a object to be child to this.	
		void RemoveChild(Object* pkObject);						///< Remove a child from this.
		void SetParent(Object* pkObject);						///< Set the parent of this object.
		Object* GetParent(){return m_pkParent;};				///< Get parent of this object.
		bool HasChild(Object* pkObject);							
		int NrOfChilds();												///< Return num of childs to this object.
		void DeleteAllChilds();										///< Remove all childs from this object.
		void GetAllObjects(vector<Object*> *pakObjects);		///< Return this + all childs.
		void AttachToClosestZone();								///< Attacth to closest ZoneObject.
		void AttachToZone();											///< Attacth to ZoneObject if inside any.

		// NetWork/Demo/Save/Load Code.
		bool IsNetWork();												// True if this object has any netactive propertys.
		bool NeedToPack();											///< Returns true if there is any netactive properys in object
		void PackTo(NetPacket* pkNetPacket);					///< Pack Object.
		void PackFrom(NetPacket* pkNetPacket);					///< Unpack Object.
		void Save(ObjectDescriptor* ObjDesc);					///< Save object to object desc.

		void Save(ZFIoInterface* pkFile);
		void Load(ZFIoInterface* pkFile);		

		// Collision / Shape.
		float GetBoundingRadius();									///< Get radius of collision object or radius 1.0 if none found.
		void Touch(Collision* pkCol);								///< Run touch on all properys of this object.

		// Game Messages
		void	AddGameMessage(GameMessage& Msg);
		void	RouteMessage(GameMessage& Msg);
		void	HandleMessages();

		// Debug
		void PrintTree(int pos);									///< Debug: Prints object tree from object.
		bool CheckLinks(bool bCheckChilds, int iPos);		// Checks that parent/child links are ok. 

		// Orientation  
		bool			GetRelativeOri()				{	return m_bRelativeOri;		};
		void			SetRelativeOri(bool bRO)	{	m_bRelativeOri = bRO;		};
		
		Matrix4		GetLocalRotM();					
		Matrix4		GetWorldRotM();
		Matrix4		GetWorldOriM();								//parent * rot * pos
		Matrix4		GetLocalOriM();
		Vector3		GetWorldRotV();
		Vector3		GetLocalRotV();		

		Vector3		GetLocalPosV();
		Vector3		GetWorldPosV();
		
		//set oritentation data
		void			SetLocalRotM(Matrix4 kNewRot);
		void			SetLocalRotV(Vector3);
		void			SetLocalPosV(Vector3);
		void			SetWorldPosV(Vector3);
		void			SetWorldRotV(Vector3);
		void			RotateLocalRotV(Vector3 kRot);

		void			ResetGotData(){m_kGotData.reset();};
		void			ResetChildsGotData();

		// Inlines
		inline int &GetUpdateStatus()				{	return m_iUpdateStatus;	};
		inline ObjectType &GetObjectType()		{	return m_iObjectType;	};
		inline bool& GetSave()						{	return m_bSave;			};
		inline string& GetName()					{	return m_strName;			};
		inline string& GetType()					{	return m_strType;			};
		inline ZFResourceHandle* GetObjectScript()  { return m_pScriptFileHandle;};

		inline Vector3& GetVel()					{	return m_kVel;				};		
		inline Vector3& GetAcc()					{	return m_kAcc;				};
		inline float& GetRadius()					{	return m_fRadius;			};		
		inline Vector3* GetVelPointer()			{	return &m_kVel;			};		
		inline Vector3* GetAccPointer()			{	return &m_kAcc;			};
		inline float* GetRadiusPointer()			{	return &m_fRadius;		};		
		inline ObjectManager *GetObjectMan()	{	return m_pkObjectMan;	};				
		
		void MakeCloneOf(Object* pkOrginal);
		
		float GetI();
		
//		inline Vector3* GetPosPointer()	{	return &m_kPos;	};
//		inline Vector3* GetRotPointer()	{	return &m_kRot;	};
//		inline Vector3& GetPos()	{	return m_kPos;		};
//		inline Vector3 GetRot()	{	return m_kRot;		};
//		Vector3 GetIPos();
//		Vector3 GetIRot();
//		void SetRot(Vector3 kRot);
//		void SetPos(Vector3 kPos);

		friend class ObjectManager;

		// Force class to be polymorfic.
		virtual void DoNothing() {}
};

#endif




