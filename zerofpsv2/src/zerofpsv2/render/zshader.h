#ifndef _ZSHADER_H_
#define _ZSHADER_H_

#include <iostream>
#include "zmaterial.h"
#include "render_x.h"
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

using namespace std;
class ZMaterial;
class ZMaterialSettings;

enum RENDER_API FORCE_LIGHTING
{
	LIGHT_ALWAYS_ON,
	LIGHT_ALWAYS_OFF,
	LIGHT_MATERIAL,
};

enum RENDER_API FORCE_BLENDING
{
	BLEND_FORCE_TRANSPARENT,
	BLEND_MATERIAL,
};

enum RENDER_API BLEND_SRC
{
	ZERO_BLEND_SRC						= 0,
	ONE_BLEND_SRC						= 1,
	DST_COLOR_BLEND_SRC				= 2,
	ONE_MINUS_DST_COLOR_BLEND_SRC	= 3,
	SRC_ALPHA_BLEND_SRC				= 4,
	ONE_MINUS_SRC_ALPHA_BLEND_SRC	= 5,
	DST_ALPHA_BLEND_SRC				= 6,
	ONE_MINUS_DST_ALPHA_BLEND_SRC	= 7,
	SRC_ALPHA_SATURATE_BLEND_SRC  = 8,
};

enum RENDER_API BLEND_DST
{
	ZERO_BLEND_DST						= 0,
	ONE_BLEND_DST						= 1,
	SRC_COLOR_BLEND_DST				= 2,
	ONE_MINUS_SRC_COLOR_BLEND_DST	= 3,
	SRC_ALPHA_BLEND_DST				= 4,
	ONE_MINUS_SRC_ALPHA_BLEND_DST	= 5,
	DST_ALPHA_BLEND_DST				= 6,
	ONE_MINUS_DST_ALPHA_BLEND_DST	= 7,
};


enum RENDER_API POLYGON_MODES
{
	FILL_POLYGON 	= 0,
	LINE_POLYGON 	= 1,
	POINT_POLYGON	= 2,	d
};

enum RENDER_API DEPTH_FUNC
{
	NEVER_DEPTH		=0,
	LESS_DEPTH		=1,		
	EQUAL_DEPTH		=2,
	LEQUAL_DEPTH	=3,
	GREATER_DEPTH	=4,
	NOTEQUAL_DEPTH =5,
	GEQUAL_DEPTH	=6,
	ALWAYS_DEPTH	=7,
};

enum RENDER_API TU_TEXCORDS
{
	CORDS_FROM_ARRAY_0	=0,
	CORDS_FROM_ARRAY_1	=1,	
	CORDS_FROM_ARRAY_2	=2,	
	CORDS_FROM_ARRAY_3	=3,	

	CORDS_OBJECT_LINEAR	=4,
	CORDS_EYE_LINEAR		=5,
	CORDS_SPHERE_MAP		=6,
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

/**	\brief	ZeroFPS Shader SubSystem.
		\ingroup Render

The shader SubSystem handles the material that are assigned to meshes. Each material
contain the texture to use and all options for it. 
*/
class RENDER_API ZShader : public ZFSubSystem
{
	private:
		/*struct Vector2
		{
			float x;
			float y;			
		};*/
		
		//systems
		TextureManager*	m_pkTexMan;
		Light*				m_pkLight;		
		
		//curent material in use
		ZMaterial*		m_pkCurentMaterial;	
		
		//pointer to geometry data
		Vector3*			m_pkVertexPointer;
		Vector3*			m_pkNormalPointer;	
		Vector2*			m_pkTexturePointer0;
		Vector2*			m_pkTexturePointer1;		
		Vector2*			m_pkTexturePointer2;		
		Vector2*			m_pkTexturePointer3;		
		unsigned int*	m_pkIndexPointer;
		Vector4*			m_pkColorPointer;
		
		//bakup pointes, used when copying data
		Vector3*			m_pkBakupVertexPointer;
		Vector3*			m_pkBakupNormalPointer;
		Vector3*			m_pkBackupTexturePointer;
		Vector2*			m_pkBakupTexturePointer0;
		Vector2*			m_pkBakupTexturePointer1;		
		Vector2*			m_pkBakupTexturePointer2;		
		Vector2*			m_pkBakupTexturePointer3;		
		unsigned int*	m_pkBakupIndexPointer;
		Vector4*			m_pkBakupColorPointer;
		
		//nr of vertexs
		int			m_iNrOfVertexs;				
		int			m_iNrOfIndexes;						
		
		//mode to use (depending on underlying system , etc this may be opengl types)
		int			m_iDrawMode;
		
		//force lighting to this
		int			m_iForceLighting;		
		
		//force blending
		int			m_iForceBledning;
		
		//did we copy the data?
		bool			m_bCopyedData;
	
		//do we have vertex program support
		bool			m_bVertexProgram;
	
		//vertex program is NO_VPROGRAM if no vertex program is active
		int			m_iCurrentVertexProgram;
		
	
		//this enable/disable client states if pointers have been set
		void SetupClientStates();			
		
		//configure setup, stuff before setting up a pass, acording to current material
		void SetupPrerenderStates();
		
		//configure render states/pass
		void SetupRenderStates(ZMaterialSettings* pkSettings);
		
		void SetupTU(ZMaterialSettings* pkSettings,int iTU);
		void SetupVertexProgram(ZMaterialSettings* pkSettings);
		
		//makes a memcopy of all none null pointers data, and sets the pointers to the new location
		void CopyVertexData();
		void CopyData(void** pkData,int iSize);
		void CleanCopyedData();
		
		//effects
		void RandomVertexMovements();
		void Waves();	
		void TextureOffset();
	
		bool HaveVertexProgramExt();
	
	public:
		ZShader();
		bool StartUp();
	
		void SetForceLighting(int iMode) { m_iForceLighting = iMode;};
		void SetForceBlending(int iMode) { m_iForceBledning = iMode;};
		void SetMaterial(ZMaterial* pkMaterial);		
		void SetPointer(int iType,void* pkPointer);
		void SetNrOfVertexs(int iNr);
		void SetNrOfIndexes(int iNr);		
		void SetDrawMode(int iDrawMode);
		
		void SetVertexProgram(int iVPID);
		int  GetCurrentVertexProgram() {return m_iCurrentVertexProgram;};
		bool SupportVertexProgram() { return m_bVertexProgram;};
		
		
		void Reset();
		void Draw();

		bool ShutDown();
		bool IsValid();

};



#endif


