// ZFIni.h: interface for the ZFIni class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _BASIC_ZFINI_H_
#define _BASIC_ZFINI_H_

#define MAX_LINE_LENGTH 1024
#define MAX_SECTIONS 1024

#include <string>
#include <vector>
using namespace std;

#include "zfsubsystem.h"
#include "basic_x.h"

/// Reads text based INI files (Basic Config Sys of ZeroFps).
class BASIC_API ZFIni /*: public ZFSubSystem*/
{
public:
	void Close();
	enum FuncId_e
	{
		FID_EXECUTECOMMANDS, // Run Commands from Ini file
	};

//	void RunCommand(int cmdid, const CmdArgument* kCommand);
	void GetCommandStrings(vector<string>* pkCommands);
	bool ExecuteCommands(const char* strName);
	
	char* GetValue(const char* strSection, const char* strKey);
	void GetSectionNames(vector<string>& kSectionList);
	bool GetBoolValue(const char* strSection, char* strKey);
	float GetFloatValue(const char* strSection, char* strKey);
	int GetIntValue(const char* strSection, char* strKey);
	
	bool SectionExist(const char* acSection);
	bool KeyExist(const char* strSection, char* strKey);
	
	bool Open(const char* strFileName, bool bCommandFile);
	
	ZFIni();
	~ZFIni();

private:
	bool m_bCommandFile;
	bool ProcessCommandFile();
	bool ProcessINIFile();
	int GetKeyNamePos(int row);
	int GetKeyValuePos(int row);
	int GetNumKeysInSection(int iSection, int iStartRow, int iEndRow);

	struct SectionData
	{
		int iNumKeys;
		char** strKeyName;
		char** strKeyValue;
	};

	struct CommandData
	{
		int iNumCommands;
		char** strCommand;
	};

	int m_iLines;
	int m_iNumSections;
	int m_aiSectionLineOffsets[MAX_SECTIONS];
	bool m_bFileReady;
	char** m_pstrLines;
	char** m_pstrSections;
	SectionData* m_pkSectionData;
	CommandData* m_pkCommandData;

/*		bool StartUp();
		bool ShutDown();
		bool IsValid();*/

};

#endif 


