#include <iostream>
#include <cstdio>
#include <algorithm>
#include <functional>

#include "zfobject.h"
#include "zfobjectmanger.h"
#include "zfvfs.h"
#include "basicconsole.h"
#include "globals.h"
 
using namespace std;

extern ZFSystem g_ZFObjSys;

ZFVFile::ZFVFile()
{
	m_pkFileSystem = static_cast<ZFVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZFVFileSystem"));		
	m_pkFilePointer = NULL;
	
}

ZFVFile::~ZFVFile()
{
	Close();
}

bool ZFVFile::Open(string strFileName, int iOptions, bool bWrite)
{
	m_pkFilePointer = m_pkFileSystem->Open(strFileName, iOptions,bWrite);
	if(m_pkFilePointer)
		return true;

	return false;
}

bool ZFVFile::Close()
{
	if(m_pkFilePointer)
		fclose(m_pkFilePointer);

	m_pkFilePointer = NULL;
	return true;
}

bool ZFVFile::Read  (void* pkData, int iSize, int iCount)
{
	return (fread(pkData, iSize, iCount, m_pkFilePointer) != 0);
}

bool ZFVFile::Write (void* pkData, int iSize, int iCount)
{
	return (fwrite(pkData, iSize, iCount, m_pkFilePointer) != 0);
}

void ZFVFile::Seek(int iPos, int iOrigin)
{
	fseek(m_pkFilePointer,iPos, iOrigin);
}

int ZFVFile::Tell()
{
	return ftell(m_pkFilePointer);
}

int ZFVFile::GetSize()
{
	return 0;
}











ZFVFileSystem::ZFVFileSystem()
: ZFSubSystem("ZFVFileSystem")
{
	Register_Cmd("cd",	FID_CD);
	Register_Cmd("root", FID_LISTROOT);
	Register_Cmd("dir",	FID_DIR);
	m_kCurentDir = "";
}

bool ZFVFileSystem::StartUp()
{ 
	m_pkBasicFS = static_cast<ZFBasicFS*>(GetSystem().GetObjectPtr("ZFBasicFS"));		
	return true;
}

bool ZFVFileSystem::ShutDown()
{ 
	return true;	
}

bool ZFVFileSystem::IsValid()	
{ 
	return true;
}




ZFVFileSystem::~ZFVFileSystem()
{

}

FILE* ZFVFileSystem::Open(string strFileName, int iOptions, bool bWrite)
{
	FILE*	pkFp;
	string	strRootMerge;
	char*	szOptions;
	if(bWrite)
		szOptions = "wb";
	else
		szOptions = "rb";
	
	// Try to open file directly.
	pkFp = fopen(strFileName.c_str(), szOptions);
	if(pkFp)
		return pkFp;

	// Try to open from all active RootPaths.
	for(unsigned int i=0; i <m_kstrRootPath.size(); i++) {
		strRootMerge = m_kstrRootPath[i] + strFileName;
		pkFp = fopen(strRootMerge.c_str(), szOptions);
		if(pkFp) {
			//cout << "Path: " << strRootMerge.c_str();
			//cout << "\n";
			return pkFp;
			}
		}

	// Failed to open file.
	return NULL;
}

void ZFVFileSystem::AddRootPath(string strRootPath)
{
	cout << "Adding to VFS root table: " <<  strRootPath << endl;
	g_Logf("Adding %s to VFS root table\n", strRootPath.c_str());
	m_kstrRootPath.push_back(strRootPath);
	int iSize = m_kstrRootPath.size();
}

string	ZFVFileSystem::GetCurrentWorkingDir()
{
	string strWorkDir = m_pkBasicFS->GetCWD();
	return strWorkDir;
}

bool ZFVFileSystem::CreateDir(string strDir)
{
	return m_pkBasicFS->CreateDir(strDir.c_str());
}

bool ZFVFileSystem::RemoveDir(string strDir)
{
	return m_pkBasicFS->RemoveDir(strDir.c_str());
}

bool ZFVFileSystem::ListDir(vector<string>* pkFiles, string strName, bool bOnlyMaps)
{
	string	strRootMerge;

	int iSize = m_kstrRootPath.size();

	// Try to open from all active RootPaths.
	for(unsigned int i=0; i <m_kstrRootPath.size(); i++) {
		strRootMerge = m_kstrRootPath[i] + strName;
		cout << "ListDir: " << strRootMerge.c_str();
		cout << endl;
		m_pkBasicFS->ListDir(pkFiles, strRootMerge.c_str(), bOnlyMaps);
		}

	// Remove All ..
	vector<string>::iterator start	=  pkFiles->begin();
	vector<string>::iterator end		=	pkFiles->end();
	remove(start, end, string(".."));

	// Remove All ..
	start	=  pkFiles->begin();
	end		=	pkFiles->end();
	sort(start, end);

	return true;
}

bool ZFVFileSystem::ListDirFilter(vector<string>* pkFiles, vector<string>& pkFilters, 
	string strName, bool bIgnoreMaps)
{
	return m_pkBasicFS->ListDirFilter(pkFiles, pkFilters, strName.c_str(), bIgnoreMaps);

}

bool ZFVFileSystem::DirExist(string strName)
{
	return m_pkBasicFS->DirExist(strName.c_str());
}

void ZFVFileSystem::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	BasicConsole* m_pkConsole = static_cast<BasicConsole*>(GetSystem().GetObjectPtr("Console"));		
	unsigned int i;
	vector<string> kFiles;

	switch(cmdid) {
		case FID_CD:
			if(kCommand->m_kSplitCommand.size() <= 1) {
				m_pkConsole->Printf(m_kCurentDir.c_str());
				return;
			}
			
			// special case when entering .. directory
			if(kCommand->m_kSplitCommand[1]=="..")
			{
				for(int i=m_kCurentDir.length()-1;i>0;i--)
				{
					if(m_kCurentDir[i]=='/'){
						m_kCurentDir[i]='\0';						
						m_kCurentDir=m_kCurentDir.c_str();
						break;
					}
				}
				m_pkConsole->Printf(m_kCurentDir.c_str());				
				break;
			}					
				
			//enter a normal directory
			if(m_pkBasicFS->ListDir(&kFiles,(m_kCurentDir+"/"+kCommand->m_kSplitCommand[1]).c_str()))
			{
				m_kCurentDir += "/";
				m_kCurentDir += kCommand->m_kSplitCommand[1];
				m_pkConsole->Printf(m_kCurentDir.c_str());
			} else
			{
				m_pkConsole->Printf("Cant change directory");
				break;
			}
					
			kFiles.clear();
					
			break;
		
		case FID_DIR:	

				ListDir(&kFiles,m_kCurentDir, true);
				
				m_pkConsole->Printf("DIRECTORY %s",m_kCurentDir.c_str());
				m_pkConsole->Printf("");
				for(i=0;i<kFiles.size();i++)
				{
					m_pkConsole->Printf("<%s>", kFiles[i].c_str());
				}

				/*m_pkConsole->Printf("%i files",kFiles.size());		
				m_pkBasicFS->ListDir(&kFiles,m_kCurentDir.c_str());

				kFiles.clear();
				m_pkConsole->Printf("Listing %s",m_kCurentDir.c_str());
				for(int i=0;i<kFiles.size();i++)
				{
					m_pkConsole->Printf(kFiles[i].c_str());
				}*/
			
			break;

		case FID_LISTROOT:
			for(unsigned int i=0; i <m_kstrRootPath.size(); i++) {
				GetSystem().Printf(" Root[%d] = %s", i, m_kstrRootPath[i].c_str()); 
				}
			
			break;
	};
}
