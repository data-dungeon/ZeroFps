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

enum RENDER_API STENCIL_FUNC
{
	STENCILFUNC_NEVER=		0,
	STENCILFUNC_LESS=			1,
	STENCILFUNC_LEQUAL=		2,
	STENCILFUNC_GREATER=		3,
	STENCILFUNC_GEQUAL=		4,
	STENCILFUNC_EQUAL=		5,
	STENCILFUNC_NOTEQUAL=	6,
	STENCILFUNC_ALWAYS=		7,
};

enum RENDER_API STENCILOP
{
	STENCILOP_KEEP=		0,
	STENCILOP_ZERO=		1,
	STENCILOP_REPLACE=	2,
	STENCILOP_INCR=		3,
	STENCILOP_DECR=		4,
	STENCILOP_INVERT=		5,
};


enum RENDER_API BUFFERS
{
	COLOR_BUFFER=		0,
	DEPTH_BUFFER=		1,
	ACCUM_BUFFER=		2,
	STENCIL_BUFFER=	3,
};

enum RENDER_API CULL_FACE
{
	CULL_FACE_NONE=	0,
	CULL_FACE_BACK=	1,
	CULL_FACE_FRONT=	2,
	CULL_FACE_ALL=		3,
};

enum RENDER_API MATRIX_MODE
{
	MATRIX_MODE_MODEL			= 0,
	MATRIX_MODE_PROJECTION	= 1,
	MATRIX_MODE_TEXTURE		= 2,
	MATRIX_MODE_COLOR			= 3,	
};

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
	POINT_POLYGON	= 2,
};

enum RENDER_API DEPTHFUNC
{
	DEPTHFUNC_NEVER	=0,
	DEPTHFUNC_LESS		=1,		
	DEPTHFUNC_EQUAL	=2,
	DEPTHFUNC_LEQUAL	=3,
	DEPTHFUNC_GREATER	=4,
	DEPTHFUNC_NOTEQUAL=5,
	DEPTHFUNC_GEQUAL	=6,
	DEPTHFUNC_ALWAYS	=7,
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


class RENDER_API ZShaderSystem : public ZFSubSystem
{
	private:
		TextureManager*	m_pkTexMan;
		Light*				m_pkLight;	
		
		//current selected material
		ZMaterial*			m_pkCurrentMaterial;

		//hardware shader data
		bool				m_bSupportVertexProgram;
		bool				m_bSupportFragmentProgram;
		int				m_iCurrentVertexProgram;
		int				m_iCurrentFragmentProgram; 
		
		
		//temporary geometry data		
		vector<Vector3>	m_kVerties;
		vector<Vector3>	m_kNormals;
		vector<Vector2>	m_kTexture[4];
		
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
		Vector2*			m_pkBakupTexturePointer0;
		Vector2*			m_pkBakupTexturePointer1;		
		Vector2*			m_pkBakupTexturePointer2;		
		Vector2*			m_pkBakupTexturePointer3;		
		unsigned int*	m_pkBakupIndexPointer;
		Vector4*			m_pkBakupColorPointer;		
		
		//nr of vertexs
		int			m_iNrOfVertexs;
		int			m_iNrOfIndexes;
		
		//polygon mode to use
		int			m_iDrawMode;		
		
		//have we done a vertex data copy
		bool			m_bCopyedData;
		
		// pushes - pops
		int			m_iPushPop;
		
		//statistics
		int			m_iMaterialReloads;
		int			m_iMaterialBinds;
		int			m_iSavedReloads;
		int			m_iGLupdates;
		int			m_iTotalVertises;
		
		//basic
		void SetupGlobalSettings();
		void SetupArrayClientStates();
		void SetupTUClientStates(const int& iPass);
		void SetupPass(int iPass);
		void SetupTU(ZMaterialSettings* pkSettings,int iTU);
		
		//hardware shaders
		void SetupVertexProgram(ZMaterialSettings* pkSettings);
		void SetupFragmentProgram(ZMaterialSettings* pkSettings);
		void UpdateFragmentProgramParameters();		
		void UpdateVertexProgramParameters();		
		void SetVertexProgram(const int& iVPID);
		void SetFragmentProgram(const int& iFPID);		
		int GetCurrentVertexProgram() 					{return m_iCurrentVertexProgram;};
		int GetCurrentFragmentProgram()					{return m_iCurrentFragmentProgram;};
		
		//vertex transform
		void VertexTransform();
		void CopyVertexData();
		void CopyData(void** pkData,int iSize);
		void CleanCopyedData();
		
		//vertex transform funktions
		void Waves();	
		
		
		
	public:
		ZShaderSystem();
		bool StartUp();
		bool ShutDown();
		bool IsValid() 	{return true;};
		
		//statistics
		int GetMaterialReloads() 	{return m_iMaterialReloads;};
		int GetMaterialBinds() 		{return m_iMaterialBinds;};
		int GetSavedReloads() 		{return m_iSavedReloads;};
		int GetGLupdates() 			{return m_iGLupdates;};
		int GetTotalVertises() 		{return m_iTotalVertises;};
		void ResetStatistics() 		{m_iMaterialReloads = 0;
											m_iMaterialBinds =	0;
											m_iSavedReloads = 	0;
											m_iGLupdates = 		0;
											m_iTotalVertises = 	0;};
															
		//basic
		void Push(const char* czNote);
		void Pop();		
		void BindMaterial(ZMaterial* pkMaterial);
		void ReloadMaterial();
		
		//other stuff
		void ClearBuffer(const int& iBuffert);
		
		//information
		bool HaveExtension(const string& strExt);
		int GetStencilBits();
		bool SupportVertexProgram() 		{return m_bSupportVertexProgram;};
		bool SupportFragmentProgram() 	{return m_bSupportFragmentProgram;};
		
		//arrays
		void ResetPointers();
		void SetPointer(int iType,void* pkPointer);
		void SetNrOfVertexs(const int& iNr)					{m_iNrOfVertexs = iNr;};
		void SetNrOfIndexes(const int& iNr)					{m_iNrOfIndexes = iNr;};
		void SetDrawMode(const int& iDrawMode);
		void DrawArray();
		void DrawArray(const int& iDrawMode);
	
		//basic draw funktions
		void ClearGeometry();
		void DrawGeometry();
		void DrawGeometry(const int& iDrawMode);
		
		//line 2V
		void AddLineV(const Vector3& kPos1,const Vector3& kPos2);
		void AddLineN(const Vector3& kNormal1,const Vector3& kNormal2);
		void AddLineUV(const Vector2& kPos1,const Vector2& kPos2,const int& iTU=0);
		
		//triangle 3V
		void AddTriangleV(const Vector3& kPos1,const Vector3& kPos2,const Vector3& kPos3);
		void AddTriangleN(const Vector3& kNormal1,const Vector3& kNormal2,const Vector3& kNormal3);
		void AddTriangleUV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const int& iTU=0);	
		
		//quad 4V
		void AddQuadV(const Vector3& kPos1,const Vector3& kPos2,const Vector3& kPos3,const Vector3& kPos4);
		void AddQuadN(const Vector3& kNormal1,const Vector3& kNormal2,const Vector3& kNormal3,const Vector3& kNormal4);
		void AddQuadUV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const Vector2& kPos4,const int& iTU=0);	

				
		//matrix manipulation
		void MatrixMode(const int& iMode);
		void MatrixIdentity()								{glLoadIdentity();};
		void MatrixTranslate(const Vector3& kPos)		{glTranslatef(kPos.x,kPos.y,kPos.z);};
		void MatrixRotate(const Vector3& kRot)			{glRotatef(kRot.x,1,0,0);
																	 glRotatef(kRot.y,0,1,0);
																	 glRotatef(kRot.z,0,0,1);};																		
		void MatrixScale(const Vector3& kScale)		{glScalef(kScale.x,kScale.y,kScale.z);};
		void MatrixScale(const float& fScale)			{glScalef(fScale,fScale,fScale);};
		void MatrixMult(const Matrix4& kMatrix)		{glMultMatrixf((float*)&kMatrix);};
		void MatrixPop()										{glPopMatrix();};
		void MatrixPush()										{glPushMatrix();};
		
		void MatrixGenerateOrtho(	const float& fLeft,const float& fRight,const float& fBottom,
											const float& fTop,const float& fNear,const float& fFar)
																	{
																		glOrtho(fLeft,fRight,fBottom,fTop,fNear,fFar);
																	};
		void MatrixGeneratePerspective(	const float& fLeft,const float& fRight,const float& fBottom,
													const float& fTop,const float& fNear,const float& fFar)
																	{
																		glFrustum(fLeft,fRight,fBottom,fTop,fNear,fFar);
																	};
		
		friend class ZFProgram;
		friend class ZVProgram;
};




#endif



