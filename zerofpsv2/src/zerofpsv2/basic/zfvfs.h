#ifndef _BASIC_ZFVFS_H_
#define _BASIC_ZFVFS_H_

#include <vector>
#include <string>
#include "basic_x.h"
#include "zfobject.h"
#include "zfbasicfs.h"

using namespace std;

class ZFVFileSystem;

/// A Open File in the VFS
class BASIC_API ZFVFile
{
private:
	ZFVFileSystem*	m_pkFileSystem;

public:
	ZFVFile();
	~ZFVFile();

	FILE*			m_pkFilePointer;
	
	bool Open(string strFileName, int iOptions, bool bWrite );
	bool Close();

	bool Read  (void* pkData, int iSize, int iCount);
	bool Write (void* pkData, int iSize, int iCount);

	void Seek(int iPos, int iOrigin);
	int Tell();
	int GetSize();
};

/// ZeroFps Virtual File Systems.
class BASIC_API ZFVFileSystem : public ZFSubSystem 
{
	private:
		ZFBasicFS*	m_pkBasicFS;					
		vector<string>	m_kstrRootPath;				///< Active Root Paths (Maps dir into our VFS).

		string			m_kCurentDir;

	public:
		ZFVFileSystem();
		~ZFVFileSystem();

		void AddRootPath(string strRootPath);		///< Add path to list of active roots
		void RemoveRootPath(string strRootPath);	///< Remove a rootpath.
		void GetNumOfRootPaths();					///< Get num of active rootpaths

		string GetRootPath(int iIndex);				///< Get path with index (0 to NumOfPaths - 1). "" if not found.

		void Flush();								///< Close all unused archives.
		bool Exists(string strFileName);			///< Returns true if a file was found.
		

		// Open / Close
		void ArchiveOpen() { }					
		void ArchiveClose()  { }

		// Create / Manage
		void ArchiveUnpack()  { }
		void ArchivePack()  { }
	
		FILE* Open(string strFileName, int iOptions, bool bWrite);	


		string	GetCurrentWorkingDir();
		bool CreateDir(string strDir);
		bool RemoveDir(string strDir);
		bool ListDir(vector<string>* pkFiles, string strName, bool bOnlyMaps=false);
		bool ListDirFilter(vector<string>* pkFiles, vector<string>& pkFilters, 
			string strName, bool bIgnoreMaps = false);
		bool DirExist(string strName);

		enum FuncId_e
			{
			FID_CD,
			FID_DIR,
			};

		void RunCommand(int cmdid, const CmdArgument* kCommand);

		bool StartUp();
		bool ShutDown();
		bool IsValid();
};	





#endif






