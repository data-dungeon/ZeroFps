#include <iostream>
#include "qpack.h"
#include "assert.h"

using namespace std;

PAKFile_Manger g_PakFileSystem;


PAKFile::PAKFile()
{
	strcpy(acPath, " ");
	iNumOfFiles = 0;
	akFiles.clear();
}

PAKFile::~PAKFile()
{
	akFiles.clear();
}

PAKFile::PAKFile(const PAKFile& pack)
{
	iNumOfFiles = pack.iNumOfFiles;
	strcpy(acPath,pack.acPath);

	// Copy File Table.
	akFiles = pack.akFiles;
}

PAKFile& PAKFile::operator=(const PAKFile& pack)
{
	iNumOfFiles = pack.iNumOfFiles;
	strcpy(acPath,pack.acPath);

	// Copy File Table.
	akFiles = pack.akFiles;
	return *this;
}

bool PAKFile::GetDir(void)
{
	FILE* fp = fopen(acPath,"rb");
	if(!fp)		return false;

	PAKHeader head;
	fread(&head,sizeof(PAKHeader),1,fp);

	iNumOfFiles = head.lDirSize / sizeof(PAKEntry);
	PAKEntry packentry;

	fseek(fp,head.lDirOffset,SEEK_SET);

	for(int i=0; i<iNumOfFiles; i++) {
		fread(&packentry,sizeof(PAKEntry),1,fp);
		akFiles.push_back(packentry);
		}

	fclose(fp);

	return true;
}

bool PAKFile::FindFile(const char* cFileName)
{
	vector<PAKEntry>::iterator pentry;

	for(pentry = akFiles.begin(); pentry != akFiles.end(); pentry++) {
		if(!strcmp((char*) pentry->ucFileName,cFileName))	return true;
		}

	return false;
}

bool PAKFile::Open(const char* cFileName, PAKFileFp* pkPakFp)
{
	vector<PAKEntry>::iterator pentry;

	for(pentry = akFiles.begin(); pentry != akFiles.end(); pentry++) {
		if(!strcmp((char*) pentry->ucFileName,cFileName))	{
			pkPakFp->pkFp	= fopen(acPath,"rb");
			pkPakFp->lBase	= pentry->lOffset;
			pkPakFp->lSize	= pentry->lSize;
			fseek(pkPakFp->pkFp,pkPakFp->lBase,SEEK_SET);
			return true;
			}
		
		}

	return false;
}


PAKFile_Manger::PAKFile_Manger()
{
	iNumOfPaks = 0;
}

bool PAKFile_Manger::IsValid(const char* filename)
{
	FILE* fp = fopen(filename,"rb");
	if(!fp)		return false;

	PAKHeader head;
	fread(&head,sizeof(PAKHeader),1,fp);

	if(head.ucMagic[0] != 'P')	return false;
	if(head.ucMagic[1] != 'A')	return false;
	if(head.ucMagic[2] != 'C')	return false;
	if(head.ucMagic[3] != 'K')	return false;

	fclose(fp);
	return true;
}

bool PAKFile_Manger::RegisterPak(const char* filename)
{
	if(!IsValid(filename)) {
//		Log << "Error: Failed to reg " << filename << "\n";	
		return false;
		}

	PAKFile newpack;
	strcpy(newpack.acPath, filename);
	newpack.GetDir();
	akPaks.push_back(newpack);
	iNumOfPaks++;

	return true;
}

bool PAKFile_Manger::FindFile(const char* filename)
{
	vector<PAKFile>::iterator pd;
	vector<PAKEntry>::iterator pentry;

	for(pd = akPaks.begin(); pd != akPaks.end(); pd++) {
		for(pentry = pd->akFiles.begin(); pentry != pd->akFiles.end(); pentry++) {
			if(!strcmp((char*) pentry->ucFileName,filename))	return true;
			}
		}

	return false;
}

bool PAKFile_Manger::Open(const char* filename, PAKFileFp* pakfp)
{
	vector<PAKFile>::iterator pd;
	vector<PAKEntry>::iterator pentry;

	for(pd = akPaks.begin(); pd != akPaks.end(); pd++) {
		for(pentry = pd->akFiles.begin(); pentry != pd->akFiles.end(); pentry++) {
			if(!strcmp((char*) pentry->ucFileName,filename))	{
				pakfp->pkFp = fopen(pd->acPath,"rb");
				pakfp->lBase	= pentry->lOffset;
				pakfp->lSize		= pentry->lSize;
				fseek(pakfp->pkFp,pakfp->lBase,SEEK_SET);
				return true;
				}
			
			}
		}

	return false;
}

void PAKFile_Manger::RemoveAll(void)
{
	akPaks.clear();
	iNumOfPaks = 0;
}

void PAKFile_Manger::Unpack(const char* ucPakFileName, const char* ucNewFileName)
{
	PAKFileFp pakfile;
	pakfile.Open(ucPakFileName);

	FILE* fp;
	fp = fopen(ucNewFileName,"wb");

	char* cRawData;

	cRawData = new char [pakfile.lSize];
	pakfile.Read(cRawData, 1, pakfile.lSize);
	fwrite(cRawData,1,pakfile.lSize, fp);
	delete [] cRawData;

	fclose(fp);
	pakfile.Close();
}




PAKFileFp::PAKFileFp()
{
	pkFp = NULL;
}

PAKFileFp::~PAKFileFp()
{
	Close();	
}

void PAKFileFp::Close(void)
{
	if(pkFp)	fclose(pkFp);
	pkFp = NULL;
}

bool PAKFileFp::Open(const char* cFileName)
{
	if(g_PakFileSystem.Open(cFileName,this))
		return true;
	else {
		lBase = 0;
		pkFp = fopen(cFileName,"rb");
		if(pkFp)	return true;
		}

	return false;
}

bool PAKFileFp::Open(const char* cFileName, char* cMode)
{
	assert(cFileName);
	assert(cMode);

	lBase = 0;

	pkFp = fopen(cFileName,cMode);
	if(pkFp)	return true;
	return false;
}

size_t PAKFileFp::Write(void* pvBuffer, size_t iSize,size_t iCount)
{
	assert(pvBuffer);
	return fwrite(pvBuffer,iSize,iCount, pkFp);
}

size_t PAKFileFp::Read(void* pvBuffer, size_t iSize,size_t iCount)
{
	assert(pvBuffer);
	return fread(pvBuffer,iSize,iCount, pkFp);
}

size_t PAKFileFp::Tell(void)
{
	return ftell(pkFp);
}

bool PAKFileFp::Seek(size_t iOffset, int iOrigin)
{
	int iReturnValue;

	switch(iOrigin)  {
		case SEEK_CUR:
			iReturnValue = fseek(pkFp,iOffset,iOrigin);
			break;

		case SEEK_END:
			iReturnValue = fseek(pkFp,lBase + iOffset,iOrigin);
			break;

		case SEEK_SET:
			iReturnValue = fseek(pkFp,lBase + iOffset,iOrigin);
			break;
		}

	//	return !fseek(pkFp,lBase + iOffset,iOrigin);
	return iReturnValue;
}

