#ifndef _ZMaterial_H_
#define _ZMaterial_H_

#include <iostream>
#include "render_x.h"
#include <vector>
#include "../ogl/zfpsgl.h"
#include "../basic/zfini.h"

#include "../basic/zfresource.h"

using namespace std;

/// Settings for one pass in a Material.
class RENDER_API ZMaterialSettings
{
	public:
		ZFResourceHandle*	m_kTUs[4];	
		ZFResourceHandle*	m_pkVP;
		
		int	m_iTUTexCords[4];
		
		int	m_iPolygonModeFront;
		int	m_iPolygonModeBack;		
		
		int	m_iDepthFunc;
		
		bool	m_bLighting;	
		bool	m_bCullFace;

		bool	m_bAlphaTest;
	
		bool	m_bBlend;

      bool m_bDepthTest;

		int	m_iBlendSrc;
		int	m_iBlendDst;
	
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
		ZFIni		m_kIni;
		
		bool LoadGlobalSection();
		bool LoadPass(int iPass);
		
	public:		
		bool	m_bCopyData;
		bool	m_bRandomMovements;
		bool	m_bWaves;		
				
		
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


