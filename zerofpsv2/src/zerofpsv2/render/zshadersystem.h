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

enum RENDER_API MATRIX_MODE
{
	MATRIX_MODE_MODEL			= 0,
	MATRIX_MODE_PROJECTION	= 1,
	MATRIX_MODE_TEXTURE		= 2,
	MATRIX_MODE_COLOR			= 3,	
};

class RENDER_API ZShaderSystem : public ZFSubSystem
{
	private:
		TextureManager*	m_pkTexMan;
		Light*				m_pkLight;	
		
		//current selected material
		ZMaterial*			m_pkCurrentMaterial;


		
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
		
		//basic
		void SetupGlobalSettings();
		void SetupArrayClientStates();
		void SetupTUClientStates(const int& iPass);
		void SetupPass(int iPass);
		void SetupTU(ZMaterialSettings* pkSettings,int iTU);
		
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
		void ResetStatistics() 		{m_iMaterialReloads = 0;
											m_iMaterialBinds =	0;
											m_iSavedReloads = 	0;
											m_iGLupdates = 		0;};
		
		//gl states that is not included in the material
		void SetColorMask(const bool& bColorMask);
		void SetDepthMask(const bool& bDepthMask);
													
		//basic
		void Push(const char* czNote);
		void Pop();		
		void BindMaterial(ZMaterial* pkMaterial);
		void ReloadMaterial();
		
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
};




#endif



