#include <stdio.h>
#include "zfobject.h"
#include "zfobjectmanger.h"

#include "zfvfs.h"

extern ZFObjectManger g_ZFObjSys;

ZFVFile::ZFVFile()
{
	m_pkFileSystem = static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		
	m_pkFilePointer = NULL;
	
}

ZFVFile::~ZFVFile()
{
	Close();
}

bool ZFVFile::Open(string strFileName, int iOptions)
{
	m_pkFilePointer = m_pkFileSystem->Open(strFileName, iOptions);
	if(m_pkFilePointer)
		return true;

	return false;
}

bool ZFVFile::Close()
{
	if(m_pkFilePointer)
		fclose(m_pkFilePointer);

	m_pkFilePointer = NULL;
	return true;
}

bool ZFVFile::Read  (void* pkData, int iSize, int iCount)
{
	return fread(pkData, iSize, iCount, m_pkFilePointer);
}

bool ZFVFile::Write (void* pkData, int iSize, int iCount)
{
	return fwrite(pkData, iSize, iCount, m_pkFilePointer);
}

void ZFVFile::Seek(int iPos, int iOrigin)
{
	fseek(m_pkFilePointer,iPos, iOrigin);
}

int ZFVFile::Tell()
{
	return ftell(m_pkFilePointer);
}

int ZFVFile::GetSize()
{
	return 0;
}











ZFVFileSystem::ZFVFileSystem()
: ZFObject("ZFVFileSystem")
{

}

ZFVFileSystem::~ZFVFileSystem()
{

}

FILE* ZFVFileSystem::Open(string strFileName, int iOptions)
{
	FILE*	pkFp;
	string	strRootMerge;

	// Try to open file directly.
	pkFp = fopen(strFileName.c_str(), "rb");
	if(pkFp)
		return pkFp;

	// Try to open from all active RootPaths.
	for(int i=0; i <m_kstrRootPath.size(); i++) {
		strRootMerge = m_kstrRootPath[i] + strFileName;
		pkFp = fopen(strFileName.c_str(), "rb");
		if(pkFp)
			return pkFp;
		}

	// Failed to open file.
	return NULL;
}

void ZFVFileSystem::AddRootPath(string strRootPath)
{
	m_kstrRootPath.push_back(strRootPath);
}

