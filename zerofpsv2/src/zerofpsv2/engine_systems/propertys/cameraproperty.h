#ifndef _ENGINE_CAMERAPROPERTY_H_
#define _ENGINE_CAMERAPROPERTY_H_

#include "../../engine/property.h"
#include <iostream>

#include "../../engine/camera.h"
#include "../../engine/zerofps.h"
#include "../engine_systems_x.h"

using namespace std;

class ENGINE_SYSTEMS_API CameraProperty:public Property 
{
	public:
		enum CamType_e
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

	private:
		Camera*		m_pkCamera;			// Camera to refresh.
		CamType_e	m_eCameraType;		// How the camera will behave.
		ZeroFps*		m_pkFps;
		

		float		m_fFov;
		Vector3		m_kDynamicIso;
		
		Vector3 m_kInterPos;

	public:
		CameraProperty();
		void CloneOf(Property* pkProperty) { }
	
		void Update();

		void SetCamera(Camera *pkCamera) { m_pkCamera = pkCamera; };
		void SetType(CamType_e kType);
		void NextType(CamType_e kType);
		void SetFpFov(float fFov);		
		Vector3& GetDynamicAngles() { return m_kDynamicIso; } 
};

Property* Create_CameraProperty();

#endif

















