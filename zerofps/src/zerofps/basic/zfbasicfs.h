#ifndef _ZFBASICFS_H_
#define _ZFBASICFS_H_

#include <vector>
#include <string>
#include "basic_x.h"

//linux code
#ifndef WIN32
	#include <sys/types.h>
	#include <dirent.h>
	#include <sys/stat.h>
	#include <fcntl.h>   
	#include <unistd.h>
#endif


using namespace std;


class BASIC_API ZFBasicFS
{
	
	public:
		bool CreateDir(const char* acName);
		bool RemoveDir(const char* acName);
		bool ListDir(vector<string>* pkFiles,const char* acName);

};

#endif






