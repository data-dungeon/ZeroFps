#ifndef _ZVPROGRAM_H_
#define _ZVPROGRAM_H_

#include <iostream>
#include "render_x.h"
#include <vector>
#include "../ogl/zfpsgl.h"
#include "../basic/zfresource.h"
#include "../basic/zfvfs.h"
#include "zshader.h"

using namespace std;


#define	NO_VPROGRAM	-1

class RENDER_API ZVProgram : public ZFResource
{
	private:
		ZShader*	m_pkShader;
	
		bool	SetupShaderPointer();
		
	public:
		string	m_sFileName;
		int		m_iId;


		ZVProgram();
		~ZVProgram();
		
		bool Create(string  strName);
		bool Load(string  strFile);
		bool UnLoad();
		
};


RENDER_API ZFResource* Create__ZVProgram();


#endif



