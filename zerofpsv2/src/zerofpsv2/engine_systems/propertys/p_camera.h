#ifndef _ENGINE_CAMERAPROPERTY_H_
#define _ENGINE_CAMERAPROPERTY_H_

#include "../../engine/property.h"
#include <iostream>

#include "../../engine/camera.h"
#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"

using namespace std;


enum ENGINE_SYSTEMS_API CamType_e
{
	CAM_TYPEFIRSTPERSON,	// View from object. 
	CAM_TYPETOPDOWN,		// Top down view from object.
	CAM_TYPEISO,			// Isometric view of object.
	CAM_TYPESIDE,
	CAM_TYPECHASE,
	CAM_TYPEDYNAMICISO,		
	CAM_TYPEMAX,
	CAM_TYPE3PERSON, 
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
		float		m_f3PYPos;
		float		m_f3PDistance;

		void LookAt(Vector3 kCamPosition, Vector3 kCamTarget,Vector3 kCamUp);
		void Look(Vector3 kCamPosision,Vector3 kLookDir,Vector3 kCamUp);
		

	public:
		P_Camera();
		void CloneOf(Property* pkProperty) { }
	
		void Update();

		void SetCamera(Camera *pkCamera) { m_pkCamera = pkCamera; };
		
		void Set3PYAngle(float fA) { m_f3PYAngle = fA;};
		void Set3PYPos(float fP) { m_f3PYPos = fP;};
		void Set3PDistance(float fD) { m_f3PDistance = fD;};
		void SetType(CamType_e kType);
		void NextType(CamType_e kType);
		void SetFpFov(float fFov);		
		Vector3& GetDynamicAngles() { return m_kDynamicIso; } 
};

Property* Create_CameraProperty();

#endif

















