// serialization.cpp: implementation of the Serialization class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>
#include "serialization.h"
#include "../zerofps/basic/zfini.h"
#include "../zerofps/basic/zfassert.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Serialization::Serialization(const char* szFileName, ZFIni* pkINI, bool bWrite)
{
	m_pkINI = pkINI;
	m_pkFile = NULL;

	if(bWrite)
	{
		m_pkFile = fopen(szFileName, "wt");
		ZFAssert(m_pkFile, "Serialization::Serialization(): Failed to create file.");
	}
	else
	{
		Open(szFileName);
	}
}

Serialization::~Serialization()
{
	if(m_pkFile)
		fclose(m_pkFile);
}

int Serialization::Output(const char *szFormat, ...)
{   
	va_list arg_ptr;
	va_start(arg_ptr, szFormat);
	int num_chars = vfprintf(m_pkFile,szFormat,arg_ptr);
	va_end(arg_ptr);
	return num_chars;
}

int Serialization::Outputa(const char *szFormat, ...)
{   
	int offset = ftell(m_pkFile);
	fseek(m_pkFile,offset,SEEK_SET);
	va_list arg_ptr;
	va_start(arg_ptr, szFormat);
	int num_chars = vfprintf(m_pkFile,szFormat,arg_ptr);
	va_end(arg_ptr);
	return num_chars;
}

void Serialization::Open(const char* szFileName)
{
	m_pkINI->Open(szFileName, false);
}
