#include <cstdio>
#include <cstdarg>
#include "basicconsole.h"
#include "zfsystem.h"
#include "globals.h"

//extern ZFSystem g_ZFObjSys;
  
using namespace std;

BasicConsole::BasicConsole(char* szName)
: ZFSubSystem(szName)
{
}

BasicConsole::~BasicConsole()
{
	for(unsigned int i =0;i<m_kText.size();i++)
	{
		delete[] m_kText[i];	
	}
	
	m_kText.clear();
}

void BasicConsole::Print(const char* aText) 
{
	string strEndLined = string(aText) + "\n";
	//g_ZFObjSys.Log("console", strEndLined.c_str());
	GetSystem().Log("console", strEndLined.c_str());

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





