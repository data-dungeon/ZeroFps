#ifndef _ENGINE_PHYSICSENGINE_H_
#define _ENGINE_PHYSICSENGINE_H_

#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"
#include "../../engine/entity.h"
//#include "physicproperty.h"

#include <list>
#include <vector>

#include <algorithm>


using namespace std;

class Entity;
class P_Physic;

struct ENGINE_SYSTEMS_API Collision
{
	P_Physic* m_pkPP1;
	P_Physic* m_pkPP2;

	Vector3 m_kPos1;
	Vector3 m_kPos2;

	float m_fDistance1;
	float m_fDistance2;
	
	Vector3 m_kNormal1;
	Vector3 m_kNormal2;	
	

	bool m_bAdded;
	bool m_bChecked;
};

struct ENGINE_SYSTEMS_API CP
{
	P_Physic* m_pkPP;
	
	Collision* m_pkCol;
};

struct ENGINE_SYSTEMS_API Sphere{
	P_Physic* m_pkPP;
	float m_fRadius;
	Vector3 m_kPos;
};


/// PhysicsEngine SubSystem.
class ENGINE_SYSTEMS_API PhysicsEngine : public ZFSubSystem
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
		vector<Property*> m_kPropertys;		
		
		//collisions
		vector<Collision*> m_kCollisions;
		
		//CP LISTA
		list<CP*> m_kCPs;		
		
		//curent motionspheres
		vector<Sphere> m_kMotionSpheres;
		
	public:
		PhysicsEngine();
		
		void Update();

		Vector3 GetNewPos(P_Physic* pkPP);
		Vector3 GetNewVel(P_Physic* pkPP);
		
		
		void CalcNewPos();
		void CheckCollisions();
		void GenerateMotionSpheres();
		bool TestMotionSpheres(P_Physic* pkPP1,P_Physic* pkPP2);
		Collision* DeepTest(P_Physic* pkPP1,P_Physic* pkPP2);		
		void HandleCollisions();
		void ClearCollisions();	
		
		bool Stride(P_Physic* pkPP);
		Collision* CheckIfColliding(P_Physic* pkPP);
		
		bool TestLine(list<P_Physic*>* pkPPList,Vector3 kPos,Vector3 kVec);
		bool TestLine(list<Entity*>* pkObList,Vector3 kPos,Vector3 kVec);
		
		void SetUpdate(bool bUpdate) { m_bUpdate=bUpdate;};
		bool GetUpdate() {return m_bUpdate;};

		bool StartUp();
		bool ShutDown();
		bool IsValid();

};





#endif






