#ifndef _ZSHADERSYSTEM_H_
#define _ZSHADERSYSTEM_H_

#include "render_x.h"
#include <iostream>
#include "zmaterial.h"
#include "../basic/zfsystem.h"
#include "../basic/vector2.h"
#include "../basic/vector3.h"
#include "../basic/vector4.h"
#include "../ogl/zfpsgl.h"
#include "texturemanager.h"
#include "frustum.h"
#include "light.h"
#include "../engine/res_texture.h"
#include "zvprogram.h"
#include "zfprogram.h"

using namespace std;



class RENDER_API ZShaderSystem : public ZFSubSystem
{
	private:
		TextureManager*	m_pkTexMan;
		Light*				m_pkLight;	
		
		

	public:
		ZShaderSystem();
		bool StartUp();
		bool ShutDown();
		bool IsValid() 	{return true;};
		

};




#endif



