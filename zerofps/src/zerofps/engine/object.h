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

class ENGINE_API Object {
	protected:
		Vector3 m_kPos;
		Vector3 m_kRot;
		Vector3 m_kVel;
		
		string m_kName;		

		bool m_bUpdateChilds;		
		bool m_bLoadChilds;
		bool m_bLockedChilds;
		
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
		void GetAllPropertys(list<Property*> *akPropertys,int iType,int iSide);		
		Property* GetProperty(char* acName);

		void AddProperty(Property* pkNewProperty);
		void AddProperty(char* acName);		
		Property* AddProxyProperty(char* acName);
		bool RemoveProperty(char* acName);
		bool Update(char* acName);
		
		void AddChild(Object* pkObject);
		void RemoveChild(Object* pkObject);
		void SetParent(Object* pkObject);				
		bool HasChild(Object* pkObject);
		int NrOfChilds();
		void DeleteAllChilds();
		void GetAllObjects(list<Object*> *pakObjects);
		void AttachToClosestZone();
		void PrintTree(int pos);
		
		
		
		bool NeedToPack();				// Returns true if there is any netactive properys in object
		void PackTo(NetPacket* pkNetPacket);
		void PackFrom(NetPacket* pkNetPacket);

		inline bool &GetUpdateChilds() {return m_bUpdateChilds;};

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





