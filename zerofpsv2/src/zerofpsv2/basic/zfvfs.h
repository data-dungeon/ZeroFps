#ifndef _BASIC_ZFVFS_H_
#define _BASIC_ZFVFS_H_

#include <vector>
#include <string>
#include "basic_x.h"
#include "zfsubsystem.h"
#include "zfbasicfs.h"
#include "zfio.h"

using namespace std;

class ZFVFileSystem;
class ZFIni;

/**	\brief	A Open File in the VFS
		\ingroup Basic

This class is used to describe a file in the ZFPS Virtual File Systems (VFS).
It should be used to open all files the ZeroFPS uses. 
*/
class BASIC_API ZFVFile : public ZFIoInterface
{
private:
	ZFVFileSystem*	m_pkFileSystem;

public:
	ZFVFile();
	~ZFVFile();

	FILE*			m_pkFilePointer;

	bool Open(string strFileName, int iOptions, bool bWrite );		// Open File.
	bool Close();																	// Close File.

	bool Read  (void* pkData, int iSize, int iCount);					// Read data from file.
	bool Write (void* pkData, int iSize, int iCount);					// Write data to file.

	void Seek(int iPos, int iOrigin);										// Seek to part of file.
	int Tell();																		// Tell current file pointer pos.
	int GetSize();																	// Get Size of file in bytes.
};

class VfsRootPath
{
public:
	VfsRootPath(string strRoot, string strVfs) : m_strRootPath(strRoot), m_strVfsPath(strVfs) {}
	string	m_strRootPath;
	string	m_strVfsPath;
};

/**	\brief	ZeroFps Virtual File Systems.
		\ingroup Basic

The VFS should be used for all file operations i ZeroFPS. Use ZFVFile to open and
close files with it.

The VFS can be set to many root paths. The base one is one step above the exe file.
*/

class BASIC_API ZFVFileSystem : public ZFSubSystem 
{
	private:
		friend class ZFVFile;
		friend class ZFIni;
		
		ZFBasicFS*				m_pkBasicFS;					
		vector<VfsRootPath>	m_kRootPath;
		string					m_strCurentDir;

		enum FuncId_e
		{
			FID_CD,
			FID_DIR,
			FID_LISTROOT,
			FID_VFSPATH,
			
		};

		FILE* Open(string strFileName, int iOptions, bool bWrite);	

	public:
		ZFVFileSystem();
		~ZFVFileSystem();

		void AddRootPath(string strRootPath, string strVfsPath);		///< Add path to list of active roots
		void RemoveRootPath(string strRootPath);	///< Remove a rootpath.
		int GetNumOfRootPaths()	{return int(m_kRootPath.size());};						///< Get num of active rootpaths

		string GetRootPath(int iIndex);				///< Get path with index (0 to NumOfPaths - 1). "" if not found.

		void Flush();										///< Close all unused archives.
		bool Exists(string strFileName);				///< Returns true if a file was found.
		
		// Open / Close
		void ArchiveOpen() 	{ }					
		void ArchiveClose()  { }

		// Create / Manage
		void ArchiveUnpack()	{ }
		void ArchivePack()  	{ }
	
		string GetFullPath(string strFileName);
		bool GetRootMerge(int iRootIndex, string strFileName, string& strRootMerge);

		string GetCurrentWorkingDir();
		bool CreateDir(string strDir);
		bool RemoveDir(string strDir);
		bool ListDir(vector<string>* pkFiles, string strName, bool bOnlyMaps=false);
		bool ListDirFilter(	vector<string>* pkFiles, vector<string>& pkFilters, 
									string strName, bool bIgnoreMaps = false);
		bool DirExist(string strName);

		void RunCommand(int cmdid, const CmdArgument* kCommand);

		bool StartUp();
		bool ShutDown();
		bool IsValid();
};	





#endif






