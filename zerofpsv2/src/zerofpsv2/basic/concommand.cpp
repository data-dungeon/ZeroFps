#include <cstdio>
#include <cstdarg>
#include "basicconsole.h"
#include "zfsystem.h"
#include "globals.h"
#include "zfassert.h"
#include "concommand.h"

using namespace std;


// CON COMMAND LINE //////////////////////////////////////////////////////////////////////////////////////
void ConCommandLine::Set(const char* szCmdArgs)
{
	m_strFullCommand = string(szCmdArgs);
//	m_strFullCommand.replace(m_strFullCommand.begin(), m_strFullCommand.end(),9,32);	// Replace tab with space.
	m_kSplitCommand.clear();
	string	kNewArg;

	if(strlen(szCmdArgs) == 0) {
		m_kSplitCommand.push_back("");
		return;
	}

	int args = 0;	// Number of arguments
	kNewArg = "";

	for(unsigned int i=0; i<strlen(szCmdArgs); i++) {

		while((unsigned int)(szCmdArgs[i]) != 32 && i < strlen(szCmdArgs) ) {	//loop until space
			kNewArg.append(1,szCmdArgs[i]);						//add to argument nr args
			i++;
		}

		if(kNewArg.size() !=0 ) {
		//if nothing was added to the argument use it in the next loop
			m_kSplitCommand.push_back(kNewArg);		
			kNewArg = "";
		}
			args++;
	}
}

int		ConCommandLine::argc() const
{
	return m_kSplitCommand.size();
}

string	ConCommandLine::argv(int iIndex) const
{
	return m_kSplitCommand[iIndex];
}

string	ConCommandLine::GetLine() const	
{	
	return m_strFullCommand;	
}


// CON COMMAND //////////////////////////////////////////////////////////////////////////////////////
ConCommand::ConCommand() 
{
	m_pkSubSystem		= NULL;
	m_bOwnedBySystem	= false;
}

ConCommand::~ConCommand()
{
	// Unregister.
}

void ConCommand::Create(string strName)
{
	m_strName = strName;

	// Register
	g_ZFObjSys.RegisterConCommand( this );
}

string ConCommand::GetSubSystemName()
{
	if(!m_pkSubSystem)
		return string("");

	return m_pkSubSystem->GetName();
}

// CONFUNCTION //////////////////////////////////////////////////////////////////////////////////////
ConFunction::ConFunction()
{
	m_pCallBackFunction = NULL;
	m_iCmdID = 0;
}

ConFunction::ConFunction(char* szName, ConFunctionCallback pCallBack)
{
	Register(NULL, szName, pCallBack);
	m_iCmdID = 0;
}

ConFunction::ConFunction(char* szName, ConFunctionCallback pCallBack, char* szHelp)
{
	Register(NULL, szName, pCallBack, szHelp);
	m_iCmdID = 0;
}

ConFunction::~ConFunction()
{
}

void ConFunction::Register(ZFSubSystem* pkSubSys, char* szName, ConFunctionCallback pCallBack)
{
	m_strName = szName;
	m_pkSubSystem = pkSubSys;

	m_pCallBackFunction = pCallBack;
	g_ZFObjSys.RegisterConCommand( this );
}

void ConFunction::Register(ZFSubSystem* pkSubSys, char* szName, ConFunctionCallback pCallBack, char* szHelp)
{
	m_strName = szName;
	m_strHelp = szHelp;
	m_pkSubSystem = pkSubSys;

	m_pCallBackFunction = pCallBack;
	g_ZFObjSys.RegisterConCommand( this );
}

void ConFunction::Register(ZFSubSystem* pkSubSys, char* szName, int iCmdID)
{
	m_strName = szName;
	m_pkSubSystem = pkSubSys;
	m_iCmdID = iCmdID;
	g_ZFObjSys.RegisterConCommand( this );
}


void ConFunction::Run( const ConCommandLine* pkCmd )
{
	/*
	CmdArgument kOst;
	string strHora;
	strHora = pkCmd->GetLine();
	kOst.Set(strHora.c_str());
	*/

	if(m_pCallBackFunction)
		m_pCallBackFunction( pkCmd );
	else
	{
		if(m_pkSubSystem)
			m_pkSubSystem->RunCommand(m_iCmdID, pkCmd );
	}
}


// CONVAR//////////////////////////////////////////////////////////////////////////////////////

ConVar::ConVar() : ConCommand()
{

}

ConVar::ConVar(char* szName, char* szDefaultValue) : ConCommand()
{
	Register(NULL, szName, szDefaultValue);
}

ConVar::ConVar(char* szName, char* szDefaultValue,char* szHelp) : ConCommand()
{
	Register(NULL, szName, szDefaultValue,szHelp);
}

ConVar::~ConVar()
{

}

void  ConVar::Register(ZFSubSystem* pkSubSys, char* szName, char* szDefaultValue)
{
	m_strName = szName;
	m_pkSubSystem = pkSubSys;

	m_strDefaultValue = szDefaultValue;
	SetString( m_strDefaultValue );
	g_ZFObjSys.RegisterConCommand( this );
}

void  ConVar::Register(ZFSubSystem* pkSubSys, char* szName, char* szDefaultValue, char* szHelp)
{
	m_strName = szName;
	m_strHelp = szHelp;
	m_pkSubSystem = pkSubSys;

	m_strDefaultValue = szDefaultValue;
	SetString( m_strDefaultValue );
	g_ZFObjSys.RegisterConCommand( this );
}

bool	ConVar::GetBool() const					{		return m_iValue;		}
int	ConVar::GetInt() const					{		return m_iValue;		}
float ConVar::GetFloat() const				{		return m_fValue;		}

string ConVar::GetString() const
{
	return m_strValue;
}

void ConVar::SetBool(bool bValue)	{		m_iValue = bValue;	}
void ConVar::SetInt(int iValue)		{		m_iValue = iValue;	}
void ConVar::SetFloat(float fValue)	{		m_fValue = fValue;	}

void ConVar::SetString(string strValue)
{
	m_strValue = strValue;

	if(IsSameIgnoreCase(m_strValue.c_str(), "false"))
	{
		m_fValue = 0;
		m_iValue = 0;
	}
	else if(IsSameIgnoreCase(m_strValue.c_str(), "true"))
	{
		m_fValue = 1;
		m_iValue = 1;
	}
	else if(IsSameIgnoreCase(m_strValue.c_str(), "toggle"))
	{
		if(m_iValue == 0)
		{
			m_fValue = 1;
			m_iValue = 1;
			m_strValue = "true";
		}
		else
		{
			m_fValue = 0;
			m_iValue = 0;
			m_strValue = "false";
		}
	}
	else
	{
		m_fValue = atof( m_strValue.c_str() );
		m_iValue = m_fValue;
	}
}

vector<bool> ConVar::GetBoolVector() const
{
	vector<bool> kbBoolVec;
	int iBVL = kbBoolVec.size();
	int iStrLen = strlen(m_strValue.c_str());
				
	/*if(iStrLen != iBVL)
	{
		printf("Wrong size of input bool vector");
		return kbBoolVec;
	}*/					
	
	for(int i = 0;i<iStrLen;i++)
	{
		if(m_strValue.c_str()[i] == '1')
			kbBoolVec.push_back(true);
		else if(m_strValue.c_str()[i] == '0')
			kbBoolVec.push_back(false);
		else
		{
			printf("Bad value, shuld be 1/0");
		}									
	}				
		
	return kbBoolVec;
}



void ConVar::SetBoolVector(vector<bool> kValue)
{
	string strValue;

	for(unsigned int i = 0;i< kValue.size();i++)
	{
		if(kValue[i])
			strValue += "1";
		else
			strValue += "0";
	}		

	m_strValue = strValue;
}


