#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#include "objects.pkg"

using namespace std;


class ENGINE_API ObjectManager {
	
	list<Object*> m_akObjects;
	
	public:
		void Add(Object* pkNewObject);
		void Remove(Object* pkObject);
		void Update();
		
		

};


#endif




