#ifndef _ZFBASICFS_H_
#define _ZFBASICFS_H_

#include <vector>
#include <string>
#include "basic_x.h"

//linux code
#ifndef WIN32
#include <sys/types.h>
#include <dirent.h>
#endif


using namespace std;


class BASIC_API ZFBasicFS
{
	
	public:
		bool CreateDirectory(const char* acName);
		bool RemoveDirectory(const char* acName);
		bool ListDirectory(vector<string>* pkFiles,const char* acName);

};

#endif






