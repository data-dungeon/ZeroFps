#include "zfbasicfs.h"

bool ZFBasicFS::ListDirectory(vector<string>* pkFiles,const char* acName)
{

// linux code
#ifndef WIN32
	DIR* kDir;
	
	kDir=opendir(acName);
	if(kDir==NULL)
		return false;
	
	
	dirent* kDirEnt;
	while( (kDirEnt=readdir(kDir)) != NULL)
	{
		string kNamn;
		kNamn=kDirEnt->d_name;
		pkFiles->push_back(kNamn);
	}
	
	return true;
#endif

}

bool ZFBasicFS::CreateDirectory(const char* acName)
{
	


}



