#ifndef _PHYSICS_ENGINE_H_
#define _PHYSICS_ENGINE_H_

#include "../basic/basic.pkg"

#include <vector>
#include <list>

#include "body.h"

using namespace std;

class Physics_Engine : public ZFObject
{
	private:
		list<Body*>		m_kBodys;
		float				m_fCurrentTime;
	
	public:

		Physics_Engine();
		
		void Update(float fTime);
		void Clear();
		void AddBody(Body* pkBody);
		void RemoveBody(Body* pkBody);
		
};

#endif



