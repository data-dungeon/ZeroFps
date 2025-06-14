#ifndef _RENDER_LIGHT_H_
#define _RENDER_LIGHT_H_

#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include "render_x.h"
#include "../ogl/zfpsgl.h"
#include "../basic/math.h"
#include "../basic/zfsystem.h"

using namespace std;


class ZSSZeroFps;


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
class RENDER_API ZSSLight : public ZFSubSystem 
{
	private:
	
		struct More_LightSource : public binary_function<LightSource*, LightSource*, bool> {
			inline bool operator()(LightSource* x, LightSource* y) { return x->fIntensity > y->fIntensity; };
		} More_Light; 		
		
		ZSSZeroFps*	m_pkZeroFps;	
		Vector3 m_kCamPos;
		
		int		m_iCurrentActiveLights;

		LightSource	m_kSun;	
		
		vector<LightSource*> m_kLights;
		vector<LightSource*> m_kSorted;
		vector<LightSource*> m_kActiveLights;

		ConVar	m_kiNrOfLights;
		ConVar	m_kfMinIntensity;
		bool		m_bAmbientOnly;	//enables use of only ambient light
		bool		m_bEnabled;			//lighting enabled?
		
		int		m_iVersion;			//is updated each time the number of lights is changed
		
		void TurnOffAll();
		void EnableLight(LightSource* pkLight,int iGlLight);		
		void DisableLight(int iGlLight);
		
	public:

		ZSSLight();
		bool StartUp();
		bool ShutDown();
		bool IsValid();
		
		void Add(LightSource* kNewLight);
		void Remove(LightSource *kLight);
		void SetCamera(Vector3 kCamPos);
		void Update(LightProfile* pkLightProfile,const Vector3& kRefPos,float fRefRadius = 0);
		void RunCommand(int cmdid, const ConCommandLine* kCommand) { } 

		void SetLighting(bool bOn);
		void SetStartUpValues();
		

		void SetAmbientOnly(bool bAmbient)					{	m_bAmbientOnly = bAmbient;		}
		
		LightSource* GetSunPointer()							{	return &m_kSun;		}		
		int  GetNrOfActiveLights()								{	return m_iCurrentActiveLights;}
		
		void GetClosestLights(vector<LightSource*>* pkLights,int iNrOfLights,Vector3 kPos,bool bNoDirectional = false);
		LightSource* GetFirstDirectionalLight();

};



#endif







