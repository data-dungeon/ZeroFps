#include "basicconsole.h"
#include <cstdio>
#include <cstdarg>

using namespace std;

char format_text[4096];	


BasicConsole::BasicConsole(char* szName)
: ZFObject(szName)
{

}

void BasicConsole::Print(const char* aText) {
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
		vsprintf(format_text, fmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	Print(format_text);
}


 


void BasicConsole::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) {
		case FID_VERSION:
			Print("ZeroFps Beta 1.0");
			break;

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
	}
}