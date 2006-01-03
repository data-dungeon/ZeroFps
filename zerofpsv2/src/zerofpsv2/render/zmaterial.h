#ifndef _ZMaterial_H_
#define _ZMaterial_H_

#include <iostream>
#include "render_x.h"
#include <vector>
#include <map>

#include "../ogl/zfpsgl.h"
#include "../basic/zfini.h"
#include "../basic/zfresource.h"
#include "../basic/vector4.h"

#include "../script/zfscript.h"


class ZShaderSystem;

using namespace std;

/**	\brief	Settings for one pass in a Material.
		\ingroup Render
*/
class RENDER_API ZMaterialSettings
{
	public:
		ZFResourceHandle*	m_pkTUs[4];	
		ZFResourceHandle*	m_pkVP;
		ZFResourceHandle*	m_pkFP;
		
		ZFResourceHandle*	m_pkSLP;
		
		int				m_iTUTexCords[4];
		int				m_iTUTexEnvMode[4];
		float				m_fTUTexGenScale[4];
		
		Vector4			m_kVertexColor;
		
		Vector4			m_kMatAmbient;
		Vector4			m_kMatDiffuse;
		Vector4			m_kMatSpecular;
		Vector4			m_kMatEmission;
		float				m_fShininess;
		
		bool				m_bColorMaterial;		
		float				m_fLineWidth;		
		int				m_iPolygonModeFront;
		int				m_iPolygonModeBack;				
		int				m_iDepthFunc;		
		bool				m_bLighting;	
		int				m_iCullFace;
		bool				m_bAlphaTest;	
		float				m_fAlphaTreshold;
		bool				m_bFog;		
		bool				m_bBlend;
		bool				m_bDepthTest;
		bool				m_bStencilTest;
		int				m_iStencilOpFail;
		int				m_iStencilOpZFail;
		int				m_iStencilOpZPass;
		int				m_iStencilFunc;
		int				m_iStencilFuncRef;
		unsigned int	m_iStencilFuncMask;
		int				m_iBlendSrc;
		int				m_iBlendDst;
		bool				m_bColorMask;
		bool				m_bDepthMask;
		
		bool				m_bUseShader;
	
		ZMaterialSettings();
		~ZMaterialSettings();
		ZMaterialSettings& operator=(const ZMaterialSettings& kOther);		
};

/**	\brief	A Material (Used by the shader SubSys).
		\ingroup Render

A ZMaterial contains all the settings used for a surface or something that need
to be rendered in the engine. 

options can be givven to manipulate a texture by entering them like "[Options]#[Filename]"
	valid options:
		c : clamp
		n : no mipmaping
		a : alpha texture
		f : no texture filtering
		
	example
		"cn#/data/textures/test.tga"
		
		will enable clamping and disable mipmaping on the texture

*/

class RENDER_API ZMaterial : public ZFResource
{
	private:		
		ZSSScriptSystem*					m_pkScript;				
		vector<ZMaterialSettings*> 	m_kPasses;				///< material passes
		
		//used for loading
		ZFIni									m_kIni;					///< inifile
		
		vector<pair<string,string> >	m_kMaterialParameters;	///< vector containing parameters, to parents		
		
		static map<string,int> 			m_kEnums;		//enums for loading				
		static int	m_iNextID;							//next created material will get this id
		
		int			m_iID;								//uniq id for this material		
		string		m_strName;
		
		
		bool LoadGlobalSection();
		bool LoadPass(int iPass);		
		int GetTranslateEnum(const string& strEnum);
		void SetupEnums();
		
		bool PassGetLuaDouble(char* czName,double& dTemp);
		bool PassGetLuaChar(char* czName,char* cTemp);
		
		void LuaMaterialGetGlobals();
		
	public:		
		//global material settings
 		bool	m_bTextureOffset;
 		float	m_faTextureOffset[2];
		bool	m_bBoneTransform;
		bool	m_bCastShadow;
		bool	m_bReceiveShadow;
	
	
		ZMaterial();
		~ZMaterial();
		ZMaterial& operator=(const ZMaterial& kOther);


		bool Create(const string& strName);	//	for resource system
		int  CalculateSize();				
	
		int GetID() 								{ return m_iID;						};
		int GetNrOfPasses() const 				{ return int(m_kPasses.size());	};
		string GetName() const					{ return m_strName;					};
		void SetName(const string& strName)	{	m_strName = strName;				};
		ZMaterialSettings* GetPass(int iPass) const;
		ZMaterialSettings* AddPass();
				
		bool LoadMaterial(string strName,bool bDontClear=false);
		bool LoadIniMaterial(const string& strFile,bool bDontClear);
		bool LoadLuaMaterial(const string& strFile,bool bDontClear);
		
		void Clear();
	
		
		//material loading stuff 
		bool LuaMaterialEndPass(int iPass);
		void SetParameterValue(const string& strName,const string& strValue);
		string GetParameterValue(const string& strName);
		
				
	friend class ZShaderSystem;
};

class ZSSZeroFps;

namespace SI_ZMATERIAL
{
	extern RENDER_API ZFScript*			g_pkMaterialScript;
	extern RENDER_API ZMaterial*			g_pkCurrentMaterial;	
	extern RENDER_API int					g_iCurrentMaterialPass;	
	extern RENDER_API bool					g_bMaterialLoadError;		///< set to true after material load failed		

	
	extern RENDER_API ZSSScriptSystem*	g_pkScript;
	extern RENDER_API ZSSZeroFps*			g_pkZeroFps;
	extern RENDER_API ZShaderSystem*		g_pkZShaderSystem;
	
	
}

RENDER_API ZFResource* Create__Material();
RENDER_API void RegisterSI_Material();

#endif


