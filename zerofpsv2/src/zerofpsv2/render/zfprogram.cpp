#include "zfprogram.h"
#include "render.h"
 
ZFProgram::ZFProgram()
{
	m_iId = NO_FPROGRAM;
	m_sFileName = "";
	m_pkShader = NULL;
}

ZFProgram::~ZFProgram()
{
	if(m_iId == NO_FPROGRAM)
		return;

	UnLoad();
}


bool ZFProgram::Create(const string&  strFileName)
{
	string strName = strFileName;

	//setup data path
	if(strName.find("data/vertexprograms/") == -1)
		strName = "data/vertexprograms/"+strName;
// 	else
// 		cerr<<"Warning: old resource path:"<<strName<<endl;

	return Load(strName);
}

int ZFProgram::CalculateSize()
{
	return 2;
}

bool ZFProgram::Load(string  strFile)
{
	if(!SetupShaderPointer())
		return false;

	//check if we have vertex program support
	if(!m_pkShader->SupportFragmentProgram())
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
	int iOldID = m_pkShader->GetCurrentFragmentProgram();	
	
	//set current active program to the new created
	m_pkShader->SetFragmentProgram(iID);

	//upLoad the program.
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, iSize, cpProgram);		
	
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
	m_pkShader->SetFragmentProgram(iOldID);
	

	cout<<"Loaded fragment program: "<<strFile<<endl;
	return true;
}

bool ZFProgram::UnLoad()
{
	if(m_iId == NO_FPROGRAM)
		return false;

	if(!SetupShaderPointer())
		return false;

	GLuint iID = m_iId;

	//delete program
	glDeleteProgramsARB(1,&iID);
	
	m_iId = NO_FPROGRAM;
	
	return true;
}

bool ZFProgram::SetupShaderPointer()
{
	if(m_pkShader)
		return true;
		
 	m_pkShader	= static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem")); 		

	if(m_pkShader)
		return true;
	else
		return false;
}

ZFResource* Create__ZFProgram()
{
	return new ZFProgram;
}


