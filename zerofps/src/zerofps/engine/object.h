#ifndef _ENGINE_OBJECT_H_
#define _ENGINE_OBJECT_H_

#include "../basic/basic.pkg"
#include <vector>
#include <list>
#include <string.h>
#include "engine_x.h"
#include "property.h"
#include "propertyfactory.h"
#include "network.h"

using namespace std;

class		ObjectManager;
struct	CollisionData;
class		LevelManager;

enum UpdateStatus {
	UPDATE_NONE				= 1,	
	UPDATE_ALL				= 2,
	UPDATE_STATIC			= 4,
	UPDATE_DYNAMIC 		= 8,
	UPDATE_PLAYERS 		= 16,
	UPDATE_STATDYN 		= 32,
	UPDATE_DECORATION		= 64,
	UPDATE_LIGHT			= 128,
};

enum ObjectType {
	OBJECT_TYPE_DYNAMIC,			// Full update, Full Collision
	OBJECT_TYPE_STATIC,	
	OBJECT_TYPE_PLAYER,			// Unused
	OBJECT_TYPE_STATDYN,	
	OBJECT_TYPE_DECORATION,
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
		bool SaveToFile(ZFFile* pkFile);
		bool LoadFromFile(ZFFile* pkFile);		
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



/// Game Object for things in game 
class ENGINE_API Object 
{
	private:
		Object*						m_pkParent;						///< Parent Object.
		vector<GameMessage>		m_kGameMessages;				///< Messages that are waiting to be handled by this object.

	protected:
		Vector3					m_kPos;								///< Position of object in world.
		Vector3					m_kRot;								///< Rotation of object in world.
		Vector3					m_kVel;								///< Velocity of object.
		Vector3					m_kAcc;								///< Acc of object.
		float						m_fRadius;		
		
		Vector3					m_kOldPos;
		Vector3					m_kOldRot;
		
		string					m_kName;								///< Object name
		string					m_strType;							///< Object type name.

		ObjectType				m_iObjectType;						
		int						m_iUpdateStatus;					
		int*						m_piDecorationStep;
		bool						m_bSave;								///< True if this object should save to disk.
		
		LevelManager* 			m_pkLevelMan;		
		PropertyFactory*		m_pkPropertyFactory;	
		ZeroFps*					m_pkFps;

		list<Object*>			m_akChilds;							///< List of child objects.
		list<Property*>		m_akPropertys;						///< List of propertys of object.
		
	public:
		int						iNetWorkID;								///< ID used by network state code.
		ObjectManager*			m_pkObjectMan;						///< Ptr to object manger.

		Object();		
		~Object();
		
		// Object Type Handling
		bool IsA(string strStringType);								///< Returns true if this object is based on type.

		// Property Mangement Code.

		Property* AddProperty(Property* pkNewProperty);		///< Add a propyrty by ptr.
		Property* AddProperty(const char* acName);			///< Create/Add a property by name.
		void RemoveProperty(Property* pkProp);					///< Remove property by pointer.
		bool DeleteProperty(const char* acName);				///< Remove property by name.

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
		void GetAllObjects(list<Object*> *pakObjects);		///< Return this + all childs.
		void AttachToClosestZone();								///< Attacth to closest ZoneObject.

		// NetWork/Demo/Save/Load Code.
		bool NeedToPack();											///< Returns true if there is any netactive properys in object
		void PackTo(NetPacket* pkNetPacket);					///< Pack Object.
		void PackFrom(NetPacket* pkNetPacket);					///< Unpack Object.
		void Save(ObjectDescriptor* ObjDesc);					///< Save object to object desc.

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

		// Inlines
		inline int &GetUpdateStatus() {return m_iUpdateStatus;};
		inline ObjectType &GetObjectType(){return m_iObjectType;};

		inline bool& GetSave()		{	return m_bSave;	};
		inline string& GetName()	{	return m_kName;	};
		inline Vector3& GetPos()	{	return m_kPos;		};
		inline Vector3& GetRot()	{	return m_kRot;		};
		inline Vector3& GetVel()	{	return m_kVel;		};		
		inline Vector3& GetAcc()	{	return m_kAcc;		};
		inline float& GetRadius()	{	return m_fRadius;		};		
		inline Vector3* GetPosPointer()	{	return &m_kPos;	};
		inline Vector3* GetRotPointer()	{	return &m_kRot;	};
		inline Vector3* GetVelPointer()	{	return &m_kVel;	};		
		inline Vector3* GetAccPointer()	{	return &m_kAcc;	};
		inline float* GetRadiusPointer()	{	return &m_fRadius;	};		
		inline ObjectManager *GetObjectMan() {return m_pkObjectMan;};				
		
		void MakeCloneOf(Object* pkOrginal);
		const char* GetType(){return m_strType.c_str();}
		
		float GetI();
		Vector3 GetIPos();
		Vector3 GetIRot();
		void SetRot(Vector3 kRot);
		void SetPos(Vector3 kPos);

		friend class ObjectManager;


		// Force class to be polymorfic.
		virtual void DoNothing() {}
};

#endif




