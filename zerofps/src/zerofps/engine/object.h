#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "../engine/objects.pkg"
#include <vector>
#include <list>
#include <string.h>
#include "engine_x.h"

using namespace std;

class ENGINE_API Object {
	protected:
		Vector3 m_kPos;
		Vector3 m_kRot;
		bool m_bStatic;	

		list<Property*> m_akPropertys;

	public:
		Object();		
		~Object();
		
		Property* GetProperty(char* acName);
		void AddProperty(Property* pkNewProperty);
		bool RemoveProperty(char* acName);
		void Update();
		bool Update(char* acName);
		
		inline bool Static(void){return m_bStatic;};
		inline void Static(bool newStatic){m_bStatic=newStatic;};
		
		inline Vector3 &GetPos(){return m_kPos;};
		inline Vector3 &GetRot(){return m_kRot;};
		
};

#endif





