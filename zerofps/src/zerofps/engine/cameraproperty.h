#ifndef _CAMERAPROPERTY_H_
#define _CAMERAPROPERTY_H_

#include "property.h"
#include <iostream>

#include "objects.pkg"
#include "camera.h"


using namespace std;

class ENGINE_API CameraProperty:public Property{
	private:
		Camera *m_pkCamera;
		
	public:
		CameraProperty(Camera * pkCamera);
		
		void Update();

		void SetCamera(Camera *pkCamera){m_pkCamera=pkCamera;};
};

#endif

















