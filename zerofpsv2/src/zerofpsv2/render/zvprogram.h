#ifndef _ZVPROGRAM_H_
#define _ZVPROGRAM_H_

#include <iostream>
#include "render_x.h"
#include <vector>
#include "../ogl/zfpsgl.h"
#include "../basic/zfresource.h"
#include "../basic/zfvfs.h"
#include "zshadersystem.h"

using namespace std;


#define	NO_VPROGRAM	-1

/**	\brief	A OpenGL Vertex Program.
		\ingroup Render

This is a OpenGL vertex program that can be assigned to a material and used
in the rendering.
*/
class RENDER_API ZVProgram : public ZFResource
{
	private:
		ZShaderSystem*	m_pkShader;
	
		bool	SetupShaderPointer();
		
	public:
		int		m_iId;


		ZVProgram();
		~ZVProgram();
		
		bool Create(string  strName);
		int CalculateSize();


		bool Load(string  strFile);
		bool UnLoad();
		
};


RENDER_API ZFResource* Create__ZVProgram();


#endif



