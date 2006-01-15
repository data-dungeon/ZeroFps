#ifndef _GLSLPROGRAM_H_
#define _GLSLPROGRAM_H_

#include "../basic/zfresource.h"
#include "../ogl/zfpsgl.h"
#include "render_x.h"
#include "zshadersystem.h"


using namespace std;


#define	NO_GLSLPROGRAM	0

/**	\brief	A OpenGL GLSL Program.
		\ingroup Render

This is a OpenGL Shading Language program that can be assigned to a material and used
in the rendering.  reqires opengl 1.4 
*/
class RENDER_API GLSLProgram : public ZFResource
{
	private:
		enum eSHADERTYPE
		{
			eVERTEX_SHADER,
			eFRAGMENT_SHADER,
		};
	
		ZShaderSystem*	m_pkShader;
	
		bool	SetupShaderPointer();		
		GLenum LoadAndCompile(const string& strFile,eSHADERTYPE iShaderType,int iLights);		
		bool LoadDataFromFile(string* pkString,const string& strFile,int iLights);
		string GetInfoLog(GLenum iProgram);

	public:
		GLenum	m_iProgramIDs[9];	
		

		GLSLProgram();
		~GLSLProgram();
		
		bool IsValid()	const								{	return m_iProgramIDs[0] != NO_GLSLPROGRAM;	};
		bool Create(const string&  strName);
		int CalculateSize();

		bool Load(string  strFile);
		bool UnLoad();
		
};


RENDER_API ZFResource* Create__GLSLProgram();

#endif


