#ifndef _ENGINE_CAMERAPROPERTY_H_
#define _ENGINE_CAMERAPROPERTY_H_

#include "../engine_systems_x.h"
#include "../../engine/property.h"



using namespace std;

class Camera;
class ZeroFps;

enum ENGINE_SYSTEMS_API CamType_e
{
	CAM_TYPEFIRSTPERSON,	// View from object. 
	CAM_TYPEFIRSTPERSON_NON_EA,
	CAM_TYPETOPDOWN,		// Top down view from object.
	CAM_TYPEISO,			// Isometric view of object.
	CAM_TYPESIDE,
	CAM_TYPECHASE,
	CAM_TYPEDYNAMICISO,		
	CAM_TYPEMAX,
	CAM_TYPE3PERSON, 
	CAM_TYPEBIRDSEYE,
	CAM_TYPEMLFRONTVIEW,
};

/// Connects a Object to a Camera.
class ENGINE_SYSTEMS_API P_Camera : public Property 
{
	private:
		Camera*		m_pkCamera;			// Camera to refresh.
		CamType_e	m_eCameraType;		// How the camera will behave.
		ZeroFps*		m_pkFps;

		float		m_fFov;
		Vector3	m_kDynamicIso;		
		Vector3	m_kInterPos;
		float		m_f3PYAngle;
		float		m_f3PPAngle;
		float		m_f3PDistance;				//wanted distance
		float		m_f3pCurrentDistance;	//
		Vector3	m_kOffset;					//camera offset

		bool		m_bAttachToMadBone;		//shuld we attatch to a mad bone
		string	m_strBone;					//bone name
		
		void Look(Vector3 kCamPosision,Vector3 kLookDir,Vector3 kCamUp);
		

		float LineTest(const Vector3& kStart,const Vector3& kStop);
		
	public:
		P_Camera();
		~P_Camera();
	
		void Update();
		void LookAt(Vector3 kCamPosition, Vector3 kCamTarget,Vector3 kCamUp);

		void SetCamera(Camera *pkCamera);
		
		float Get3PYAngle()							{	return m_f3PYAngle; 		}
		float Get3PPAngle()							{	return m_f3PPAngle; 		}
		float Get3PDistance()						{	return m_f3PDistance;	}
		
		void SetAttachToBone(bool bAttach)		{	m_bAttachToMadBone = bAttach;}
		void SetBone(const string& strBone)		{	m_strBone = strBone;		}
		
		void Set3PYAngle(float fA) 				{	m_f3PYAngle =  fA;		}
		void Set3PPAngle(float fP) 				{	m_f3PPAngle =  fP;		}
		void Set3PDistance(float fD) 				{	m_f3PDistance = fD;		}
		void SetOffset(Vector3 kOffset) 			{	m_kOffset = kOffset;		}
		void SetType(CamType_e kType);
		void NextType(CamType_e kType);
		void SetFpFov(float fFov);		
		Vector3& GetDynamicAngles() 				{	return m_kDynamicIso; 	} 

		
		void OrthoMove(Vector3 kMove);

};

Property* Create_CameraProperty();

#endif

















