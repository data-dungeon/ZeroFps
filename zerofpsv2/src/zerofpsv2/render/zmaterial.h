#ifndef _ZMaterial_H_
#define _Material_H_

#include <iostream>
#include "render_x.h"
#include <vector>
#include <GL/gl.h>

using namespace std;

class RENDER_API ZMaterialSettings
{
	public:
		int	m_iTUs[4];		
		
		int	m_iPolygonModeFront;
		int	m_iPolygonModeBack;		
		
		int	m_iDepthFunc;
		
		bool	m_bLighting;	
		bool	m_bCullFace;

	
		ZMaterialSettings();

};


class RENDER_API ZMaterial
{
	private:
		vector<ZMaterialSettings> m_kPasses;
		
	public:		
		bool	m_bCopyData;
		bool	m_bRandomMovements;
		bool	m_bWaves;		
				
		
		ZMaterial();
	
		int GetNrOfPasses() {return m_kPasses.size();};
		ZMaterialSettings* GetPass(int iPass);
		ZMaterialSettings* AddPass();
		
	
	friend class ZShader;
};



#endif


