#include <iostream>
#include <string>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdarg>

#include "basic_x.h"
//#include "os.h"
#include "zfsubsystem.h"
#include "zfsystem.h"
#include "globals.h"

using namespace std;
//char g_LogFormatTxt3[4096];	


BASIC_API ZFSystem g_ZFObjSys;
 
ZFSubSystem::ZFSubSystem(char *szName)
{
	m_strZFpsName = string("");
	m_pkSystem = NULL;
	g_ZFObjSys.Register(this, szName);
}

ZFSubSystem::~ZFSubSystem()
{
	GetSystem().UnRegister(this);
}

/// Get Ptr to ZFSystem
ZFSystem& ZFSubSystem::GetSystem()
{
	return *m_pkSystem;
}

/// Register a Cmd, See ZFSystem::Register_Cmd() for paramters.
bool ZFSubSystem::Register_Cmd(char* szName, int iCmdID, int iFlags, char* szHelp, int iNumOfArg)
{
	return GetSystem().Register_Cmd(szName,iCmdID,this, iFlags, szHelp, iNumOfArg);	
}

/// Register a Variable, See ZFSystem::RegisterVariable() for paramters.
bool ZFSubSystem::RegisterVariable(const char* szName, void* pvAddress, ZFCmdDataType eType, int iFlags)
{
	return GetSystem().RegisterVariable(szName,pvAddress,eType, this, iFlags);	
}

/// Log text to a Log file.
void ZFSubSystem::Logf(const char* szName, const char* szMessageFmt,...)
{
	va_list		ap;							// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (szMessageFmt == NULL)	return;					

	va_start(ap, szMessageFmt);						// Parses The String For Variables
		vsprintf(g_szFormatText, szMessageFmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	GetSystem().Log(szName, g_szFormatText);
}
