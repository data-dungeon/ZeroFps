#include "zvprogram.h"
#include "render.h"
 
ZVProgram::ZVProgram()
{
	m_iId = NO_VPROGRAM;
	m_pkShader = NULL;
}

ZVProgram::~ZVProgram()
{
	if(m_iId == NO_VPROGRAM)
		return;

	UnLoad();
}


bool ZVProgram::Create(const string&  strFile)
{
	string strName = strFile;

	//setup data path
	if(strName.find("data/vertexprograms/") == -1)
		strName = "data/vertexprograms/"+strName;
	else
		cerr<<"Warning: old resource path:"<<strName<<endl;
		
		
	return Load(strName);
}

int ZVProgram::CalculateSize()
{
	return 2;
}

bool ZVProgram::Load(string  strFile)
{
	if(!SetupShaderPointer())
		return false;

	//check if we have vertex program support
	if(!m_pkShader->SupportVertexProgram())
		return false;

	ZFVFile		m_kFile;
	char*			cpProgram;
	int 			iSize;
	GLuint		iID;
	
	//open file
	if(!m_kFile.Open(strFile,0,false))
	{
		cout<<"error opening file:"<<strFile<<endl;
		return false;
	}
		
	//get program size
	iSize = m_kFile.GetSize();
	
	//allocated data buffer
	cpProgram = new char[iSize];
	
	//load program into buffer
	m_kFile.Read(cpProgram,iSize,1);
	
	//close file
	m_kFile.Close();
	
	
	//reset glERROR
	glGetError();
	
	//Generate a program.
	glGenProgramsARB(1, &iID);

	//save the old active vertex program
	int iOldID = m_pkShader->GetCurrentVertexProgram();	
	
	//set current active program to the new created
	m_pkShader->SetVertexProgram(iID);

	//upLoad the program.
	glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, iSize, cpProgram);		
	
	//error checking
	int error = glGetError();
	if(error != GL_NO_ERROR)
	{
		cout<<"Error occured while loading vertex program: "<<GetOpenGLErrorName(error)<<endl;
		
		int	pos;
		char*	errormsg;
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB,&pos);		
		errormsg = (char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		
		cout<<"Error: "<<errormsg<<" On offset "<<pos<<endl;
		
		return false;
	}
		
	//set id
	m_iId = iID;
	
	//reactivate the old program
	m_pkShader->SetVertexProgram(iOldID);
	

	cout<<"Loaded vertex program: "<<strFile<<endl;
	return true;
}

bool ZVProgram::UnLoad()
{
	if(m_iId == NO_VPROGRAM)
		return false;

	if(!SetupShaderPointer())
		return false;

	GLuint iID = m_iId;

	//delete program
	glDeleteProgramsARB(1,&iID);
	
	m_iId = NO_VPROGRAM;
	
	return true;
}

bool ZVProgram::SetupShaderPointer()
{
	if(m_pkShader)
		return true;
		
 	m_pkShader	= static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem")); 		

	if(m_pkShader)
		return true;
	else
		return false;
}

ZFResource* Create__ZVProgram()
{
	return new ZVProgram;
}


