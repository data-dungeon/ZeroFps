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
		
		int				m_iTUTexCords[4];
		
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
		
		//software effects
		int				m_iTextureColorEffect;
		
	
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
		vector<ZMaterialSettings*> m_kPasses;		//material passes
		ZFIni								m_kIni;			//inifile
		static map<string,int> 		m_kEnums;		//enums for loading
		
		
		static int	m_iNextID;							//next created material will get this id
		int			m_iID;								//uniq id for this material
		
		string		m_strName;
		
		
		bool LoadGlobalSection();
		bool LoadPass(int iPass);		
		int GetTranslateEnum(string strEnum);
		void SetupEnums();
		
	public:		
		//global material settings
		bool	m_bCopyData;
		
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
				
		bool LoadShader(const char* acFile);
		void Clear();
	
		
		
	friend class ZShaderSystem;
};

RENDER_API ZFResource* Create__Material();


#endif


