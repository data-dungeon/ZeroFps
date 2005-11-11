#ifndef _BASIC_ZFVFS_H_
#define _BASIC_ZFVFS_H_

#include <vector>
#include <string>
#include "basic_x.h"
#include "zfsubsystem.h"
#include "zfio.h"
#include "concommand.h"

using namespace std;

class ZSSVFileSystem;
class ZFIni;
class ZSSBasicFS;

/**	\brief	A Open File in the VFS
		\ingroup Basic

This class is used to describe a file in the ZFPS Virtual File Systems (VFS).
It should be used to open all files the ZeroFPS uses. 
*/
class BASIC_API ZFVFile : public ZFIoInterface
{
private:
	ZSSVFileSystem*	m_pkFileSystem;

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

class BASIC_API ZSSVFileSystem : public ZFSubSystem 
{
	private:
		enum FuncId_e
		{
			FID_CD,
			FID_DIR,
			FID_LISTROOT,
			FID_VFSPATH,			
			FID_ADDROOTPATH,
		};

				
		ZSSBasicFS*				m_pkBasicFS;					
		vector<VfsRootPath>	m_kRootPath;
		string					m_strCurentDir;

		//bool						m_bCaseSensitive;
		ConVar					m_kCaseSensitive;

		FILE* Open(string strFileName, int iOptions, bool bWrite);	

		struct SORT_FILES : public binary_function<string, string, bool> {
			bool operator()(string x, string y) {return (y > x);}
		} SortFiles;

	public:
		ZSSVFileSystem();
		~ZSSVFileSystem();
		void RunCommand(int cmdid, const ConCommandLine* kCommand);
		bool StartUp();
		bool ShutDown();
		bool IsValid();

		//vfs handling
		/// Add path to list of active roots
		void AddRootPath(string strRootPath, string strVfsPath);		
		/// Remove a rootpath.
		void RemoveRootPath(string strRootPath);	
	
		/// returns full path to a file
		string GetFullPath(string strFileName);
		bool GetRootMerge(int iRootIndex, string strFileName, string& strRootMerge);

		string GetRealName(const string& strName);		
		string GetCurrentWorkingDir();
		
		bool CreateDir(const string& strDir);
		bool RemoveDir(const string& strDir);		
		bool RemoveFile(const string& strFile);
		
		bool ListDir(vector<string>& kFiles, const string& strName, bool bOnlyMaps=false);
		bool ListDirFilter(vector<string>& kFiles, const vector<string>& pkFilters, const string& strName, bool bIgnoreMaps = false);
		void ListDirRecursive(vector<string>& kFiles, string strRootPath,const vector<string>& kExtensions, bool bAlsoFolders=false); ///< Search ALL dirs, starting from root dir.
		
		bool DirExist(const string& strName);						///< Returns true if a directory was found.
		bool FileExists(const string& strFileName);				///< Returns true if a file was found.




		friend class ZFVFile;
		friend class ZFIni;
};	





#endif






