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
		ZFResourceHandle*	m_kTUs[4];	
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
		
	
		ZMaterialSettings();
		~ZMaterialSettings();
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
		ZFScriptSystem*				m_pkScript;
		
		vector<ZMaterialSettings*> m_kPasses;		//material passes
		ZFIni								m_kIni;			//inifile
		ZFScript*						m_pkMaterialScript;
		
		static map<string,int> 		m_kEnums;		//enums for loading
		
		
		static int	m_iNextID;							//next created material will get this id
		int			m_iID;								//uniq id for this material
		
		string		m_strName;
		
		
		bool LoadGlobalSection();
		bool LoadPass(int iPass);		
		int GetTranslateEnum(string strEnum);
		void SetupEnums();
		
		bool PassGetLuaDouble(char* czName,double& dTemp);
		bool PassGetLuaChar(char* czName,char* cTemp);
		
	public:		
		//global material settings
		bool	m_bRandomMovements;
		bool	m_bWaves;
		bool	m_bTextureOffset;
		float	m_faTextureOffset[2];
		
	
	
		ZMaterial();
		~ZMaterial();

		bool Create(string strName);	//	for resource system
		int  CalculateSize();				
	
		int GetID() 			{return m_iID;};
		int GetNrOfPasses() 	{ return int(m_kPasses.size()); }
		ZMaterialSettings* GetPass(int iPass);
		ZMaterialSettings* AddPass();
				
		bool LoadIniMaterial(const char* acFile);
		bool LoadLuaMaterial(const char* acFile);
		
		void Clear();
	
		
		//material loading stuff 
		void LuaMaterialEndPass(int iPass);

				
	friend class ZShaderSystem;

};

class ZeroFps;

namespace SI_ZMATERIAL
{
	extern RENDER_API ZMaterial*			g_pkCurrentMaterial;	
	extern RENDER_API int					g_iCurrentMaterialPass;	
	
	extern RENDER_API ZFScriptSystem*	g_pkScript;
	extern RENDER_API ZeroFps*				g_pkZeroFps;
	
	int RENDER_API PassBeginLua(lua_State* pkLua);
	int RENDER_API PassEndLua(lua_State* pkLua);
}

RENDER_API ZFResource* Create__Material();
RENDER_API void RegisterSI_Material();

#endif


