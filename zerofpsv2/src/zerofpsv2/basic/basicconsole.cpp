#include <cstdio>
#include <cstdarg>
#include "basicconsole.h"
#include "zfobjectmanger.h"
#include "globals.h"

extern ZFSystem g_ZFObjSys;

using namespace std;


//char format_text[4096];	


BasicConsole::BasicConsole(char* szName)
: ZFSubSystem(szName)
{
}

void BasicConsole::Print(const char* aText) 
{
	string strEndLined = string(aText) + "\n";
	g_ZFObjSys.Log("console", strEndLined.c_str());

	delete[] m_kText[m_kText.size()-1];
	
	for(int i=m_kText.size()-1;i>0;i--){
		if(m_kText[i-1]!=NULL){
			m_kText[i]=m_kText[i-1];			
		}
	}
	
	m_kText[0]=new char[TEXT_MAX_LENGHT];
	strcpy(m_kText[0],aText);
}

void BasicConsole::Printf(const char *fmt, ...)
{
	va_list		ap;							// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (fmt == NULL)	return;					

	va_start(ap, fmt);						// Parses The String For Variables
		vsprintf(g_szFormatText, fmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	Print(g_szFormatText);
}

void BasicConsole::RunCommand(int cmdid, const CmdArgument* kCommand)
{
/*	switch(cmdid) {
		case FID_HELP:
			Print("");
			Print("### help ###");
			Print(" quit        -exit program");
			Print(" varlist     -list variables");		
			Print(" set $n $v   -set variable");		
			Print(" music 1/0   -music on/off");
			Print(" togglegrab  -toggle input grab");
			Print(" setdisplay  -reinit display");
			break;
	}*/
}




