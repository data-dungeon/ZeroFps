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
		
		Vector4	m_kVertexColor;
		bool		m_bColorMaterial;
		
		int		m_iTUTexCords[4];
		
		int		m_iPolygonModeFront;
		int		m_iPolygonModeBack;		
		
		int		m_iDepthFunc;
		
		bool		m_bLighting;	
		bool		m_bCullFace;

		bool		m_bAlphaTest;
	
		bool		m_bBlend;

		bool		m_bDepthTest;

		int		m_iBlendSrc;
		int		m_iBlendDst;
		
		//software effects
		int		m_iTextureColorEffect;
		
		//----
		
	
		ZMaterialSettings();
		~ZMaterialSettings();
};

/**	\brief	A Material (Used by the shader SubSys).
		\ingroup Render

A ZMaterial contains all the settings used for a surface or something that need
to be rendered in the engine. 
*/
class RENDER_API ZMaterial : public ZFResource
{
	private:
		
		vector<ZMaterialSettings*> m_kPasses;
		ZFIni								m_kIni;
		static map<string,int> 		m_kEnums;
		
		bool LoadGlobalSection();
		bool LoadPass(int iPass);		
		int GetTranslateEnum(string strEnum);
		void SetupEnums();
		
	public:		
		bool	m_bCopyData;
		
		//software effects
		bool	m_bRandomMovements;
		bool	m_bWaves;
		bool	m_bTextureOffset;
		float	m_faTextureOffset[2];
		//----
		
		ZMaterial();
		~ZMaterial();

		bool Create(string strName);	//	for resource system
		int  CalculateSize();		
		
	
		int GetNrOfPasses() {return m_kPasses.size();};
		ZMaterialSettings* GetPass(int iPass);
		ZMaterialSettings* AddPass();
		
		
		bool LoadShader(const char* acFile);
		void Clear();
	
	friend class ZShader;
};

RENDER_API ZFResource* Create__Material();


#endif


