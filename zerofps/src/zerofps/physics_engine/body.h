#ifndef _BODY_H_
#define _BODY_H_

#include "../engine/mad_core.h"
#include "physics_engine_x.h"
#include <iostream>
#include <vector>
#include <list>


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
		
		Matrix4		m_kAngles;				//Angle in body coordinats
		Vector3		m_kAngleVel;			 //angle velocity in body coordinats
		Vector3		m_kAngleAcceleration; //angle acclereration in body coordinats		
		Matrix4		m_kOrientation;		 //orientatiojn in world cordinats
		
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
				
				
				
		Body();
		void Reset();
		void SetPos(Vector3 kPos);
		Vector3 GetPos();								
		void Rest(Body* pkBody);				//set object to rest against pkBody
		void Awaken();
		Vector3 TransRot(Vector3 kVert);
		
		bool SetMad(Mad_Core* pkMad,int iMesh);
		void SetScalep(float* pfScale) { m_pfScale = pfScale;};
		
		friend class Physics_Engine;
};
#endif


