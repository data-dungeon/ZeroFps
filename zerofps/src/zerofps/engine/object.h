#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "../basic/basic.pkg"
#include <vector>
#include <list>
#include <string.h>
#include "engine_x.h"
//#include "physicsengine.h"
#include "property.h"
#include "objectmanager.h"
#include "propertyfactory.h"
#include "network.h"


using namespace std;

class ObjectManager;
struct CollisionData;


enum UpdateStatus {
	UPDATE_NONE,
	UPDATE_ALL,
	UPDATE_STATIC,
	UPDATE_DYNAMIC,
	UPDATE_PLAYERS,
};

enum ObjectType {
	OBJECT_TYPE_DYNAMIC,
	OBJECT_TYPE_STATIC,
	OBJECT_TYPE_PLAYER,
};

class ENGINE_API PropertyDescriptor{
	public:
		string m_kName;
		ZFMemPackage m_kData;
};

class ENGINE_API ObjectDescriptor{
	public:
		string m_kName;
		
		Vector3 m_kPos;									// Position of object in world.
		Vector3 m_kRot;									// Rotation of object in world.
		Vector3 m_kVel;									// Velocity of object.
		Vector3 m_kAcc;									// Acc of object.
		
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



class ENGINE_API Object {
	private:
		Object*				m_pkParent;							// Parent Object.

	protected:
		Vector3				m_kPos;								// Position of object in world.
		Vector3				m_kRot;								// Rotation of object in world.
		Vector3				m_kVel;								// Velocity of object.
		Vector3				m_kAcc;								// Acc of object.
		
		string				m_kName;							// Object type name

		ObjectType			m_iObjectType;						
		UpdateStatus		m_iUpdateStatus;					
//		bool				m_bLockedChilds;					
		
		bool				m_bSave;
		
		ObjectManager*		m_pkObjectMan;						// Ptr to object manger.
		PropertyFactory*	m_pkPropertyFactory;	

		list<Object*>		m_akChilds;							// List of child objects.
		list<Property*>		m_akPropertys;						// List of propertys of object.
		
	public:
		int		iNetWorkID;										// ID used by network state code.

		Object();		
		~Object();
		
		// Property Mangement Code.

		bool AddProperty(Property* pkNewProperty);				// Add a propyrty by ptr.
		bool AddProperty(const char* acName);					// Create/Add a property by name.
		void RemoveProperty(Property* pkProp);					// Remove property by pointer.
		bool DeleteProperty(const char* acName);				// Remove property by name.

		Property* GetProperty(const char* acName);									// Returns property by name (first one only). 
		void GetPropertys(list<Property*> *akPropertys,int iType,int iSide);		// Get all propertys by flags.
		void GetAllPropertys(list<Property*> *akPropertys,int iType,int iSide);		// used mainly for updates

		Property* AddProxyProperty(char* acName);									// Add a property if not exist.
		bool Update(const char* acName);											// Run update on property 'name'.

		// Child/Parent object mangement.
		void AddChild(Object* pkObject);					// Set a object to be child to this.	
		void RemoveChild(Object* pkObject);					// Remove a child from this.
		void SetParent(Object* pkObject);					// Set the parent of this object.
		Object* GetParent(){return m_pkParent;};			// Get parent of this object.
		bool HasChild(Object* pkObject);					//
		int NrOfChilds();									// Return num of childs to this object.
		void DeleteAllChilds();								// Remove all childs from this object.
		void GetAllObjects(list<Object*> *pakObjects);		// Return this + all childs.
		void PrintTree(int pos);							// Debug: Prints object tree from object.
		void AttachToClosestZone();							// Attacth to closest ZoneObject.

		// NetWork/Demo/Save/Load Code.
		bool NeedToPack();									// Returns true if there is any netactive properys in object
		void PackTo(NetPacket* pkNetPacket);				// Pack Object.
		void PackFrom(NetPacket* pkNetPacket);				// Unpack Object.
		void Save(ObjectDescriptor* ObjDesc);				// Save object to object desc.

		// Collision / Shape.
		float GetBoundingRadius();							// Get radius of collision object or radius 1.0 if none found.
		void Touch(Object* pkObject);						// Run touch on all properys of this object.

		inline UpdateStatus &GetUpdateStatus() {return m_iUpdateStatus;};
		inline ObjectType &GetObjectType(){return m_iObjectType;};
		inline bool &GetSave(){return m_bSave;};
		inline string &GetName(){return m_kName;};
		inline Vector3 &GetPos(){return m_kPos;};
		inline Vector3 &GetRot(){return m_kRot;};
		inline Vector3 &GetVel(){return m_kVel;};		
		inline Vector3 &GetAcc(){return m_kAcc;};				
		inline ObjectManager *GetObjectMan() {return m_pkObjectMan;};				
		
		bool CheckLinks(bool bCheckChilds, int iPos);		// Checks that parent/child links are ok. 
		// Force class to be polymorfic.
		virtual void DoNothing() {}
};

#endif




