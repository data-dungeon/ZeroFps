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
		
	
		ZMaterialSettings()
		{
			m_iTUs[0] = -1;
			m_iTUs[1] = -1;		
			m_iTUs[2] = -1;			
			m_iTUs[3] = -1;			
			
			m_iPolygonModeFront = GL_LINE;
			m_iPolygonModeBack = GL_LINE;			
		};
};


class RENDER_API ZMaterial
{
	private:
		vector<ZMaterialSettings> m_kPasses;
		
	public:		
		ZMaterial();
	
		int GetNrOfPasses() {return m_kPasses.size();};
		ZMaterialSettings* GetPass(int iPass);
		ZMaterialSettings* AddPass();
		
	
	friend class ZShader;
};



#endif


