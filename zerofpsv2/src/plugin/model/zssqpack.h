#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef FILE__QPACK_H
#define FILE__QPACK_H

/*
	Classes to load / read files in pak files (Quake1 / Quake 2)
*/

#include <iostream>
#include <cstdio>
#include <vector>
#include "../../zerofpsv2/engine/application.h"

using namespace std;

class PAKFileFp;

/// Header of a PackFile
struct PAKHeader
{ 
	unsigned char ucMagic[4];		// Name of the new WAD format
	long lDirOffset;				// Position of WAD directory from start of file
	long lDirSize;					// Number of entries * 0x40 (64 char)
};

/// Entry inte a packFile
struct PAKEntry
{ 
	unsigned char ucFileName[0x38];	// Name of the file, Unix style, with extension,
									// 50 chars, padded with '\0'.
	long lOffset;					// Position of the entry in PACK file
	long lSize;						// Size of the entry in PACK file
};

/// A PackFile
class PAKFile 
{
public:
	PAKFile();
	~PAKFile();
	PAKFile(const PAKFile& pack);
	PAKFile&  operator=(const PAKFile& pack);
	
	char acPath[256];							// Path to packfile.
	int iNumOfFiles;							// Num of files.
	vector<PAKEntry>	akFiles;

	bool GetDir(void);
	bool FindFile(const char* cFileName);
	bool Open(const char* cFileName, PAKFileFp* pkPakFp);

};

/// PakFile File Pointer.
class PAKFileFp
{
public:
	PAKFileFp();
	~PAKFileFp();

	FILE*	pkFp;		// Filepointer to open file.
	long	lBase;		// Base offset of file.
	long	lSize;		// Size of file.

	bool	Open(const char* cFileName);
	void	Close(void);
	bool	Open(const char* cFileName, char* cMode);

	size_t	Write(void* pvBuffer, size_t iSize,size_t iCount);
	size_t	Read(void* pvBuffer, size_t iSize,size_t iCount);
	size_t	Tell(void);
	bool	Seek(size_t iOffset, int iOrigin);
};

/// PakFile Manger (Quake1/Quake2) 
class ZSSPakFile : ZFSubSystem
{
private:
	enum FuncId_e
	{
		FID_REG,
		FID_DEFPATHS,
	};

public:
	ZSSPakFile();
	virtual ~ZSSPakFile() {}

	vector<PAKFile> akPaks;
	int iNumOfPaks;

	bool IsValid(const char* filename);		
	bool RegisterPak(const char* filename);

	bool FindFile(const char* filename);
	bool Open(const char* filename, PAKFileFp* pakfp);
	void RemoveAll(void);
	void Unpack(const char* ucPakFileName, const char* ucNewFileName);

			
	void RunCommand(int cmdid, const CmdArgument* kCommand);
	bool StartUp()		{ return true;		}
	bool ShutDown()	{  return true;	}
	bool IsValid()		{ return true;		} 

};

#endif

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
