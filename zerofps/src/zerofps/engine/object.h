#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <list>
#include <string.h>
#include "engine_x.h"
#include "../basic/basic.pkg"
#include "property.h"
//#include "../engine/objects.pkg"
#include "objectmanager.h"
#include "propertyfactory.h"

using namespace std;

class ObjectManager;

class ENGINE_API Object {
		PropertyFactory*	m_pkPropFactory;

	protected:
		Vector3 m_kPos;
		Vector3 m_kRot;
		Vector3 m_kVel;
		bool m_bStatic;	
		int m_iType;
		
		list<Property*> m_akPropertys;
		ObjectManager* m_pkObjectMan; 
	public:


		Object();		
		~Object();
		
		void Remove();
		Property* GetProperty(char* acName);
		void SetPropertyFactory(PropertyFactory* pkPropFactory) { m_pkPropFactory = pkPropFactory; }
		void AddProperty(Property* pkNewProperty);
		void AddProperty(char* acName);
		bool RemoveProperty(char* acName);
		void Update();
		bool Update(char* acName);
		
		inline Vector3 &GetPos(){return m_kPos;};
		inline Vector3 &GetRot(){return m_kRot;};
		inline Vector3 &GetVel(){return m_kVel;};		
		inline int &GetType(){return m_iType;};
		inline bool &GetStatic(){return m_bStatic;};		
		inline void SetObjectMan(ObjectManager* pkObjectMan) {m_pkObjectMan=pkObjectMan;};		
		inline ObjectManager *GetObjectMan() {return m_pkObjectMan;};				
		
		virtual void HandleCollision(Object* pkObject,Vector3 kPos,bool bContinue);
		virtual void ObjectUpdate();		
		virtual bool Save(void *pkData,int iSize);

};

#endif





