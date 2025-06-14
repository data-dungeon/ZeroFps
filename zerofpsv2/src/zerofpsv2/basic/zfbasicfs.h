#ifndef _BASIC_ZFBASICFS_H_
#define _BASIC_ZFBASICFS_H_

#include <vector>
#include <string>
#include "basic_x.h"
#include "zfsubsystem.h"

//linux code
#ifndef WIN32
	#include <sys/types.h>
	#include <dirent.h>
	#include <sys/stat.h>
	#include <fcntl.h>   
	#include <unistd.h>
#endif


using namespace std;

/**	\brief	SubSystem for LowLevel FileIO.
		\ingroup Basic
*/
class BASIC_API ZSSBasicFS : public ZFSubSystem 
{
	char m_acDir[2048];

	public:
		ZSSBasicFS() : ZFSubSystem("ZSSBasicFS") 
		{
		}
		
		char* GetCWD();		
		bool CreateDir(const char* acName);
		bool RemoveDir(const char* acName);
		bool RemoveFile(const char* acName);
		
		bool ListDir(vector<string>* pkFiles, const char* acName, bool bOnlyMaps=false);
		bool ListDirFilter(vector<string>* pkFiles,const vector<string>& pkFilters, const char* acName, bool bIgnoreMaps=false);
		
		bool DirExist(const char* acName);
		bool FileExist(const char* szFile);

		bool IsDirectory(const char* szName);

		bool StartUp()	{ return true;	}
		bool ShutDown()	{ return true;	}
		bool IsValid()	{ return true;	}

};




#endif






