#include "basic_x.h"
#include "os.h"
#include "cmdsystem.h"
#include "zfsystem.h"
#include "globals.h"

CmdSystem::CmdSystem()
: ZFSubSystem("CmdSystem") 
{
	m_pkCon = NULL;

	Register_Cmd("set",			FID_SET,			CSYS_FLAG_SRC_ALL,	"set name value", 2);
	Register_Cmd("varlist",		FID_VARLIST,	CSYS_FLAG_SRC_ALL);
	Register_Cmd("commands",	FID_COMMANDS,	CSYS_FLAG_SRC_ALL);
	Register_Cmd("sys",			FID_SYS,			CSYS_FLAG_SRC_ALL);
}

bool CmdSystem::StartUp()	
{ 
	m_pkCon = dynamic_cast<BasicConsole*>((GetSystem().GetObjectPtr("Console")));
	return true; 
}

bool CmdSystem::ShutDown() 
{ 
	return true; 
}

bool CmdSystem::IsValid()	
{ 
	return true; 
}

bool CmdSystem::Set(const char* acName,const char* acData)
{
	return GetSystem().SetVariable(const_cast<char*>(acName),const_cast<char*>(acData));
}

void CmdSystem::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_SET:
			if(!Set(kCommand->m_kSplitCommand[1].c_str(),&kCommand->m_strFullCommand.c_str()[kCommand->m_kSplitCommand[0].length() + kCommand->m_kSplitCommand[1].length() + 2])){
				if(m_pkCon)
					m_pkCon->Printf("Variable not found");
				return;
			} else {
				if(m_pkCon)
					m_pkCon->Printf("Setting %s = %s",kCommand->m_kSplitCommand[1].c_str(),kCommand->m_kSplitCommand[2].c_str());
			}
			
			
			break;

		case FID_VARLIST:		GetSystem().PrintVariables();	break;
		case FID_COMMANDS:	GetSystem().PrintCommands();	break;

		case FID_SYS:
			// Draw All Systems to console.
			for(unsigned int i=0; i < m_pkSystem->kObjectNames.size();i++) {
				m_pkCon->Printf(" %s, %d\n", m_pkSystem->kObjectNames[i].m_strName.c_str(), m_pkSystem->kObjectNames[i].m_iNumOfRequests );
			}

			break;
	}
}


















