#ifndef _ZSHADER_H_
#define _ZSHADER_H_

#include <iostream>
#include "zmaterial.h"
#include "render_x.h"
#include "../basic/zfobjectmanger.h"
#include "../basic/vector3.h"
#include "../basic/vector4.h"
#include <GL/gl.h>
#include "texturemanager.h"
#include "frustum.h"
#include "light.h"

using namespace std;

enum RENDER_API POINTER_TYPE
{
	VERTEX_POINTER,
	NORMAL_POINTER,
	TEXTURE_POINTER,	
	INDEX_POINTER,	
	COLOR_POINTER,
	
};

enum RENDER_API DRAW_MODE
{
	POINTS_MODE,
	LINES_MODE,
	LINESTRIP_MODE,	
	LINELOOP_MODE,
	TRIANGLES_MODE,	
	TRIANGLESTRIP_MODE,
	TRIANGLEFAN_MODE,
	QUADS_MODE,
	QUADSTRIP_MODE,
	POLYGON_MODE,
};

class RENDER_API ZShader : public ZFObject
{
	private:
		struct Vector2
		{
			float x;
			float y;			
		};
		
		//systems
		Frustum*				m_pkFrustum;
		TextureManager*	m_pkTexMan;
		Light*				m_pkLight;		
		
		//curent material in use
		ZMaterial*	m_pkCurentMaterial;	
		
		//pointer to geometry data
		Vector3*		m_pkVertexPointer;
		Vector3*		m_pkNormalPointer;	
		Vector2*		m_pkTexturePointer;
		int*			m_pkIndexPointer;
		Vector4*		m_pkColorPointer;
		
		//nr of vertexs
		int			m_iNrOfVertexs;				
		
		//mode to use (depending on underlying system , etc this may be opengl types)
		int			m_iDrawMode;
	
		//this enable/diable client states if pointers have been set
		void SetupClientStates();			
	
	public:
		ZShader();
		bool StartUp();
	
		void BindMaterial(ZMaterial* pkMaterial);		
		void SetPointer(int iType,void* pkPointer);
		void SetNrOfVertexs(int iNr);
		void SetDrawMode(int iDrawMode);
		void Reset();
		void Draw();
		
};



#endif


