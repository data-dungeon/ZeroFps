#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <iostream>
#include <vector>
#include <list>
#include <function.h>
#include "render_x.h"
#include "../ogl/zfpsgl.h"
#include "../basic/basic.pkg"

using namespace std;
enum LIGHT_TYPE {
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};


class RENDER_API LightSource {
	public:
		LightSource();
	
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
		
		float fIntensity;
		
		
//		bool operator<(const LightSource& l) const;

};

    
    
class RENDER_API Light : public ZFObject {
	private:
		struct Less_LightSource : public binary_function<LightSource*, LightSource*, bool> {
			bool operator()(LightSource* x, LightSource* y) { return x->fIntensity < y->fIntensity; };
		} Less_Light;
		
		struct More_LightSource : public binary_function<LightSource*, LightSource*, bool> {
			bool operator()(LightSource* x, LightSource* y) { return x->fIntensity > y->fIntensity; };
		} More_Light;
		
		
		Vector3 m_kCamPos;
		int m_iNrOfLights;
		list<LightSource*> m_kLights;		
		list<LightSource*> m_kSorted;
		vector<LightSource*> m_kActiveLights;

		void TurnOffAll();

	public:

		Light();
		
		void Add(LightSource* kNewLight);
		void Remove(LightSource *kLight);
		void SetCamera(Vector3 kCamPos);
		void Update();
//		bool Comp(LightSource* l1,LightSource* l2);
};



#endif







