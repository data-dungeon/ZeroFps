#ifndef _COLLISIONMANAGER_H_
#define _COLLISIONMANAGER_H_


#include "engine.pkg"
#include "objects.pkg"
#include "../basic/basic.pkg"
#include <vector>
#include <iostream>
#include <list>
#include "collisionproperty.h"


using namespace std;

class ENGINE_API CollisionManager : public ZFObject{
	private:
		list<CollisionProperty*> m_akColPropertys;
		
	public:
		CollisionManager() : ZFObject("CollisionManager") { };
	
		void Add(Object* kObject);
//		void Add(CollisionProperty* kColProperty);		
		void Remove(Object* kObject);
		void Remove(CollisionProperty* kColProperty);
		
		void Update();


};

#endif






