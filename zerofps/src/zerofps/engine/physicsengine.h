#ifndef _PHYSICSENGINE_H_
#define _PHYSICSENGINE_H_


//#include "objectmanager.h"

#include "../basic/basic.pkg"
#include "zerofps.h"
#include "engine_x.h"
#include "object.h"
//#include "physicproperty.h"

#include <list>
#include <vector>

#include <algorithm>


using namespace std;

class Object;
class PhysicProperty;

struct ENGINE_API CollisionData
{
	Object* m_pkOther;	
	Vector3 m_kOtherPos;		
	Vector3 m_kOtherVel;	
	Vector3 m_kOtherAcc;		
	Vector3 m_kOtherRot;		
	
	Vector3 m_kPos;
	Vector3 m_kVel;
	Vector3 m_kAcc;
	Vector3 m_kRot;	

	Vector3 m_kNormal;

};

struct Sphere{
	PhysicProperty* m_pkPP;
	float m_fRadius;
	Vector3 m_kPos;
};

class ENGINE_API PhysicsEngine : public ZFObject
{
	private:
		ObjectManager* m_pkObjectMan;
		ZeroFps* m_pkZeroFps;

		float m_fFrameTime;
		bool m_bUpdate;
	
		//curent physicpropertys
		list<Property*> m_kPropertys;		
		
		//curent motionspheres
		vector<Sphere> m_kMotionSpheres;

	public:
		PhysicsEngine();
		
		void Update();

		Vector3 GetNewPos(PhysicProperty* pkPP);
		Vector3 GetNewVel(PhysicProperty* pkPP);
		
		void CalcMotionSpheres();
		void TestCollisions();
		bool TestSphere(Sphere* S1,Sphere* S2);
		void DeepTest(Sphere* S1,Sphere* S2);
		
		void MoveObject(PhysicProperty* pkPP);

		void SetUpdate(bool bUpdate) { m_bUpdate=bUpdate;};
		bool GetUpdate() {return m_bUpdate;};
};





#endif






