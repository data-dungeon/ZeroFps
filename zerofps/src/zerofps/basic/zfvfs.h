#ifndef _ZFVFS_H_
#define _ZFVFS_H_

#include <vector>
#include <string>
#include "basic_x.h"
#include "zfobject.h"

using namespace std;

class ZFVFileSystem;

class BASIC_API ZFVFile
{
private:
	ZFVFileSystem*	m_pkFileSystem;
	FILE*			m_pkFilePointer;

public:
	ZFVFile();
	~ZFVFile();
	
	bool Open(string strFileName, int iOptions, bool bWrite );
	bool Close();

	bool Read  (void* pkData, int iSize, int iCount);
	bool Write (void* pkData, int iSize, int iCount);

	void Seek(int iPos, int iOrigin);
	int Tell();
	int GetSize();
};

class BASIC_API ZFVFileSystem : public ZFObject 
{
	private:
		vector<string>	m_kstrRootPath;				// Active root paths.
	

	public:
		ZFVFileSystem();
		~ZFVFileSystem();

		// Root Paths (Maps dir into our VFS)
		void AddRootPath(string strRootPath);		// Add path to list of active roots
		void RemoveRootPath(string strRootPath);	// Remove a rootpath.
		void GetNumOfRootPaths();					// Get num of active rootpaths

		string GetRootPath(int iIndex);				// Get path with index (0 to NumOfPaths - 1). "" if not found.

		void Flush();								// Close all unused archives.
		bool Exists(string strFileName);			// Returns true if a file was found.
		

		// Open / Close
		void ArchiveOpen() { }
		void ArchiveClose()  { }

		// Create / Manage
		void ArchiveUnpack()  { }
		void ArchivePack()  { }
	
		FILE* Open(string strFileName, int iOptions, bool bWrite);	
};	





#endif






