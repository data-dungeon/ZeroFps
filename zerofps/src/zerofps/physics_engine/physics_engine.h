#ifndef _PHYSICS_ENGINE_H_
#define _PHYSICS_ENGINE_H_

#include "../basic/basic.pkg"
#include "physics_engine_x.h"

#include <vector>
#include <list>
#include "body.h"

using namespace std;


struct PHYSICSENGINE_API Collission
{
	Body*		pkBody1;
	Body*		pkBody2;
	
	Vector3	kPos;					//collission point
	Vector3	kNormal;				//pkBody2's normal
	
	Vector3	kRelVelocity;			//relative velocity 
	Vector3	kRelAcceleration;	//relative acceleration
	Vector3	kCollissionTangent;	//collission tangent

	float		fAtime;					//alpha time when collission occured
};

enum PHYSICSENGINE_API CollisionState
{
	COLLISSION,
	PENETRATING,
	NOT,
};

class PHYSICSENGINE_API Physics_Engine : public ZFObject
{
	private:
		list<Body*>				m_kBodys;
		list<Plane*>			m_kPlanes;
		vector<Collission>	m_kCollissions;
		float						m_fAlphaTime;
		
		float						m_fRTime;
	
		float						m_fCtol;
		int						m_iMaxTests;
		
	public:
		int						m_iNrOfCollissions;


		Physics_Engine();
		
		void Update(float fTime);
		void Clear();
		
		bool AddBody(Body* pkBody);
		bool RemoveBody(Body* pkBody);
		bool ExistBody(Body* pkBody);
		
		bool AddPlane(Plane* pkPlane);
		bool RemovePlane(Plane* pkPlane);
		bool ExistPlane(Plane* pkPlane);		
		
		
		void UpdateForces();
		void UpdateVelNPos(float fAtime);
		void UpdateBodyVelNPos(Body* pkBody,float fAtime);
		
		void TestWithPlanes(float fATime);
		bool TestSphereVSPlane(Body* pkBody,Plane* pkPlane,float fATime);
		int CollideSphereVSPlane(Body* pkBody,Plane* pkPlane); 

		void TestBodys(float fATime);
		bool TestBodyVSBody(Body* pkBody1,Body* pkBody2,float fATime);
		int CollideBody(Body* pkBody1,Body* pkBody2);

		void UpdateResting(Body* pkBody1,Body* pkBody2);
		void HandleCollission(Collission* pkCol);
		
		Collission* FindNextCollission();

};

#endif



