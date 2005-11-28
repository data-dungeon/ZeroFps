#ifndef _ZSHADERSYSTEM_H_
#define _ZSHADERSYSTEM_H_

#include "render_x.h"
#include <iostream>
#include "zmaterial.h"
#include "../basic/zfsystem.h"
#include "../basic/math.h"
#include "../ogl/zfpsgl.h"
#include "texturemanager.h"
#include "frustum.h"
#include "light.h"
#include "res_texture.h"
#include "zvprogram.h"
#include "zfprogram.h"
// #include "zvertexbuffer.h"
#include "stack"


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
	BLEND_FORCE_OPAQ,
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

enum RENDER_API TEXENV_MODES
{
	TEXENV_MODE_MODULATE		=0,
	TEXENV_MODE_DECAL			=1,
	TEXENV_MODE_BLEND			=2,
	TEXENV_MODE_REPLACE		=3,
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
	VERTEX2D_POINTER,
	TANGENT_POINTER,
	BITANGENT_POINTER,
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

enum RENDER_API FORCE_SETTING
{
	FORCE_DEFAULT = -1,
	FORCE_DISABLE = 0,
	FORCE_ENABLE = 1,
	FORCE_OTHER = 2,
};


class ZVertexBuffer;
class ResTexture;

class RENDER_API ZShaderSystem : public ZFSubSystem
{
	private:
		//consol commands
		enum FuncId_e
		{
			FID_SETGAMMA,
		};
	
	
		TextureManager*	m_pkTexMan;
		ZSSLight*			m_pkLight;	
		
		//current selected material
		const ZMaterial*	m_pkCurrentMaterial;

		//eye position
		Vector3			m_kEyePosition;
		int				m_iShadowmapWidth;
		int				m_iShadowmapHeight;		
				
		//empty material, used when pushing 
		ZMaterial		m_kEmptyMaterial;

		//FOB's
		bool				m_bSupportFBO;

		//hardware shader data
		bool				m_bSupportVertexProgram;
		bool				m_bSupportFragmentProgram;
		int				m_iCurrentVertexProgram;
		int				m_iCurrentFragmentProgram; 
		
		bool				m_bSupportGLSLProgram;
		GLenum			m_iCurrentGLSLProgramID;				
		bool				m_bForceDisableGLSL;

		ConVar			m_kbUseGLSL;
		ConVar			m_kbUseHDR;
		ConVar			m_kbSpecMap;
		ConVar			m_kbNormalMap;



		ZFResourceHandle* m_pkDefaultGLSLProgram;
		bool					m_bUseDefaultGLSLProgram;
		
		//support texturecompression
		bool				m_bSupportARBTC;
		
		//have occulusion support
		bool				m_bOcclusion;
		unsigned int	m_iOcQuery;
		
		//other stuff
		ConVar			m_kfRedGamma;
		ConVar			m_kfGreenGamma;
		ConVar			m_kfBlueGamma;
		

		//vertex buffers
		bool				m_bSupportVertexBuffers;
		
		//temporary geometry data		
		vector<Vector2>	m_kVerties2D;
		vector<Vector3>	m_kVerties;
		vector<Vector3>	m_kNormals;
		vector<Vector2>	m_kTexture[4];
		vector<Vector4>	m_kColors;
		vector<Vector3>	m_kTangents;
		vector<Vector3>	m_kBiTangents;
		
		//pointer to geometry data
		Vector2*			m_pk2DVertexPointer;
		Vector3*			m_pkVertexPointer;
		Vector3*			m_pkNormalPointer;	
		Vector2*			m_pkTexturePointer0;
		Vector2*			m_pkTexturePointer1;		
		Vector2*			m_pkTexturePointer2;		
		Vector2*			m_pkTexturePointer3;		
		unsigned int*	m_pkIndexPointer;
		Vector4*			m_pkColorPointer;		
		Vector3*			m_pkTangentPointer;
		Vector3*			m_pkBiTangentPointer;
		
		//pointer is set
		bool			m_b2DVertexPointer;
		bool			m_bVertexPointer;
		bool			m_bNormalPointer;	
		bool			m_bTexturePointer0;
		bool			m_bTexturePointer1;		
		bool			m_bTexturePointer2;		
		bool			m_bTexturePointer3;		
		bool			m_bIndexPointer;
		bool			m_bColorPointer;
		bool			m_bTangentPointer;
		bool			m_bBiTangentPointer;
		
		//bakup pointes, used when copying data
		Vector2*			m_pkBakup2DVertexPointer;
		Vector3*			m_pkBakupVertexPointer;
		Vector3*			m_pkBakupNormalPointer;
		Vector2*			m_pkBakupTexturePointer0;
		Vector2*			m_pkBakupTexturePointer1;		
		Vector2*			m_pkBakupTexturePointer2;		
		Vector2*			m_pkBakupTexturePointer3;		
		unsigned int*	m_pkBakupIndexPointer;
		Vector4*			m_pkBakupColorPointer;		
		Vector3*			m_pkBakupTangentPointer;		
		Vector3*			m_pkBakupBiTangentPointer;		
		
		//HDR
		float			m_fExposure;										///< exposure factor sent to shaders
		bool			m_bSupportHDR;
		
		//current fog default setting
		bool			m_bFogSetting;
		
		//nr of vertexs
		int			m_iNrOfVertexs;
		int			m_iNrOfIndexes;
		
		//polygon mode to use
		GLenum		m_iGLDrawMode;	//gl mode
		DRAW_MODE	m_eDrawMode;		
		
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
		int			m_iRenderedVBOs;
		
		//force settings
		int				m_iForceCullFace;
		FORCE_SETTING	m_iForceColorMask;
		FORCE_SETTING	m_iForceAlphaTest;
		int				m_iForceLighting;
		bool				m_bDisableTU3;
		int				m_iForceBlend;
		
		//texture settings
		stack<GLuint>	m_kTextureStacks[4];
		GLuint			m_aiCurrentTextures[4];
		
		//basic		
		void SetupOpenGL();
		void SetupGlobalSettings();
		void SetupArrayClientStates();
		void SetupTUClientStates(const int& iPass);
		void SetupPass(int iPass);
		void SetupTU(ZMaterialSettings* pkSettings,int iTU);
		
		//hardware shaders
		void SetupGLSLProgram(ZMaterialSettings* pkSettings);
		void UpdateGLSLProgramParameters(int iPass);
		
		void SetupVertexProgram(ZMaterialSettings* pkSettings);
		void SetupFragmentProgram(ZMaterialSettings* pkSettings);
		void UpdateFragmentProgramParameters();		
		void UpdateVertexProgramParameters();		
		void SetVertexProgram(const int& iVPID);
		void SetFragmentProgram(const int& iFPID);		
		int GetCurrentVertexProgram() 					{return m_iCurrentVertexProgram;};
		int GetCurrentFragmentProgram()					{return m_iCurrentFragmentProgram;};
		
		//occlusion
		void SetupOcculusion();
				
		//vertex transform
		void VertexTransform();
		void CopyVertexData();
		void CopyData(void** pkData,int iSize);
		void CleanCopyedData();
		void CopyPointerData(int iPointer);
		
		//vertex transform funktions
		void Waves();	
		void TextureOffset();
		
		
	public:
		ZShaderSystem();
		bool StartUp();
		bool ShutDown();
		bool IsValid() 	{return true;};
		void RunCommand(int cmdid, const ConCommandLine* kCommand);
		
		//statistics
		int GetRenderedVBOs()		{return m_iRenderedVBOs;};
		int GetMaterialReloads() 	{return m_iMaterialReloads;};
		int GetMaterialBinds() 		{return m_iMaterialBinds;};
		int GetSavedReloads() 		{return m_iSavedReloads;};
		int GetGLupdates() 			{return m_iGLupdates;};
		int GetTotalVertises() 		{return m_iTotalVertises;};
		void ResetStatistics() 		{m_iMaterialReloads = 0;
											m_iMaterialBinds =	0;
											m_iSavedReloads = 	0;
											m_iGLupdates = 		0;
											m_iTotalVertises = 	0;
											m_iRenderedVBOs =		0;
											};

		//hardware shader stuff
		void UseDefaultGLSLProgram(bool bUse)					{	m_bUseDefaultGLSLProgram = bUse;	}
		bool GetUseDefaultGLSLProgram()							{	return m_bUseDefaultGLSLProgram;	}
		void SetDefaultGLSLProgram(ZFResourceHandle* pkRH)	{	m_pkDefaultGLSLProgram = pkRH;	}
		ZFResourceHandle* GetDefaultGLSLProgram()				{	return m_pkDefaultGLSLProgram;	}
		void SetForceDisableGLSL(bool bDisable)				{	m_bForceDisableGLSL = bDisable;	}
		bool GetForceDisableGLSL()									{	return m_bForceDisableGLSL;		}
										
		//texture management
		void BindTexture(const ResTexture* pkTexture);
		void PushTexture();
		void PopTexture();
																										
		//basic
		void Push(const char* czNote);
		void Pop();		
		void BindMaterial(const ZMaterial* pkMaterial,bool bForceReload = false);
		void ReloadMaterial();
		
		
		//other stuff
		void ClearBuffer(const int& iBuffert);
		void SetClearColor(const Vector4& kColor);
		void SetDepthMask(bool bMask)							{	if(bMask) glDepthMask(GL_TRUE); else glDepthMask(GL_FALSE);}
		void SetFog(const Vector4& kColor,float fStart,float fStop,bool bEnabled);
		unsigned int GetDepth(int iX,int iY);
		
		void SetShadowMapSize(int iWidth,int iHeight)			{	m_iShadowmapWidth = iWidth; m_iShadowmapHeight = iHeight;	}
		void SetEyePosition(const Vector3& kEyePos)				{	m_kEyePosition = kEyePos;				}		
		void SetExposure(float f)										{	m_fExposure = f;					}
		float GetExposure()												{	return m_fExposure;				}
		bool SetGamma(float fGamma);
		bool SetGamma(float fRed,float fGreen,float fBlue);
		void GetGamma(float& fRed,float& fGreen,float& fBlue)	{	fRed =   m_kfRedGamma.GetFloat();
																					fGreen = m_kfGreenGamma.GetFloat();
																					fBlue =  m_kfBlueGamma.GetFloat();	}
		
		void SetNormal(const Vector3& kNormal)				{	glNormal3fv(&kNormal.x);				}
		void SetColor(const Vector4& kColor)				{	glColor4fv(&kColor.x);					}
																					
		//ucculusion
		void OcculusionBegin();
		unsigned int  OcculusionEnd();

		//force settings
		void ForceColorMask(FORCE_SETTING iMask)			{	m_iForceColorMask = iMask;				}
		void ForceAlphaTest(FORCE_SETTING iAlphaTest)	{	m_iForceAlphaTest = iAlphaTest;		}		
		void ForceLighting(int iLight)						{	m_iForceLighting = iLight;				}
		void ForceBlending(int iBlend)						{	m_iForceBlend = iBlend;					}
		void ForceTU3Disabled(bool bDisableTU3)			{	m_bDisableTU3 = bDisableTU3;			}
		
		FORCE_SETTING 	GetForceColorMask()					{	return m_iForceColorMask;				}
		FORCE_SETTING 	GetForceAlphaTest()					{	return m_iForceAlphaTest;				}
		int 				GetForceLighting()					{	return m_iForceLighting;				}
		int 				GetForceBlending()					{	return m_iForceBlend;					}
		bool 				GetForceTU3Disabled()				{	return m_bDisableTU3;					}
		
		//information
		bool HaveExtension(const string& strExt);
		int GetStencilBits();
		bool GetUseGLSL()											{	return m_kbUseGLSL.GetBool();						}
		bool SupportVertexProgram() 							{	return m_bSupportVertexProgram;		}
		bool SupportFragmentProgram() 						{	return m_bSupportFragmentProgram;	}
		bool SupportOcculusion()								{	return m_bOcclusion;						}
		bool SupportGLSLProgram()								{	return m_bSupportGLSLProgram;			}		
		bool SupportFBO()											{	return m_bSupportFBO;					}
		bool SupportARBTC()										{	return m_bSupportARBTC;					}
		bool UseSpecMap() const									{	return m_kbSpecMap.GetBool();			}
		bool UseNormalMap() const								{	return m_kbNormalMap.GetBool();		}		
		bool UseHDR()												{	return m_kbUseHDR.GetBool() && m_bSupportHDR;	}
		
		//arrays
		void ResetPointers();
		void SetPointer(POINTER_TYPE eType,void* pkPointer);
		void SetNrOfVertexs(const int& iNr)					{	m_iNrOfVertexs = iNr;	};
		void SetNrOfIndexes(const int& iNr)					{	m_iNrOfIndexes = iNr;	};
		void SetDrawMode(DRAW_MODE eDrawMode);
		void DrawArray();
		void DrawArray(DRAW_MODE eDrawMode);
	
		ZVertexBuffer* CreateVertexBuffer(DRAW_MODE eDrawMode);
		ZVertexBuffer* CreateVertexBuffer();
		
		void DrawVertexBuffer(ZVertexBuffer* pkBuffer);
		
		//basic draw funktions
		void ClearGeometry();
		void DrawGeometry();
		void DrawGeometry(DRAW_MODE eDrawMode);
		
		//VERTEX 3 STUFF
		
		//point
		void AddPointV(const Vector3& kPos);
		void AddPointN(const Vector3& kNormal);
		void AddPointC(const Vector4& kColor);
		void AddPointUV(const Vector2& kPos,const int& iTU=0);
		//line 2V
		void AddLineV(const Vector3& kPos1,const Vector3& kPos2);
		void AddLineN(const Vector3& kNormal1,const Vector3& kNormal2);
		void AddLineC(const Vector4& kColor1,const Vector4& kColor2);
		void AddLineUV(const Vector2& kPos1,const Vector2& kPos2,const int& iTU=0);		
		//triangle 3V
		void AddTriangleV(const Vector3& kPos1,const Vector3& kPos2,const Vector3& kPos3);
		void AddTriangleN(const Vector3& kNormal1,const Vector3& kNormal2,const Vector3& kNormal3);
		void AddTriangleC(const Vector4& kColor1,const Vector4& kColor2,const Vector4& kColor3);
		void AddTriangleUV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const int& iTU=0);			
		void AddTriangleT(const Vector3& kTangent1,const Vector3& kTangent2,const Vector3& kTangent3);
		void AddTriangleBT(const Vector3& kTangent1,const Vector3& kTangent2,const Vector3& kTangent3);
		//quad 4V
		void AddQuadV(const Vector3& kPos1,const Vector3& kPos2,const Vector3& kPos3,const Vector3& kPos4);
		void AddQuadN(const Vector3& kNormal1,const Vector3& kNormal2,const Vector3& kNormal3,const Vector3& kNormal4);
		void AddQuadC(const Vector4& kColor1,const Vector4& kColor2,const Vector4& kColor3,const Vector4& kColor4);
		void AddQuadUV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const Vector2& kPos4,const int& iTU=0);	

		//VERTEX 2 STUFF
		void AddQuadV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3,const Vector2& kPos4);
		void AddTriangleV(const Vector2& kPos1,const Vector2& kPos2,const Vector2& kPos3);
		void AddLineV(const Vector2& kPos1,const Vector2& kPos2);

		
				
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
		
		void MatrixSave(Matrix4* pkMatrix);
		void MatrixLoad(const Matrix4* pkMatrix);
		
		void MatrixGenerateOrtho(	const float& fLeft,const float& fRight,const float& fBottom,
											const float& fTop,const float& fNear,const float& fFar)
																	{
																		glLoadIdentity();
																		glOrtho(fLeft,fRight,fBottom,fTop,fNear,fFar);
																	};
		void MatrixGeneratePerspective(	const float& fFov,const float& fAspect,const float& fNear,
													const float& fFar)
																	{
																		glLoadIdentity();
																		gluPerspective(fFov,fAspect,fNear,fFar);
																	};
		
		friend class ZFProgram;
		friend class ZVProgram;
};




#endif




