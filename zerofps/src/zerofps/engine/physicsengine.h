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

/*
struct ENGINE_API CollisionData
{
	PhysicProperty* m_pkPP1;
	Vector3 m_kPos1;
	Vector3 m_kVel1;
	Vector3 m_kAcc1;
	Vector3 m_kRot1;	
	float   m_fDistance1;
	
	PhysicProperty* m_pkPP2;
	Vector3 m_kPos2;
	Vector3 m_kVel2;
	Vector3 m_kAcc2;
	Vector3 m_kRot2;	
	float   m_fDistance2;
	

	Vector3 m_kNormal;
	

};
*/

struct ENGINE_API Collision
{
	PhysicProperty* m_pkPP1;
	PhysicProperty* m_pkPP2;

	Vector3 m_kPos1;
	Vector3 m_kPos2;

	float m_fDistance1;
	float m_fDistance2;
	
	Vector3 m_kNormal1;
	Vector3 m_kNormal2;	
	

	bool m_bAdded;
	bool m_bChecked;
};

struct ENGINE_API CP
{
	PhysicProperty* m_pkPP;
	
	Collision* m_pkCol;
};

struct ENGINE_API Sphere{
	PhysicProperty* m_pkPP;
	float m_fRadius;
	Vector3 m_kPos;
};

class ENGINE_API PhysicsEngine : public ZFObject
{
	private:
		struct Sort_Collision : public binary_function<CP*, CP*, bool> {
			bool operator()(CP* x, CP* y) { 
				
				//its the same object then check wich collided first
				if(x->m_pkPP == y->m_pkPP)
				{
					float Distance1;
					float Distance2;
				
					if(x->m_pkPP == x->m_pkCol->m_pkPP1)
						Distance1 = x->m_pkCol->m_fDistance1;					
					else
						Distance1 = x->m_pkCol->m_fDistance2;					
					
					if(y->m_pkPP == y->m_pkCol->m_pkPP1)
						Distance2 = y->m_pkCol->m_fDistance1;					
					else
						Distance2 = y->m_pkCol->m_fDistance2;					
													
					return Distance1 < Distance2;
				}
				
				return x->m_pkPP < y->m_pkPP; 			
			};
		} SortCollision;
	
		
		ObjectManager* m_pkObjectMan;
		ZeroFps* m_pkZeroFps;

		float m_fFrameTime;
		bool m_bUpdate;
	
		bool m_bChanged;
	
		//curent physicpropertys
		list<Property*> m_kPropertys;		
		
		//collisions
		vector<Collision*> m_kCollisions;
		
		//CP LISTA
		list<CP*> m_kCPs;		
		
		//curent motionspheres
		vector<Sphere> m_kMotionSpheres;
		
	public:
		PhysicsEngine();
		
		void Update();

		Vector3 GetNewPos(PhysicProperty* pkPP);
		Vector3 GetNewVel(PhysicProperty* pkPP);
		
		
		void CalcNewPos();
		void CheckCollisions();
		void GenerateMotionSpheres();
		bool TestMotionSpheres(PhysicProperty* pkPP1,PhysicProperty* pkPP2);
		Collision* DeepTest(PhysicProperty* pkPP1,PhysicProperty* pkPP2);		
		void HandleCollisions();
		void ClearCollisions();	
		
		bool Stride(PhysicProperty* pkPP);
		Collision* CheckIfColliding(PhysicProperty* pkPP);
		
		bool TestLine(list<PhysicProperty*>* pkPPList,Vector3 kPos,Vector3 kVec);
		
		void SetUpdate(bool bUpdate) { m_bUpdate=bUpdate;};
		bool GetUpdate() {return m_bUpdate;};
};





#endif






