#include <iostream>
#include <cstdio>
#include <algorithm>
#include <functional>

#include "zfsubsystem.h"
#include "zfsystem.h"
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
	int size;
	int pos = Tell();
	
	fseek(m_pkFilePointer,0,SEEK_END);
	size = Tell();	
	
	fseek(m_pkFilePointer,pos,SEEK_SET);

	return size;
}







ZFVFileSystem::ZFVFileSystem()
: ZFSubSystem("ZFVFileSystem")
{
	m_pkBasicFS = static_cast<ZFBasicFS*>(GetSystem().GetObjectPtr("ZFBasicFS"));		
	
	
	Register_Cmd("cd",		FID_CD);
	Register_Cmd("root",	 	FID_LISTROOT);
	Register_Cmd("dir",		FID_DIR);
	Register_Cmd("ls",		FID_DIR);
	Register_Cmd("vfspath",	FID_VFSPATH);
	m_strCurentDir = "";


	AddRootPath("./","/"); 
	AddRootPath("../datafiles/sysdata" ,"/data");
	AddRootPath("../datafiles/extdata" ,"/data");

}

bool ZFVFileSystem::StartUp()
{ 
	
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

bool ZFVFileSystem::GetRootMerge(int iRootIndex, string strFileName, string& strRootMerge)
{
	// always convert to an absolut path, 
	if(strFileName[0] != '/')
		strFileName = string("/") + strFileName;
		//strFileName.insert(0,1,'/');
		//strFileName.erase(0,1);

	if(m_kRootPath[iRootIndex].m_strVfsPath.length() == 1)
	{
		// This is a pure root path that maps to the base of the VFS
		strRootMerge = m_kRootPath[ iRootIndex ].m_strRootPath + strFileName;
	}
	else
	{
		// This is a path that maps into the VFS system.
		if(strncmp( m_kRootPath[ iRootIndex ].m_strVfsPath.c_str(), strFileName.c_str(), 
			m_kRootPath[ iRootIndex ].m_strVfsPath.length()) == 0)
		{
			strFileName.erase(0, m_kRootPath[ iRootIndex ].m_strVfsPath.length() );
			strRootMerge = m_kRootPath[ iRootIndex ].m_strRootPath + strFileName;
		}
		else
			return false;
	}

	return true;
}


FILE* ZFVFileSystem::Open(string strFileName, int iOptions, bool bWrite)
{
	FILE*	pkFp;
	string	strRootMerge;
	char*	szOptions;
	if(bWrite)
		szOptions = "w+b";
	else
		szOptions = "rb";
	
	// Try to open file directly.  (this shuld not exist)
/*	pkFp = fopen(strFileName.c_str(), szOptions);
	if(pkFp)
		return pkFp;
*/
	
	// Try to open from all active RootPaths.
	unsigned int num_paths = m_kRootPath.size();	
	for(unsigned int i=0; i <num_paths; i++) 
	{
		if(GetRootMerge(i, strFileName, strRootMerge)) 
		{
			pkFp = fopen(strRootMerge.c_str(), szOptions);
			if(pkFp) {
				return pkFp;
				}

		}
	}

	//dvoid hack
	//if file still not open and it was opened for writing create directory
	if(bWrite)
	{
		string dir  = "";
		for(int i = strFileName.size();i > 0 ;i--)
		{
			if( strFileName[i] == '/')
			{
				dir  = strFileName.substr(0,i);
				
				//create directory
				if(!m_pkBasicFS->CreateDir(dir.c_str()))
					cout<<"ERROR: creating directory "<<dir<<" failed"<<endl;
				
				//try to open the file now			
				pkFp = fopen(strFileName.c_str(), szOptions);
				if(pkFp)
					return pkFp;
				else
					return NULL;
			}
		}
	}

	// Failed to open file.
	return NULL;
}

string ZFVFileSystem::GetFullPath(string strFileName)
{
	FILE*	pkFp;
	string strRootMerge;
	
	// Try to open from all active RootPaths.
	for(unsigned int i=0; i <m_kRootPath.size(); i++) 
	{
		//strRootMerge = m_kRootPath[i].m_strRootPath + strFileName;
		if(GetRootMerge(i, strFileName, strRootMerge)) 
		{
			pkFp = fopen(strRootMerge.c_str(), "rb");
			if(pkFp) 
			{
				fclose(pkFp);
				return strRootMerge;
			}
		}
	}

	strRootMerge = "";
	return strRootMerge;
}


void ZFVFileSystem::AddRootPath(string strRootPath, string strVfsPath)
{
	//always absolut vfs paths
	if(strVfsPath[0] != '/')
		strVfsPath = string("/") + strVfsPath; 
		//strVfsPath.insert(0,1,'/');
	
	//no / at the end of either real or vfs path
	if(strVfsPath.length() != 1)
		if(strVfsPath[strVfsPath.length()-1] == '/')
			strVfsPath.erase(strVfsPath.length()-1,1);
			
	if(strRootPath.length() != 1)
		if(strRootPath[strRootPath.length()-1] == '/')
			strRootPath.erase(strRootPath.length()-1,1);
			

	g_Logf("Mapping system %s to VFS %s\n", strRootPath.c_str(),strVfsPath.c_str());
	cout<<"Mapping:"<<strRootPath<<" to "<<strVfsPath<<endl;
	
	m_kRootPath.insert(m_kRootPath.begin(),VfsRootPath(strRootPath, strVfsPath));
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

	// Try to open from all active RootPaths.
	for(unsigned int i=0; i <m_kRootPath.size(); i++)
	{
		//strRootMerge = m_kRootPath[i].m_strRootPath + strName;
		if(GetRootMerge(i, strName, strRootMerge)) 
		{
			//cout<<"dir:"<<strRootMerge<<endl;
			m_pkBasicFS->ListDir(pkFiles, strRootMerge.c_str(), bOnlyMaps);
		}	
	}

	
	// Remove All ..

	/*
	remove(start, end, string(".."));

	// Remove All .
	start	=  pkFiles->begin();
	end	=	pkFiles->end();
	remove(start, end, string("."));
	*/
	
	//add virtual directories
	for(unsigned int i=0; i <m_kRootPath.size(); i++)
	{
		if(m_kRootPath[i].m_strVfsPath.length() >= 2)
		{
			string Parent;
			int pos = m_kRootPath[i].m_strVfsPath.rfind('/');
			Parent.append(m_kRootPath[i].m_strVfsPath,0,pos);
			
			if(Parent == strName)
			{			
				string Name;
				Name.append(m_kRootPath[i].m_strVfsPath,pos+1,m_kRootPath[i].m_strVfsPath.length()-pos-1);
				pkFiles->push_back(Name);
			}
		}
	}
	
	// sort All
	vector<string>::iterator start	=  pkFiles->begin();
	vector<string>::iterator end		=	pkFiles->end();
	sort(start, end);

	//remove duplicates (WARNING, this algoritm only works if the list is sorted by name)
	for(vector<string>::iterator it = pkFiles->begin();it != pkFiles->end();it++)
	{	
		if( (it+1) == pkFiles->end())
			continue;
		if( *it == *(it+1))
		{
			pkFiles->erase(it+1);
			it--;
		}
	}
	
	return true;
}

bool ZFVFileSystem::ListDirFilter(vector<string>* pkFiles, vector<string>& pkFilters, 
	string strName, bool bIgnoreMaps)
{
	return m_pkBasicFS->ListDirFilter(pkFiles, pkFilters, strName.c_str(), bIgnoreMaps);

}

bool ZFVFileSystem::DirExist(string strName)
{
	//try to list directory
	vector<string> kDirs;
	ListDir(&kDirs,strName);
	
	//if there was files in it, it exists
	if(kDirs.size() != 0)
		return true;
	
	return false;
}

void ZFVFileSystem::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	BasicConsole* m_pkConsole = static_cast<BasicConsole*>(GetSystem().GetObjectPtr("Console"));		
	unsigned int i;
	vector<string> kFiles;
	string strRootMerge;

	switch(cmdid) {
		case FID_CD:
		{
			if(kCommand->m_kSplitCommand.size() <= 1) 
			{
				m_pkConsole->Printf(m_strCurentDir.c_str());
				return;
			}
			
			// special case when entering .. directory
			if(kCommand->m_kSplitCommand[1] == "..")
			{
				if(m_strCurentDir.length() == 0)
				{
					m_pkConsole->Printf("Already at the root");
					break;
				}
					
				int iPos = m_strCurentDir.rfind('/');
				m_strCurentDir.erase(iPos);
				break;
			}			
			
			//enter a normal directory
			if(DirExist(m_strCurentDir+"/"+kCommand->m_kSplitCommand[1]))
			{
				m_strCurentDir = m_strCurentDir+"/"+kCommand->m_kSplitCommand[1];				
				m_pkConsole->Printf(m_strCurentDir.c_str());
			}
			else
				m_pkConsole->Printf("%s does not exist",kCommand->m_kSplitCommand[1].c_str());

			break;
		}
			
		case FID_DIR:	
		{		
			bool bDirOnly = false;
		
			if(kCommand->m_kSplitCommand.size() == 2) 
				if(kCommand->m_kSplitCommand[1] == "-d")
					bDirOnly = true;				
	
			ListDir(&kFiles,m_strCurentDir,bDirOnly);			
			
			m_pkConsole->Printf(" ");
			m_pkConsole->Printf("DIRECTORY %s",m_strCurentDir.c_str());
			m_pkConsole->Printf(" ");
			for(i=0;i<kFiles.size();i++)
			{
				m_pkConsole->Printf("<%s>", kFiles[i].c_str());
			}
			m_pkConsole->Printf(" ");
			m_pkConsole->Printf("%d files in directory",kFiles.size());
							
			break;
		}

		case FID_LISTROOT:
			// Check for file in all root dirs.
			GetSystem().Printf("Now Checking for %s in rootpaths.", kCommand->m_kSplitCommand[1].c_str()); 

			for(unsigned int i=0; i <m_kRootPath.size(); i++)
			{
				if(GetRootMerge(i, kCommand->m_kSplitCommand[1], strRootMerge))
				{
					GetSystem().Printf(" [%d] = Yes : %s ", i, strRootMerge.c_str()); 
				}
				else 
				{
					GetSystem().Printf(" [%d] = No : %s ", i, strRootMerge.c_str()); 
				}
			}
		break;

			
		case FID_VFSPATH:
		{
			for(unsigned int i=0; i <m_kRootPath.size(); i++)
			{
				GetSystem().Printf(" Root[%d] = %s maps to %s", i, m_kRootPath[i].m_strRootPath.c_str(),
					m_kRootPath[i].m_strVfsPath.c_str()); 
			}
		
			break;		
		}
			
	};
}
