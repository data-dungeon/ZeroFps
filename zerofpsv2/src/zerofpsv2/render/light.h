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
#include "../basic/zfsystem.h"

using namespace std;


class ZeroFps;


enum LIGHT_TYPE {
	DIRECTIONAL_LIGHT =	0,
	POINT_LIGHT = 			1,
	SPOT_LIGHT =			2,
};


class RENDER_API LightProfile
{
	public:
		int		m_iLastVersion;
		float		m_fLastTime;	
		int		m_aiLights[8];		
		
	LightProfile()
	{
		m_iLastVersion =	-1;
		m_fLastTime		=	-1;
		
		for(int i =0;i<8;i++)
			m_aiLights[i] = -1;
	}
};


/**	\brief	Light Source used by render.
		\ingroup Render
*/
class RENDER_API LightSource 
{
	public:
		//position and rotation
		Vector3 kPos;					///< Position of light source.
		Vector3 kRot;					
	
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
		
		float fIntensity;				//used for sorting
		int	iListPos;				//used for sorting
		
		LightSource();
/*		bool operator<(const LightSource& kOther)
		{
			return fIntensity < kOther.fIntensity;		
		};
*/	

};

/**	\brief	SubSystem that handles all lights in ZeroFPS.
		\ingroup Render
*/
class RENDER_API Light : public ZFSubSystem 
{
	private:
	
		struct More_LightSource : public binary_function<LightSource*, LightSource*, bool> {
			inline bool operator()(LightSource* x, LightSource* y) { return x->fIntensity > y->fIntensity; };
		} More_Light; 		
		
		ZeroFps*	m_pkZeroFps;
	
		Vector3 m_kCamPos;

		vector<LightSource*> m_kLights;
		vector<LightSource*> m_kSorted;
		vector<LightSource*> m_kActiveLights;

		int		m_iNrOfLights;		//max number of lights used at the same time
		bool		m_bAmbientOnly;	//enables use of only ambient light
		bool		m_bEnabled;			//lighting enabled?
		
		int		m_iVersion;			//is updated each time the number of lights is changed
		
		void TurnOffAll();
		
	public:

		Light();
		bool StartUp();
		bool ShutDown();
		bool IsValid();

		void Add(LightSource* kNewLight);
		void EnableLight(LightSource* pkLight,int iGlLight);
		void Remove(LightSource *kLight);
		void SetCamera(Vector3 kCamPos);
		void Update(Vector3 kPos);
		void Update(LightProfile* pkLightProfile,Vector3 kRefPos);
		void RunCommand(int cmdid, const CmdArgument* kCommand) { } 

		void SetLighting(bool bOn);
		void SetStartUpValues();

		void SetAmbientOnly(bool bAmbient)	{	m_bAmbientOnly = bAmbient;	}
		
		void GetClosestLights(vector<LightSource*>* pkLights,int iNrOfLights,Vector3 kPos,bool bNoDirectional = false);
		LightSource* GetFirstDirectionalLight();

};



#endif







