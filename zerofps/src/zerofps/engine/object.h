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

using namespace std;

class ObjectManager;

class ENGINE_API Object {
	protected:
		Vector3 m_kPos;
		Vector3 m_kRot;
		bool m_bStatic;	

		list<Property*> m_akPropertys;
		ObjectManager* m_pkObjectMan; 
	public:

		Object();		
		~Object();
		
		void Remove();
		Property* GetProperty(char* acName);
		void AddProperty(Property* pkNewProperty);
		bool RemoveProperty(char* acName);
		void Update();
		bool Update(char* acName);
		

//		inline void Static(bool newStatic){m_bStatic=newStatic;};
		
		inline Vector3 &GetPos(){return m_kPos;};
		inline Vector3 &GetRot(){return m_kRot;};
		inline bool &GetStatic(){return m_bStatic;};		
		inline void SetObjectMan(ObjectManager* pkObjectMan) {m_pkObjectMan=pkObjectMan;};		
		inline ObjectManager *GetObjectMan() {return m_pkObjectMan;};				
		
		virtual void HandleCollision(Object* pkObject,bool bContinue);
		
};

#endif





