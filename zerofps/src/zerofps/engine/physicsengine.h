#ifndef _PHYSICSENGINE_H_
#define _PHYSICSENGINE_H_


//#include "objectmanager.h"

#include "../basic/basic.pkg"
#include "zerofps.h"
#include "engine_x.h"
#include "object.h"

using namespace std;

class Object;

struct ENGINE_API CollisionData
{
	Object* m_pkOtherObject;	
	
	Vector3 m_kPos;
	Vector3 m_kOtherVel;
	Vector3 m_kVel;
	Vector3 m_kNormal;

};


class ENGINE_API PhysicsEngine : public ZFObject
{
	private:
		ObjectManager* m_pkObjectMan;
		ZeroFps* m_pkZeroFps;

	public:
		PhysicsEngine();
		
		

};




#endif






