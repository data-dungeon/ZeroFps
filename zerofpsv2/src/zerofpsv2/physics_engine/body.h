#ifndef _BODY_H_
#define _BODY_H_

#include "../engine_systems/mad/mad_core.h"
#include "physics_engine_x.h"
#include <iostream>
#include <vector>
#include <list>
#include "../basic/matrix3.h"
#include "../basic/quaternion.h"


using namespace std;


class PHYSICSENGINE_API Body
{
	private:
		vector<Mad_Face>*	  m_pkFaces;
		vector<Vector3>*		m_pkVertex;			// Vertex frames for mesh.
		vector<Vector3>*		m_pkNormal;

		float*		m_pfScale;

		Matrix3		m_kInertia;				//Mass moment of inertia in body coordinats
		Matrix3		m_kInertiaInverse;	//inverse of mass moment of inertia matrix
		Vector3		m_kMassCenter;			//objects mass center in object cordinats
		
		Vector3		m_kPosition;			//object possition in world cordinats		
		Vector3		m_kVelocity;			//velocity in world coordinats
		Vector3		m_kBodyVelocity;		//Velocity in body coordinats		
		Vector3		m_kAcceleration;		//acceleration of (Center of gravity) in body coordinats
		
		Vector3		m_kAngleVel;			 //angle velocity in body coordinats
		Vector3		m_kAngleAcceleration; //angle acclereration in body coordinats		
		Matrix3		m_kOrientation;
		
		float			m_fSpeed;
		
		Vector3		m_kForces;				//total force on body
		Vector3		m_kMoment;				//total moment(torque) on body
		
		Matrix3		m_kIeInverse;			//inverse of moment of inertia in world coordinats
			
			
	public:
		Vector3		m_kVertex[2];		
		
		bool			m_bPolygonCheck;		//shuld we do a per polygon check in body
		bool			m_bGravity;				//is object affeceted of gravity
		bool			m_bSolid;				//is this a solid body?				
		bool			m_bResting;				//is the object in a resting possition		
				
		float			m_fBounce;				//bounce factor		
		float			m_fMass;					//object mass (constant)
		float			m_fRadius;				//object bounding radius
		float			m_fAirFriction;		
				
				
		Body();
		void Reset();
		void SetPos(Vector3 kPos);
		Vector3 GetPos();								
		Matrix3 GetRot();
		void SetRot(Matrix3 kRot);
		
		void Rest(Body* pkBody);				//set object to rest against pkBody
		void Awaken();
		Vector3 TransRot(Vector3 kVert);
		Vector3 Rot(Vector3 kVert);		
		
		bool SetMad(Mad_Core* pkMad,int iMesh);
		void SetScalep(float* pfScale) { m_pfScale = pfScale;};
		Mad_CoreMesh* GetMeshByID(Mad_Core* pkMad,int iMesh);
		
		Vector3 CalculateMassCenter();
		float CalculateRadius();
		
		friend class Physics_Engine;
};
#endif


