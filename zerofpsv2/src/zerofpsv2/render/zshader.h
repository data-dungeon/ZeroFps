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
#include "zfprogram.h"

#include <stack>

using namespace std;
class ZMaterial;
class ZMaterialSettings;



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

		//vertexdata offset (used for shadow calculations etc)
		stack<Matrix4>	m_kModelMatrixStack;
		Matrix4			m_kModelMatrix;

		//force lighting to this
		int			m_iForceLighting;		
		
		//force blending
		int			m_iForceBledning;
		
		//did we copy the data?
		bool			m_bCopyedData;
	
		//do we have vertex program support
		bool			m_bVertexProgram;
		bool			m_bFragmentProgram;
	
		//vertex program is NO_VPROGRAM if no vertex program is active
		int			m_iCurrentVertexProgram;
		
		//fragment program is NO_FPROGRAM if no fragment  program is active
		int			m_iCurrentFragmentProgram; 
		
	
		//this enable/disable client states if pointers have been set
		void SetupClientStates();			
		
		//configure setup, stuff before setting up a pass, acording to current material
		void SetupPrerenderStates();
		
		//configure render states/pass
		void SetupRenderStates(ZMaterialSettings* pkSettings);
		
		//setup texture unit
		void SetupTU(ZMaterialSettings* pkSettings,int iTU);
		
		//setup and update vertex/fragment  programs
		void SetupVertexProgram(ZMaterialSettings* pkSettings);
		void SetupFragmentProgram(ZMaterialSettings* pkSettings);
		
		void UpdateFragmentProgramParameters();
		
		//makes a memcopy of all none null pointers data, and sets the pointers to the new location
		void CopyVertexData();
		void CopyData(void** pkData,int iSize);
		void CleanCopyedData();
		
		//effects
		void RandomVertexMovements();
		void Waves();	
		void TextureOffset();
		void ColorEffect(ZMaterialSettings* pkSettings);
	
		bool HaveVertexProgramExt();
		bool HaveFragmentProgramExt();

		void SetVertexProgram(int iVPID);
		void SetFragmentProgram(int iVPID);
		int  GetCurrentVertexProgram() 	{ return m_iCurrentVertexProgram;};
		int  GetCurrentFragmentProgram() { return m_iCurrentFragmentProgram;};
		bool SupportVertexProgram() 		{ return m_bVertexProgram;};
		bool SupportFragmentProgram() 	{ return m_bFragmentProgram;};
		
	public:
		ZShader();
		bool StartUp();

		void SetForceLighting(int iMode) { m_iForceLighting = iMode;};
		int  GetForceLighting() { return m_iForceLighting; }
		void SetForceBlending(int iMode) { m_iForceBledning = iMode;};
		void SetMaterial(ZMaterial* pkMaterial);
		void SetPointer(int iType,void* pkPointer);
		void SetNrOfVertexs(int iNr);
		void SetNrOfIndexes(int iNr);
		void SetDrawMode(int iDrawMode);


		
		//matrix manipulation
		void MatrixIdentity()						{m_kModelMatrix.Identity();};
		void MatrixTranslate(Vector3 kPos)		{m_kModelMatrix.Translate(kPos.x,kPos.y,kPos.z);};
		void MatrixRotate(Vector3 kRot)			{m_kModelMatrix.Rotate(kRot.x,kRot.y,kRot.z);};
		void MatrixScale(Vector3 kScale)			{m_kModelMatrix.Scale(kScale.x,kScale.y,kScale.z);};
		void MatrixScale(float fScale)			{m_kModelMatrix.Scale(fScale,fScale,fScale);};
		void MatrixMult(Matrix4 kMatrix)			{m_kModelMatrix *= kMatrix;};
		void MatrixPop();
		void MatrixPush();

		void Reset();
		void Draw();


		bool ShutDown();
		bool IsValid();

		
		friend class ZVProgram;
		friend class ZFProgram;
};



#endif


