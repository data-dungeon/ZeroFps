#include <cstdio>
#include <cstdarg>
#include "basicconsole.h"
#include "zfsystem.h"
#include "globals.h"
#include "zfassert.h"

using namespace std;

char acWordWrap[1024];
char acPrint[1024];

BasicConsole::BasicConsole(char* szName)
: ZFSubSystem(szName)
{
	m_iMaxWidth = 50;	//TEXT_MAX_LENGHT;
	m_bLog      = false;
}


BasicConsole::~BasicConsole()
{
	for(unsigned int i =0;i<m_kText.size();i++)
	{
		delete[] m_kText[i];	
	}
	
	m_kText.clear();
}


/**	\brief	Prints a single row to console.
		\ingroup Basic
*/
void BasicConsole::PrintOneRow(const char* aText)
{
	ZFAssert(aText, "NULL Pointer argument");

	delete[] m_kText[m_kText.size()-1];
	
	for(int i=m_kText.size()-1;i>0;i--)
	{
		if(m_kText[i-1]!=NULL)
		{
			m_kText[i]=m_kText[i-1];			
		}
	}
	
	m_kText[0]=new char[m_iMaxWidth+2];
	
	strcpy(m_kText[0],aText);
}


/**	\brief	Prints a row and word wraps so it don't go beoynd edge of console.
		\ingroup Basic
*/
void BasicConsole::PrintWordWrap(const char* aText)
{
	ZFAssert(aText, "NULL Pointer argument");

	string strEndLined = string(aText) + "\n";
	if(m_bLog)
		GetSystem().Log("console", strEndLined.c_str());

	int iNumOfChars = strlen( aText );

	while(iNumOfChars > m_iMaxWidth)
	{
		const char* szSpace = &aText[m_iMaxWidth];
		int	iWidht  = m_iMaxWidth;

		while(szSpace > aText && szSpace[0] != ' ')	szSpace--;
		if(szSpace == aText)
		{
			szSpace =  &aText[m_iMaxWidth];
		}
		else
		{
			iWidht  = szSpace - aText;
		}

		strncpy(acWordWrap, aText, iWidht);
		acWordWrap[iWidht] = 0;
		PrintOneRow(acWordWrap);
		aText += iWidht;
		iNumOfChars -= iWidht;
	}

	if(aText[0])
		PrintOneRow(aText);
}


/**	\brief	Prints one row and handles splitting it up at line breaks.
		\ingroup Basic
*/
void BasicConsole::Print(const char* aText) 
{
	ZFAssert(aText, "NULL Pointer argument");

	const char* pszText = aText;
	
	char* pszCr = strchr(pszText, 10);
	while(pszCr)
	{
		strncpy(acPrint,pszText, pszCr - pszText);
		acPrint[pszCr - pszText] = 0;
      PrintWordWrap(acPrint);
		//cout << " - '" << acPrint << "'"<< endl;
		pszText = pszCr + 1;
		pszCr = strchr(pszText, 10);
	}

	if(pszText[0])
	{
		//cout << "- '" << pszText << "'"<< endl;
      PrintWordWrap(pszText);
	}
}


/**	\brief	The function used to print to the console.
		\ingroup Basic
*/
void BasicConsole::Printf(const char *fmt, ...)
{
	ZFAssert(fmt, "NULL Pointer argument");

	// Make sure we got something to work with.	
	if (fmt == NULL)			
		return;							

	va_list		ap;							// Pointer To List Of Arguments

	va_start(ap, fmt);						// Parses The String For Variables
		vsprintf(g_szFormatText, fmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	Print(g_szFormatText);
}





