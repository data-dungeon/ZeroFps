#ifndef _ZSHADER_H_
#define _ZSHADER_H_

#include <iostream>
#include "zmaterial.h"
#include "render_x.h"
#include "../basic/zfobjectmanger.h"
#include "../basic/vector3.h"
#include "../basic/vector4.h"
#include "../ogl/zfpsgl.h"
#include "texturemanager.h"
#include "frustum.h"
#include "light.h"

using namespace std;
class ZMaterial;
class ZMaterialSettings;

enum RENDER_API TU_TEXCORDS
{
	CORDS_FROM_ARRAY_0,
	CORDS_FROM_ARRAY_1,	
	CORDS_FROM_ARRAY_2,	
	CORDS_FROM_ARRAY_3,	

	CORDS_OBJECT_LINEAR,
	CORDS_EYE_LINEAR,
	CORDS_SPHERE_MAP,
};

enum RENDER_API POINTER_TYPE
{
	VERTEX_POINTER,
	NORMAL_POINTER,
	TEXTURE_POINTER0,	
	TEXTURE_POINTER1,	
	TEXTURE_POINTER2,		
	TEXTURE_POINTER3,		
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


class RENDER_API ZShader : public ZFSubSystem
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
		Vector3*			m_pkVertexPointer;
		Vector3*			m_pkNormalPointer;	
		Vector2*			m_pkTexturePointer0;
		Vector2*			m_pkTexturePointer1;		
		Vector2*			m_pkTexturePointer2;		
		Vector2*			m_pkTexturePointer3;		
		unsigned int*	m_pkIndexPointer;
		Vector4*			m_pkColorPointer;
		
		//nr of vertexs
		int			m_iNrOfVertexs;				
		
		//mode to use (depending on underlying system , etc this may be opengl types)
		int			m_iDrawMode;
		
		//did we copy the data?
		bool			m_bCopyedData;
	
	
	
		//this enable/diable client states if pointers have been set
		void SetupClientStates();			
		
		//configure setup, stuff before setting up a pass, acording to current material
		void SetupPrerenderStates();
		
		//configure render states/pass
		void SetupRenderStates(ZMaterialSettings* pkSettings);
		
		void SetupTU(ZMaterialSettings* pkSettings,int iTU);
		
		//makes a memcopy of all none null pointers data, and sets the pointers to the new location
		void CopyVertexData();
		void CopyData(void** pkData,int iSize);
		void CleanCopyedData();
		
		//effects
		void RandomVertexMovements();
		void Waves();		
	
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


