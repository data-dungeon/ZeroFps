#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <vector>
#include <list>
#include "../basic/basic.pkg"


struct LightSource {
	//position and rotation
	Vector3 *kPos;	
	Vector3 *kRot;
	
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
};


class Light {
	private:
		Vector3 *m_kCamPos;
		int m_iNrOfLights;
		list<LightSource*> m_kLights;
		
		LightSource *m_akCurrentLights;

	public:

		Light(Vector3 *kCamPos);
		
		void Add(LightSource* kNewLight);
		void Remove(LightSource *kLight);
		
		void Update();



};



#endif







