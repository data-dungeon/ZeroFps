#ifndef _RENDER_LIGHT_H_
#define _RENDER_LIGHT_H_

#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include "render_x.h"
#include "../ogl/zfpsgl.h"
#include "../basic/vector3.h"
#include "../basic/vector4.h"
#include "../basic/zfobjectmanger.h"

using namespace std;
enum LIGHT_TYPE {
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

using namespace std;

/// Light Source used by render.
class RENDER_API LightSource {
	public:
		//position and rotation
		Vector3 *kPos;					///< Position of light source.
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
		
		LightSource();
		bool operator<(const LightSource& kOther)
		{
			return fIntensity < kOther.fIntensity;		
		};
	

};

/// SubSystem that handles all lights in ZeroFPS.
class RENDER_API Light : public ZFSubSystem {
	private:
		Vector3 m_kCamPos;

		list<LightSource*> m_kLights;		
		vector<LightSource*> m_kSorted;
		vector<LightSource*> m_kActiveLights;

		void TurnOffAll();
		
	public:
		int m_iNrOfLights;
		
		/*
		struct Less_LightSource : public binary_function<LightSource*, LightSource*, bool> {
			bool operator()(LightSource* x, LightSource* y) { return x->fIntensity < y->fIntensity; };
		} Less_Light;
		*/

		struct More_LightSource : public binary_function<LightSource*, LightSource*, bool> {
			inline bool operator()(LightSource* x, LightSource* y) { return x->fIntensity > y->fIntensity; };
		} More_Light;

		Light();
		
		void Add(LightSource* kNewLight);
		void EnableLight(LightSource* pkLight,int iGlLight);
		void Remove(LightSource *kLight);
		void SetCamera(Vector3 kCamPos);
		void Update();
		void Update(Vector3 kPos);
		void RunCommand(int cmdid, const CmdArgument* kCommand) { }

		bool StartUp();	
		bool ShutDown();
		bool IsValid();

};



#endif







