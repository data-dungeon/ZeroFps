#include "glslprogram.h"
 
GLSLProgram::GLSLProgram()
{
	m_iProgramID = NO_GLSLPROGRAM;
	m_pkShader = NULL;
}

GLSLProgram::~GLSLProgram()
{
	if(m_iProgramID == NO_VPROGRAM)
		return;

	UnLoad();
}

bool GLSLProgram::Create(string  strName)
{
	return Load(strName);
}

int GLSLProgram::CalculateSize()
{
	return 2;
}

bool GLSLProgram::Load(string  strFile)
{
	if(!SetupShaderPointer())
		return false;
	
	if(!m_pkShader->SupportGLSLProgram())
		return false;
		
	//remove .glsl sufix
	strFile.erase(strFile.length() - 5,5);
			
// 	cout<<"setting up glsl program: "<<strFile<<endl;
	
 	int iSplit = strFile.find('#');
	string strVShader;
	string strFShader;
	
	strVShader = strFile.substr(0,iSplit);
	strFShader = strFile.substr(iSplit+1,strFile.length());
	

	//load and compile shaders
	GLenum iVSID = NO_GLSLPROGRAM;
	GLenum iFSID = NO_GLSLPROGRAM;
		
	if(!strVShader.empty())
		iVSID = LoadAndCompile(strVShader,eVERTEX_SHADER);

	if(!strFShader.empty())
		iFSID = LoadAndCompile(strFShader,eFRAGMENT_SHADER);

	//no shader was loaded
	if(iVSID == NO_GLSLPROGRAM && iFSID == NO_GLSLPROGRAM)
	{
		cout<<"WARNING: glslprogram not created, no shader loaded"<<endl;
		return false;	
	}
		
	//create program object
	m_iProgramID = glCreateProgramObjectARB();
		
	//attach shaders to program
	if(iVSID != NO_GLSLPROGRAM)
	{
		glAttachObjectARB(m_iProgramID, iVSID);
	}
	
	if(iFSID != NO_GLSLPROGRAM)
	{
		glAttachObjectARB(m_iProgramID, iFSID);
	}

	// Link The Program Object
	glGetError();
	glLinkProgramARB(m_iProgramID);
	
	
	//get log
	static char log[1024];
	int iLogSize = 0;
	glGetInfoLogARB(m_iProgramID,1024,&iLogSize,log);
	
	if(iLogSize != 0)
	{
		cout<<"ERROR: While linking program "<<strFile<<endl;
		cout<<log<<endl;
		
		glDeleteObjectARB(m_iProgramID);
		m_iProgramID = NO_GLSLPROGRAM;
	}	

	//remove shader objects
	if(iVSID != NO_GLSLPROGRAM)
		glDeleteObjectARB(iVSID);
	if(iFSID != NO_GLSLPROGRAM)
		glDeleteObjectARB(iFSID);
	
	
	if(m_iProgramID == NO_GLSLPROGRAM)
		return false;
		
// 	cout<<"GLSL program created:"<<strFile<<" id "<<m_iProgramID<<endl;
	return true;
}

GLenum GLSLProgram::LoadAndCompile(const string& strFile,eSHADERTYPE iShaderType)
{
	ZFVFile kFile;
	
	if(!kFile.Open(strFile,0,false))
	{
		cout<<"ERROR: could not open shader "<<strFile<<endl;
		return NO_GLSLPROGRAM;
	}
		
	GLenum iShaderID;

	//create shader object
	if(iShaderType == eVERTEX_SHADER)
		iShaderID = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	else
		iShaderID = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);		
			
	//load source from file
	int iSize = kFile.GetSize();
	char* pkData = new char[iSize+1];		
	kFile.Read(pkData,iSize,1);
	pkData[iSize] = 0;	
	
	//load source to shader object
	glShaderSourceARB(iShaderID, 1, (const char**)&pkData, NULL);

	//compile shader
	glCompileShaderARB(iShaderID);	
	
	//get log
	static char log[1024];
	int iLogSize = 0;
	glGetInfoLogARB(iShaderID,1024,&iLogSize,log);
	
	if(iLogSize != 0)
	{
		cout<<"ERROR: While compiling shader "<<strFile<<endl;
		cout<<log<<endl;
		
		delete pkData;
		glDeleteObjectARB(iShaderID);
		return NO_GLSLPROGRAM;
	}
	
	
	//delete data buffert
	delete pkData;
	
	return iShaderID;
}

bool GLSLProgram::UnLoad()
{
	if(m_iProgramID == NO_GLSLPROGRAM)
		return false;

	if(!SetupShaderPointer())
		return false;

			
	glDeleteObjectARB(m_iProgramID);	
	
	m_iProgramID = NO_GLSLPROGRAM;
	
	return true;
}

bool GLSLProgram::SetupShaderPointer()
{
	if(m_pkShader)
		return true;
		
 	m_pkShader	= static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem")); 		

	if(m_pkShader)
		return true;
	else
		return false;
}

ZFResource* Create__GLSLProgram()
{
	return new GLSLProgram;
}











