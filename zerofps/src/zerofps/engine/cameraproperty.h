#ifndef _CAMERAPROPERTY_H_
#define _CAMERAPROPERTY_H_

#include "property.h"
#include <iostream>

#include "objects.pkg"
#include "camera.h"


using namespace std;

class ENGINE_API CameraProperty:public Property{
	public:
		enum CamType_e
		{
			CAM_TYPEFIRSTPERSON,
			CAM_TYPETOPDOWN,
			CAM_TYPEISO,
			CAM_TYPESIDE,
			CAM_TYPEMAX,
		};

	private:
		Camera*		m_pkCamera;
		CamType_e	m_eCameraType;
		
	public:
		CameraProperty();
		//CameraProperty(Camera * pkCamera);
		
		void Update();

		void SetCamera(Camera *pkCamera) { m_pkCamera = pkCamera; };
		void SetType(CamType_e kType);
		void NextType(CamType_e kType);
				
};

Property* Create_CameraProperty();

#endif

















