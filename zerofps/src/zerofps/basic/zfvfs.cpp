#include "zfobject.h"
#include "zfobjectmanger.h"

#include "zfvfs.h"

extern ZFObjectManger g_ZFObjSys;

ZFVFile::ZFVFile()
{
	m_pkFileSystem = static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		
	
}

ZFVFile::~ZFVFile()
{

}

bool ZFVFile::Open(string strFileName, int iOptions)
{
	m_pkFilePointer = m_pkFileSystem->Open(strFileName, iOptions);

	return false;
}

bool ZFVFile::Close()
{
	return false;

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
		
	return NULL;
}