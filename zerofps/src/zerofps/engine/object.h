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

class ENGINE_API PropertyDescriptor{
	public:
		string m_kName;
		ZFMemPackage m_kData;
};

class ENGINE_API ObjectDescriptor{
	public:
		string m_kName;
		
		Vector3 m_kPos;
		Vector3 m_kRot;
		Vector3 m_kVel;
		Vector3 m_kAcc;
		
		bool m_bSave;
		int m_iObjectType;
		
		list<PropertyDescriptor*> m_acPropertyList;		
		
		~ObjectDescriptor();
		void Clear();
		bool SaveToFile(ZFFile* pkFile);
		bool LoadFromFile(ZFFile* pkFile);		
		void SaveToMem(ZFMemPackage* pkPackage);
		void LoadFromMem(ZFMemPackage* pkPackage);		
};


enum UPDATE_STATUS{
	UPDATE_NONE,
	UPDATE_ALL,
	UPDATE_STATIC,
	UPDATE_DYNAMIC,
	UPDATE_PLAYERS,
};

enum OBJECT_TYPE{
	OBJECT_TYPE_DYNAMIC,
	OBJECT_TYPE_STATIC,
	OBJECT_TYPE_PLAYER,
};

class ENGINE_API Object {
	protected:
		Vector3 m_kPos;
		Vector3 m_kRot;
		Vector3 m_kVel;
		Vector3 m_kAcc;		
		
		string m_kName;		

		int m_iObjectType;		
		int m_iUpdateStatus;
		bool m_bLoadChilds;
		bool m_bLockedChilds;
		bool m_bAutoParent;
		
		bool m_bSave;
		
		list<Property*> m_akPropertys;
		ObjectManager* m_pkObjectMan; 
	

		list<Object*> m_akChilds;
		Object* m_pkParent;	
	
		PropertyFactory*	m_pkPropertyFactory;
		
	public:

		int		iNetWorkID;		// ID used by network state code.
		Object();		
		~Object();
		
		void GetPropertys(list<Property*> *akPropertys,int iType,int iSide);		
		void GetAllPropertys(list<Property*> *akPropertys,int iType,int iSide);		//used mainly for updates
		Property* GetProperty(const char* acName);

		bool AddProperty(Property* pkNewProperty);
		bool AddProperty(const char* acName);		
		Property* AddProxyProperty(char* acName);
		bool DeleteProperty(const char* acName);
		void RemoveProperty(Property* pkProp);
		bool Update(const char* acName);
		
		void AddChild(Object* pkObject);
		void RemoveChild(Object* pkObject);
		void SetParent(Object* pkObject);
		Object* GetParent(){return m_pkParent;};
		bool HasChild(Object* pkObject);
		int NrOfChilds();
		void DeleteAllChilds();
		void GetAllObjects(list<Object*> *pakObjects);
		void AttachToClosestZone();
		void PrintTree(int pos);
		
		bool NeedToPack();				// Returns true if there is any netactive properys in object
		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

		void Save(ObjectDescriptor* ObjDesc);

		inline int &GetUpdateStatus() {return m_iUpdateStatus;};

		inline int &GetObjectType(){return m_iObjectType;};
		inline bool &GetSave(){return m_bSave;};
		inline string &GetName(){return m_kName;};
		inline Vector3 &GetPos(){return m_kPos;};
		inline Vector3 &GetRot(){return m_kRot;};
		inline Vector3 &GetVel(){return m_kVel;};		
		inline Vector3 &GetAcc(){return m_kAcc;};				
		inline void SetObjectMan(ObjectManager* pkObjectMan) {m_pkObjectMan=pkObjectMan;};		
		inline ObjectManager *GetObjectMan() {return m_pkObjectMan;};				
		
		float GetBoundingRadius();
		
		virtual void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);

		void Touch(CollisionData *pkData);


//		void Update();
//		void Update(int iType,int iSide);		
//		inline void SetUpdateChilds(bool bUpdateChilds) { m_bUpdateChilds=bUpdateChilds;};
//inline int &GetType(){return m_iType;};
//		inline bool &GetStatic(){return m_bStatic;};		
//		void SetPropertyFactory(PropertyFactory* pkPropFactory) { m_pkPropFactory = pkPropFactory; }
//		void Remove();
};

#endif





