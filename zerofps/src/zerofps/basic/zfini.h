// ZFIni.h: interface for the ZFIni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZFINI_H__3DD9DED9_3B35_455C_B1FB_B5D7BD87B819__INCLUDED_)
#define AFX_ZFINI_H__3DD9DED9_3B35_455C_B1FB_B5D7BD87B819__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_LINE_LENGTH 1024
#define MAX_SECTIONS 1024

#include <string>
#include <vector>
using namespace std;

#include "zfobject.h"
#include "basic_x.h"

class BASIC_API ZFIni : public ZFObject
{
public:
	enum FuncId_e
	{
		FID_EXECUTECOMMANDS, // Run Commands from Ini file
	};

	void RunCommand(int cmdid, const CmdArgument* kCommand);
	void GetCommandStrings(vector<string>* pkCommands);
	char* GetValue(const char* strSection, char* strKey);
	void GetSectionNames(vector<string>& kSectionList);
	bool Open(const char* strFileName, bool bCommandFile);
	bool ExecuteCommands(const char* strName);
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
};

#endif // !defined(AFX_ZFINI_H__3DD9DED9_3B35_455C_B1FB_B5D7BD87B819__INCLUDED_)



