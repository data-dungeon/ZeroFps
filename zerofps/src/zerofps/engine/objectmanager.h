#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#include "objects.pkg"

using namespace std;


class ENGINE_API ObjectManager {
	
	list<Object*> m_akObjects;
	vector<Object*> m_akDeleteList;
	
	public:
		void Add(Object* pkNewObject);
		void Delete(Object* pkNewObject);		//adds an object to delete qeue
		void Remove(Object* pkObject);
		void Update();
		void UpdateDelete();								//deletes objects in delete qeue	
		

};


#endif




