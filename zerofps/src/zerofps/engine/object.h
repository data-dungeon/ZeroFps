#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "../basic/basic.pkg"
#include <vector>
#include <list>
#include <string.h>
#include "engine_x.h"
#include "property.h"
#include "objectmanager.h"
#include "propertyfactory.h"
#include "network.h"

using namespace std;

class ObjectManager;

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
		
		list<PropertyDescriptor*> m_acPropertyList;		
		
		~ObjectDescriptor()
		{
			for(list<PropertyDescriptor*>::iterator it=m_acPropertyList.begin();it!=m_acPropertyList.end();it++)
			{
				delete (*it);
			}
		}
		
		void Clear()
		{
			m_kName="";
			m_kPos.Set(0,0,0);
			m_kRot.Set(0,0,0);
			m_kVel.Set(0,0,0);			
			
			for(list<PropertyDescriptor*>::iterator it=m_acPropertyList.begin();it!=m_acPropertyList.end();it++)
			{
				delete (*it);
			}
			
			m_acPropertyList.clear();
		}
};


enum UPDATE_STATUS{
	UPDATE_NONE,
	UPDATE_ALL,
	UPDATE_STATIC,
	UPDATE_DYNAMIC,
	UPDATE_PLAYERS
};


class ENGINE_API Object {
	protected:
		Vector3 m_kPos;
		Vector3 m_kRot;
		Vector3 m_kVel;
		
		string m_kName;		

//		bool m_bUpdateChilds;
		int m_iUpdateStatus;
		bool m_bLoadChilds;
		bool m_bLockedChilds;
		bool m_bAutoParent;
		
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
		bool RemoveProperty(const char* acName);
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

		inline string &GetName(){return m_kName;};
		inline Vector3 &GetPos(){return m_kPos;};
		inline Vector3 &GetRot(){return m_kRot;};
		inline Vector3 &GetVel(){return m_kVel;};		
		inline void SetObjectMan(ObjectManager* pkObjectMan) {m_pkObjectMan=pkObjectMan;};		
		inline ObjectManager *GetObjectMan() {return m_pkObjectMan;};				
		
		float GetBoundingRadius();
		
		virtual void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);

//		void Update();
//		void Update(int iType,int iSide);		
//		inline void SetUpdateChilds(bool bUpdateChilds) { m_bUpdateChilds=bUpdateChilds;};
//inline int &GetType(){return m_iType;};
//		inline bool &GetStatic(){return m_bStatic;};		
//		void SetPropertyFactory(PropertyFactory* pkPropFactory) { m_pkPropFactory = pkPropFactory; }
//		void Remove();
};

#endif





