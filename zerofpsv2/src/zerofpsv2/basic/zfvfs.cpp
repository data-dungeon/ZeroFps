#include <iostream>
#include <cstdio>
#include <algorithm>
#include <functional>

#include "zfsubsystem.h"
#include "zfsystem.h"
#include "zfvfs.h"
//#include "basicconsole.h"
#include "globals.h"
#include "zfbasicfs.h"

#include <set>
#include <list>

using namespace std;

extern ZFSystem g_ZFObjSys;

ZFVFile::ZFVFile()
{
	m_pkFileSystem = static_cast<ZSSVFileSystem*>(g_ZFObjSys.GetObjectPtr("ZSSVFileSystem"));
	
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







ZSSVFileSystem::ZSSVFileSystem()
: ZFSubSystem("ZSSVFileSystem")
{
	m_pkBasicFS = static_cast<ZSSBasicFS*>(GetSystem().GetObjectPtr("ZSSBasicFS"));		
	
	m_strCurentDir = "";
	//m_bCaseSensitive = false;
	
	Register_Cmd("cd",			FID_CD);
	Register_Cmd("root",	 		FID_LISTROOT);
	Register_Cmd("dir",			FID_DIR);
	Register_Cmd("ls",			FID_DIR);
	Register_Cmd("vfspath",		FID_VFSPATH);
	Register_Cmd("addrootpath",FID_ADDROOTPATH); 
	
//	RegisterVariable("f_casesensitive",		&m_bCaseSensitive,	CSYS_BOOL);
	m_kCaseSensitive.Register(this, "f_casesensitive", "0","");

//	AddRootPath("/","/"); 
	AddRootPath("./","/"); 
	AddRootPath("../datafiles/sysdata" ,"/data");
	AddRootPath("../datafiles/extdata" ,"/data");

}

bool ZSSVFileSystem::StartUp()
{ 
	char* strDir = m_pkBasicFS->GetCWD();	
	cout << "WorkingDir is: " << strDir << endl;
	m_pkBasicFS->CreateDir("logs/");
	m_pkBasicFS->CreateDir("screenshots/");
	
	return true;
}

bool ZSSVFileSystem::ShutDown()
{ 
	return true;	
}

bool ZSSVFileSystem::IsValid()	
{ 
	return true;
}




ZSSVFileSystem::~ZSSVFileSystem()
{

}

bool ZSSVFileSystem::GetRootMerge(int iRootIndex, string strFileName, string& strRootMerge)
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


FILE* ZSSVFileSystem::Open(string strFileName, int iOptions, bool bWrite)
{

	FILE*	pkFp;
	string	strRootMerge;
	char*	szOptions;
	if(bWrite)
		szOptions = "w+b";
	else
		szOptions = "rb";
	
	
	// Try to open file directly.  (this should not exist)
	pkFp = fopen(strFileName.c_str(), szOptions);
	if(pkFp)
		return pkFp;
	
	
	// Try to open from all active RootPaths.
	unsigned int num_paths = m_kRootPath.size();	
	for(unsigned int i=0; i <num_paths; i++) 
	{
		if(GetRootMerge(i, strFileName, strRootMerge)) 
		{
			//if not case sensetive, try to find the real filename
			if(!m_kCaseSensitive.GetBool())
				strRootMerge = GetRealName(strRootMerge);
			
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

string ZSSVFileSystem::GetFullPath(string strFileName)
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


void ZSSVFileSystem::AddRootPath(string strRootPath, string strVfsPath)
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
	//cout<<"Mapping:"<<strRootPath<<" to "<<strVfsPath<<endl;
	
	m_kRootPath.insert(m_kRootPath.begin(),VfsRootPath(strRootPath, strVfsPath));
}

string	ZSSVFileSystem::GetCurrentWorkingDir()
{
	string strWorkDir = m_pkBasicFS->GetCWD();
	return strWorkDir;
}

bool ZSSVFileSystem::CreateDir(const string& strDir)
{
	return m_pkBasicFS->CreateDir(strDir.c_str());
}

bool ZSSVFileSystem::RemoveDir(const string& strDir)
{
	return m_pkBasicFS->RemoveDir(strDir.c_str());
}

bool ZSSVFileSystem::RemoveFile(const string& strFile)
{
	return m_pkBasicFS->RemoveFile(strFile.c_str());
}


void ZSSVFileSystem::ListDirRecursive(vector<string>& kFiles, string strRootPath, 
												 const vector<string>& szExtensions, bool bAlsoFolders)
{
	if(strRootPath.size() > 1)
	{
		char last_char[] = { strRootPath[strRootPath.size()-1], '\0' };
		if(!(last_char[0] == '/' || last_char[0] == '\\'))
			strRootPath += "/";
	}

	set<string> kSearchedFiles;
	list<string> dir_list;

	dir_list.push_back(strRootPath);

	while(1)
	{
		list<string> vkFileNames;
		string currentFolder = dir_list.back();

		// H�mta filerna i den aktuella katalogen och sortera listan.
		vector<string> t;
		ListDir(t, currentFolder);
		for(unsigned int i=0; i<t.size(); i++)
			vkFileNames.push_back(t[i]); 
		t.clear(); vkFileNames.sort(SortFiles);

		// L�gg till alla filer
		for(list<string>::iterator it = vkFileNames.begin(); it != vkFileNames.end(); it++)  
		{
			string strFile = (*it);
			string id = currentFolder + strFile;

			bool bIsFolder = strFile.find(".") == string::npos;

			if(kSearchedFiles.find(id) == kSearchedFiles.end())
			{			
				if(bIsFolder)		
				{
					string d;

					char last_char[] = { currentFolder[currentFolder.size()-1], '\0' };
					if(!(last_char[0] == '/' || last_char[0] == '\\'))
						d = currentFolder + string("/") + strFile; 
					else
						d = currentFolder + strFile;

					dir_list.push_back( d );					
				}
				
				if(strFile != "..")
				{
					char *ext = strrchr( strFile.c_str(), '.');

					bool bAdd = false;

					if(ext != NULL)
					{					
						for(unsigned int i=0; i<szExtensions.size(); i++)
						{
							if(strcmp(ext, szExtensions[i].c_str()) == 0)
							{
								
								if(currentFolder.size() > 1)
								{
									bAdd = true;
									//char last_char[] = { currentFolder[currentFolder.size()-1], '\0' };
									//if(!(last_char[0] == '/' || last_char[0] == '\\'))
									//	vkFiles->push_back( currentFolder + "/" + strFile );
									//else
									//	vkFiles->push_back( currentFolder + strFile );

									//// ta bort rotpathen
									//vkFiles->back().erase(0, strRootPath.length());

									//int pp;
									//if((pp=vkFiles->back().find("//"))!= string::npos)
									//	vkFiles->back().erase(pp,1);
								}								
							}
						}
					}
					
					if(bAlsoFolders)
					{
						bAdd = true;
					}

					if(bAdd)
					{
						char last_char[] = { currentFolder[currentFolder.size()-1], '\0' };
						if(!(last_char[0] == '/' || last_char[0] == '\\'))
							kFiles.push_back( currentFolder + "/" + strFile );
						else
							kFiles.push_back( currentFolder + strFile );

						// ta bort rotpathen
						kFiles.back().erase(0, strRootPath.length());						
					}
				}

				kSearchedFiles.insert(id);
			}
		}

		// ej klivit in i ett nytt dir? g� tillbaks
		if(dir_list.back() == currentFolder)
		{
			// sista?
			if(currentFolder == strRootPath)
				break;

			dir_list.pop_back();
		}
	}
}

bool ZSSVFileSystem::ListDir(vector<string>& kFiles, const string& strName, bool bOnlyMaps)
{
	string	strRootMerge;

	// Try to open from all active RootPaths.
	for(unsigned int i=0; i <m_kRootPath.size(); i++)
	{
		//strRootMerge = m_kRootPath[i].m_strRootPath + strName;
		if(GetRootMerge(i, strName, strRootMerge)) 
		{
			//cout<<"dir:"<<strRootMerge<<endl;
			m_pkBasicFS->ListDir(&kFiles, strRootMerge.c_str(), bOnlyMaps);
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
				kFiles.push_back(Name);
			}
		}
	}
	
	// sort All
	vector<string>::iterator start	=  kFiles.begin();
	vector<string>::iterator end		=	kFiles.end();
	sort(start, end);

	//remove duplicates (WARNING, this algoritm only works if the list is sorted by name)
	for(vector<string>::iterator it = kFiles.begin();it != kFiles.end();it++)
	{	
		if( (it+1) == kFiles.end())
			continue;
		if( *it == *(it+1))
		{
			kFiles.erase(it+1);
			it--;
		}
	}
	
		
	return true;
}

bool ZSSVFileSystem::ListDirFilter(vector<string>& kFiles,const vector<string>& kFilters, const string& strName, bool bIgnoreMaps)
{
// 	return m_pkBasicFS->ListDirFilter(&kFiles, kFilters, strName.c_str(), bIgnoreMaps);

	string	strRootMerge;

	// Try to open from all active RootPaths.
	for(unsigned int i=0; i <m_kRootPath.size(); i++)
	{
		//strRootMerge = m_kRootPath[i].m_strRootPath + strName;
		if(GetRootMerge(i, strName, strRootMerge)) 
		{
			//cout<<"dir:"<<strRootMerge<<endl;
			m_pkBasicFS->ListDirFilter(&kFiles,kFilters, strRootMerge.c_str(), bIgnoreMaps);
		}	
	}

	
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
				kFiles.push_back(Name);
			}
		}
	}
	
	// sort All
	vector<string>::iterator start	=  kFiles.begin();
	vector<string>::iterator end		=	kFiles.end();
	sort(start, end);

	//remove duplicates (WARNING, this algoritm only works if the list is sorted by name)
	for(vector<string>::iterator it = kFiles.begin();it != kFiles.end();it++)
	{	
		if( (it+1) == kFiles.end())
			continue;
		if( *it == *(it+1))
		{
			kFiles.erase(it+1);
			it--;
		}
	}
	
		
	return true;
}

bool ZSSVFileSystem::DirExist(const string& strName)
{
	//try to list directory
	vector<string> kDirs;
	ListDir(kDirs,strName);
	
	//if there was files in it, it exists
	if(kDirs.size() != 0)
		return true;
	
	return false;
}

bool ZSSVFileSystem::FileExists(const string& strName)
{
	string strDir;
	string strFile;
	
	int pos = strName.rfind('/');
	if(pos == -1)
	{
		strDir="/";
		strFile = strName;
	}
	else
	{
		strDir.append(strName,0,pos);
		strFile.append(strName,pos+1,strName.length() - pos);
	}
	
	//try to list directory
	vector<string> kDirs;
	ListDir(kDirs,strDir);
	
	if(m_kCaseSensitive.GetBool())
	for(unsigned int i = 0;i<kDirs.size();i++)
	{
		if(m_kCaseSensitive.GetBool())
		{
			if(kDirs[i] == strName)
				return true;
		}
		else
		{
			if(IsSameIgnoreCase(kDirs[i].c_str(),strFile.c_str()))
				return true;		
		}	
	}
	
	return false;
}

string ZSSVFileSystem::GetRealName(const string& strName)
{
	string strDir;
	string strFile;
	
	int pos = strName.rfind('/');
	if(pos == -1)
	{
		strDir="/";
		strFile = strName;
	}
	else
	{
		strDir.append(strName,0,pos);
		strFile.append(strName,pos+1,strName.length() - pos);
	}
			
	//try to list directory
	vector<string> kDirs;
	ListDir(kDirs,strDir);
		
	for(unsigned int i = 0;i<kDirs.size();i++)
	{
		if(IsSameIgnoreCase(kDirs[i].c_str(),strFile.c_str()))
		{
			return strDir+string("/")+kDirs[i];			
		}
	}
	
	return strName;
}

class I_NameGenerator
{
public:
	virtual string GetName() = 0;
};

void ZSSVFileSystem::RunCommand(int cmdid, const ConCommandLine* kCommand)
{
//	BasicConsole* m_pkConsole = static_cast<BasicConsole*>(GetSystem().GetObjectPtr("ZSSConsole"));		
	unsigned int i;
	vector<string> kFiles;
	string strRootMerge;

	switch(cmdid) {
		case FID_CD:
		{
			if(kCommand->m_kSplitCommand.size() <= 1) 
			{
				GetSystem().Printf(m_strCurentDir.c_str());
				return;
			}
			
			// special case when entering .. directory
			if(kCommand->m_kSplitCommand[1] == "..")
			{
				if(m_strCurentDir.length() == 0)
				{
					GetSystem().Printf("Already at the root");
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
				GetSystem().Printf(m_strCurentDir.c_str());
			}
			else
				GetSystem().Printf("%s does not exist",kCommand->m_kSplitCommand[1].c_str());

			break;
		}
			
		case FID_DIR:	
		{
			I_NameGenerator* pkGenerator = dynamic_cast<I_NameGenerator*>(GetSystem().GetObjectPtr("NameGenerator"));
			if(!pkGenerator)
			{
				GetSystem().Printf(" Namegenerator not found\n ");
			}
			else
			{
				GetSystem().Printf(" Namegenerator is active: %s\n ", pkGenerator->GetName().c_str());

			}

			break;

			bool bDirOnly = false;
		
			if(kCommand->m_kSplitCommand.size() == 2) 
				if(kCommand->m_kSplitCommand[1] == "-d")
					bDirOnly = true;				
	
			ListDir(kFiles,m_strCurentDir,bDirOnly);			
			
			GetSystem().Printf(" ");
			GetSystem().Printf("DIRECTORY %s",m_strCurentDir.c_str());
			GetSystem().Printf(" ");
			for(i=0;i<kFiles.size();i++)
			{
				GetSystem().Printf("<%s>", kFiles[i].c_str());
			}
			GetSystem().Printf(" ");
			GetSystem().Printf("%d files in directory",kFiles.size());
							
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
		
		case FID_ADDROOTPATH:
		{
			if(kCommand->m_kSplitCommand.size() != 3)
			{
				GetSystem().Printf("addrootpath [realdir] [vfsdir]");
				break;		
			}
			
			GetSystem().Printf("Adding root path: %s at %s",kCommand->m_kSplitCommand[1].c_str(),kCommand->m_kSplitCommand[2].c_str());
			AddRootPath(kCommand->m_kSplitCommand[1],kCommand->m_kSplitCommand[2]);
		
			break;
		}					
	};
}


