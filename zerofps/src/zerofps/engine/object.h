#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <list>
#include <string.h>
#include "engine_x.h"
#include "../engine/objects.pkg"
#include "objectmanager.h"

using namespace std;

class ObjectManager;

class ENGINE_API Object {
	protected:
		Vector3 m_kPos;
		Vector3 m_kRot;
		bool m_bStatic;	
		bool m_bDelete;

		list<Property*> m_akPropertys;

	public:
		ObjectManager* m_pkObjectMan; 
		Object();		
		~Object();
		
		Property* GetProperty(char* acName);
		void AddProperty(Property* pkNewProperty);
		bool RemoveProperty(char* acName);
		void Update();
		bool Update(char* acName);
		

//		inline void Static(bool newStatic){m_bStatic=newStatic;};
		
		inline Vector3 &GetPos(){return m_kPos;};
		inline Vector3 &GetRot(){return m_kRot;};
//		inline bool &GetDelete(){return m_bDelete;};
		inline bool &Static(){return m_bStatic;};		
		inline void SetObjectMan(ObjectManager* pkObjectMan) {m_pkObjectMan=pkObjectMan;};		
		
		
};

#endif





