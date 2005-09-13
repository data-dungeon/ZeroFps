#ifndef _ZFPROGRAM_H_
#define _ZFPROGRAM_H_

#include <iostream>
#include "render_x.h"
#include <vector>
#include "../ogl/zfpsgl.h"
#include "../basic/zfresource.h"
#include "../basic/zfvfs.h"
#include "zshadersystem.h"

using namespace std;


#define	NO_FPROGRAM	-1

/**	\brief	A OpenGL Fragment Program.
		\ingroup Render

This is a OpenGL fragment program that can be assigned to a material and used
in the rendering.
*/
class RENDER_API ZFProgram : public ZFResource
{
	private:
		ZShaderSystem*	m_pkShader;
	
		bool	SetupShaderPointer();
		
	public:
		string	m_sFileName;
		int		m_iId;


		ZFProgram();
		~ZFProgram();
		
		bool Create(const string&  strName);
		int CalculateSize();


		bool Load(string  strFile);
		bool UnLoad();
		
};


RENDER_API ZFResource* Create__ZFProgram();


#endif
