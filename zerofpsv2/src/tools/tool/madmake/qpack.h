#ifndef FILE__QPACK_H
#define FILE__QPACK_H

/*
	Classes to load / read files in pak files (Quake1 / Quake 2)
*/

#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

class PAKFileFp;

struct PAKHeader
{ 
	unsigned char ucMagic[4];		// Name of the new WAD format
	long lDirOffset;				// Position of WAD directory from start of file
	long lDirSize;					// Number of entries * 0x40 (64 char)
};

struct PAKEntry
{ 
	unsigned char ucFileName[0x38];	// Name of the file, Unix style, with extension,
									// 50 chars, padded with '\0'.
	long lOffset;					// Position of the entry in PACK file
	long lSize;						// Size of the entry in PACK file
};

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

class PAKFile_Manger
{
public:
	PAKFile_Manger();

	vector<PAKFile> akPaks;
	int iNumOfPaks;

	bool IsValid(const char* filename);		
	bool RegisterPak(const char* filename);

	bool FindFile(const char* filename);
	bool Open(const char* filename, PAKFileFp* pakfp);
	void RemoveAll(void);
	void Unpack(const char* ucPakFileName, const char* ucNewFileName);


};

extern PAKFile_Manger g_PakFileSystem;

#endif