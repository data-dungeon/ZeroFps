#ifndef _ZMaterial_H_
#define _ZMaterial_H_

#include <iostream>
#include "render_x.h"
#include <vector>
#include "../ogl/zfpsgl.h"
#include "../basic/zfini.h"



using namespace std;

class RENDER_API ZMaterialSettings
{
	public:
		int	m_iTUs[4];		
		
		int	m_iTUTexCords[4];
		
		int	m_iPolygonModeFront;
		int	m_iPolygonModeBack;		
		
		int	m_iDepthFunc;
		
		bool	m_bLighting;	
		bool	m_bCullFace;

		bool	m_bAlphaTest;
	
		ZMaterialSettings();

};


class RENDER_API ZMaterial
{
	private:
		vector<ZMaterialSettings> m_kPasses;
		ZFIni		m_kIni;
		
	public:		
		bool	m_bCopyData;
		bool	m_bRandomMovements;
		bool	m_bWaves;		
				
		
		ZMaterial();
	
		int GetNrOfPasses() {return m_kPasses.size();};
		ZMaterialSettings* GetPass(int iPass);
		ZMaterialSettings* AddPass();
		
		
		bool LoadShader(const char* acFile);
		void Clear();
	
	friend class ZShader;
};



#endif


