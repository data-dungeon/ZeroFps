#ifndef _LIGHT_H_
#define _LIGHT_H_



#include <iostream>
#include <vector>
#include <list>
#include <SDL/SDL_opengl.h>
#include "../basic/basic.pkg"

using namespace std;
enum LIGHT_TYPE {
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};


struct LightSource {
	//position and rotation
	Vector3 *kPos;	
	Vector3 *kRot;
	Vector3 kConstPos;
	Vector3 kConstRot;
	
	//light color
	Vector4 kDiffuse;
	Vector4 kAmbient;
	Vector4 kSpecular;
	
	//spotlight
	float fCutoff;
	float fExp;
	
	//distance attenuation 
	float fConst_Atten;
	float fLinear_Atten;
	float fQuadratic_Atten;
	
	int iType;
	int iPriority;
};


class Light {
	private:
		Vector3 *m_kCamPos;
		int m_iNrOfLights;
		list<LightSource*> m_kLights;		
		vector<LightSource*> m_kActiveLights;

		void TurnOffAll();

	public:

		Light();
		
		void Add(LightSource* kNewLight);
		void Remove(LightSource *kLight);
		void SetCamera(Vector3 *kCamPos);
		void Update();

};



#endif







