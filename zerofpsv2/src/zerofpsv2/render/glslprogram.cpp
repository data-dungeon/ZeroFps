#include "glslprogram.h"
 
GLSLProgram::GLSLProgram()
{
	for(int i= 0;i<9;i++)
		m_iProgramIDs[i] = NO_GLSLPROGRAM;
		
	m_pkShader = NULL;
}

GLSLProgram::~GLSLProgram()
{
	if(m_iProgramIDs[0] == NO_VPROGRAM)
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
	
	for(int i =0;i<9;i++)
	{
		//load and compile shaders
		GLenum iVSID = NO_GLSLPROGRAM;
		GLenum iFSID = NO_GLSLPROGRAM;
			
		if(!strVShader.empty())
			iVSID = LoadAndCompile(strVShader,eVERTEX_SHADER,i);
	
		if(!strFShader.empty())
			iFSID = LoadAndCompile(strFShader,eFRAGMENT_SHADER,i);
	
		//no shader was loaded
		if(iVSID == NO_GLSLPROGRAM && iFSID == NO_GLSLPROGRAM)
		{
			cout<<"WARNING: glslprogram not created, no shader loaded"<<endl;
			return false;	
		}
			
		//create program object
		m_iProgramIDs[i] = glCreateProgramObjectARB();
			
		//attach shaders to program
		if(iVSID != NO_GLSLPROGRAM)
		{
			glGetError();
			glAttachObjectARB(m_iProgramIDs[i], iVSID);
			if(glGetError() != GL_NO_ERROR)
			{
				cout<<"error while attaching vertex program: "<<strVShader<<endl;
			}
		}
		
		if(iFSID != NO_GLSLPROGRAM)
		{
			glGetError();
			glAttachObjectARB(m_iProgramIDs[i], iFSID);
			if(glGetError() != GL_NO_ERROR)
			{
				cout<<"error while attaching fragment program: "<<iFSID<<endl;
			}		
		}
	
		// Link The Program Object
		glLinkProgramARB(m_iProgramIDs[i]);
		
		GLint iRet;
		glGetObjectParameterivARB(m_iProgramIDs[i],GL_OBJECT_LINK_STATUS_ARB,&iRet);
		if(iRet == GL_FALSE)
		{	
			cout<<"ERROR: While linking GLSL program: "<<strFile<<endl;		
				
			//get log
			static char log[1024];
			int iLogSize = 0;
			glGetInfoLogARB(m_iProgramIDs[i],1024,&iLogSize,log);
			
			if(iLogSize != 0)
				cout<<log<<endl;			
		
			glDeleteObjectARB(m_iProgramIDs[i]);
			m_iProgramIDs[i] = NO_GLSLPROGRAM;	
		}
		
		
		//remove shader objects
		if(iVSID != NO_GLSLPROGRAM)
			glDeleteObjectARB(iVSID);
		if(iFSID != NO_GLSLPROGRAM)
			glDeleteObjectARB(iFSID);
		
		
		if(m_iProgramIDs[i] == NO_GLSLPROGRAM)
		{
			return false;
		}
	}
//  	cout<<"GLSL program created:"<<strFile<<" id "<<m_iProgramID<<endl;
	return true;
}

GLenum GLSLProgram::LoadAndCompile(const string& strFile,eSHADERTYPE iShaderType,int iLights)
{
	GLenum iShaderID;

	//create shader object
	if(iShaderType == eVERTEX_SHADER)
		iShaderID = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	else
		iShaderID = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);		
			
		
	string strData;
	
	//load shader
	if(!LoadDataFromFile(&strData,strFile,iLights))
	{
		glDeleteObjectARB(iShaderID);
		return NO_GLSLPROGRAM;
	}

	//load source to shader object	
	const char* pkData = strData.c_str();
 	glShaderSourceARB(iShaderID, 1, (const char**)&pkData, NULL);
	
	//compile shader
	glCompileShaderARB(iShaderID);	
	
	GLint iRet;
	glGetObjectParameterivARB(iShaderID,GL_OBJECT_COMPILE_STATUS_ARB,&iRet);
	if(iRet == GL_FALSE)
	{	
		cout<<"ERROR: While compiling shader "<<strFile<<endl;			
		
		//get log
		static char log[1024];
		int iLogSize = 0;
		glGetInfoLogARB(iShaderID,1024,&iLogSize,log);
		
		if(iLogSize != 0)
			cout<<log<<endl;
 	
		glDeleteObjectARB(iShaderID);
		return NO_GLSLPROGRAM; 	
 	}
			
	return iShaderID;
}

bool GLSLProgram::LoadDataFromFile(string* pkString,const string& strFile,int iLights)
{
	static string strGLSLdir = "/data/glsl/";

	ZFVFile kFile;
	
	if(!kFile.Open(strGLSLdir+strFile,0,false))
	{	
		cout<<"ERROR: could not open shader "<<strFile<<endl;
		return false;
	}
		
	//load to buffert	
	int iSize = kFile.GetSize();
	char* pkData = new char[iSize+1];		
	kFile.Read(pkData,iSize,1);
	
	kFile.Close();
	
	//insert to total buffert
	pkString->insert(0,pkData,iSize);
	
	//deallocate temporary buffert
	delete pkData;
	

	//fix light stuff	
	int iStartPos = pkString->find("#FOR_EACH_LIGHT_START",0) ;		//21
	int iStopPos = pkString->find("#FOR_EACH_LIGHT_STOP",0);			//20

	if(iStartPos != -1 && iStopPos != -1)
	{
		//copy to seperat buffer
 		string strLbuffert = pkString->substr(iStartPos+21,iStopPos - iStartPos-21);
		
		//erase from shader buffer
		pkString->erase(iStartPos,(iStopPos+20) - (iStartPos));
		
		for(int i = iLights-1;i>0;i--)
		{
			string strLights = IntToString(i);			
			string strTemp = strLbuffert;
		
			int iLight = 0;		
			while(iLight != -1)
			{
				iLight = strTemp.find("#LIGHT",0);							
				if(iLight != -1)
					strTemp.replace(iLight,6,strLights);
			}
			
			pkString->insert(iStartPos,strTemp);
			
		}			
		
	// 		cout<<"FIXED:"<<*pkString<<":END"<<endl;		
	// 		exit(1);
	}
	

	//find include files
	vector<string>	kIncludes;
			
	int iPos = 0;
	int iEnd;
	while(1)
	{
		//find includes
		iPos = pkString->find("#include",iPos);
		
		//no more includes found
		if(iPos == -1)
			break;
		
		//check for comment
		if(iPos != 0)
			if(pkString->rfind('\n',iPos) != iPos -1)
			{
				iPos++;
				continue;
			}
		
		//find line size
		iEnd = pkString->find('\n',iPos); 		

		//add include file to include list
 		kIncludes.push_back(pkString->substr(iPos + 9,iEnd - iPos - 10 ));//) iEnd-11));
 		
 		//erase include statement from buffrt
 		pkString->erase(iPos,iEnd - iPos);
	}
	
	//append includes on the top
	for(int i =0;i<kIncludes.size();i++)
	{		
 		if(!LoadDataFromFile(pkString,kIncludes[i],iLights))
 		{
 			cout<<"ERROR while including: "<<kIncludes[i]<<"|"<<endl;
 		}
	}
	
	return true;
}


bool GLSLProgram::UnLoad()
{
	if(!SetupShaderPointer())
		return false;

			
	for(int i =0;i<9;i++)
	{
		if(m_iProgramIDs[i] != NO_GLSLPROGRAM)
		{
			glDeleteObjectARB(m_iProgramIDs[i]);		
			m_iProgramIDs[i] = NO_GLSLPROGRAM;
		}
	}
	
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











