// serialization.h: interface for the Serialization class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALIZATION_H__BE50BCC4_E009_49C5_A0FB_6549A8E9AADB__INCLUDED_)
#define AFX_SERIALIZATION_H__BE50BCC4_E009_49C5_A0FB_6549A8E9AADB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ZFIni;

class Serialization  
{
public:
	int Output(const char *szFormat, ...);
	int Outputa(const char *szFormat, ...);
	Serialization(const char* szFileName, ZFIni* pkINI, bool bWrite);
	virtual ~Serialization();

private:
	void Open(const char* szFileName);
	FILE* m_pkFile;
	ZFIni* m_pkINI;
};

#endif // !defined(AFX_SERIALIZATION_H__BE50BCC4_E009_49C5_A0FB_6549A8E9AADB__INCLUDED_)
