//windows version oc zfbasicfs

#include "zfbasicfs.h"

#ifdef WIN32

bool ZFBasicFS::ListDirectory(vector<string>* pkFiles,const char* acName)
{


	return true;
}

bool ZFBasicFS::CreateDirectory(const char* acName)
{
	
	return true;
}

bool ZFBasicFS::RemoveDirectory(const char* acName)
{
	
	return true;
}

#endif

