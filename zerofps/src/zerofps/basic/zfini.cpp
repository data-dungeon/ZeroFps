// ZFIni.cpp: implementation of the ZFIni class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <malloc.h>
#include <memory.h>
#include "zfini.h"
#include "zfobjectmanger.h"
#include "zfbasicfs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZFIni::ZFIni() : ZFObject("ZFIni")
{
	m_iLines = 0;
	m_iNumSections = 0;
	m_bFileReady = false;
	m_pstrLines = NULL;
	m_pkSectionData = NULL;
	m_pstrSections = NULL;
	m_pstrLines = NULL;
	m_pkCommandData = NULL;	

	g_ZFObjSys.Register_Cmd("execommands",FID_EXECUTECOMMANDS,this);

	//m_pkBasicFS = static_cast<ZFBasicFS*>(g_ZFObjSys.GetObjectPtr("ZFBasicFS"));	
	
}

ZFIni::~ZFIni()
{
	int i, j;

	if(m_pstrLines)
	{
		for( i=0; i<m_iLines; i++)
			delete[] m_pstrLines[i];
	}

	if(m_pstrSections)
	{
		for( i=0; i<m_iNumSections; i++)
			delete[] m_pstrSections[i];
	}

	if(m_pkSectionData)
	{
		for( j=0; j<m_iNumSections; j++)
		{
			for( i=0; i<m_pkSectionData[j].iNumKeys; i++)
			{
				delete[] m_pkSectionData[j].strKeyName[i];
				delete[] m_pkSectionData[j].strKeyValue[i];
			}
		}

		delete[] m_pkSectionData;
	}

	if(m_pkCommandData)
	{
		for( i=0; i<m_pkCommandData->iNumCommands; i++)
		{
			delete[] m_pkCommandData->strCommand[i];
		}

		delete[] m_pkCommandData;
	}
}

bool ZFIni::Open(const char *strFileName, bool bCommandFile)
{
	m_bCommandFile = bCommandFile;

	FILE* pkFile;
	if ((pkFile = fopen(strFileName, "rb")) == NULL)
	{
		printf("Failed to load ini file %s", strFileName);
		return (m_bFileReady = false);
	}

	fseek(pkFile, 0, SEEK_SET);

	m_iLines = 0;
	char strLine[MAX_LINE_LENGTH];

	// Kolla hur många rader som finns i filen.
	while (!feof(pkFile))
	{
		fgets(strLine, MAX_LINE_LENGTH, pkFile);
		m_iLines++;
	}

	// Kopiera datat till en buffert.
	m_pstrLines = new char*[m_iLines+1];
	int iLineLength, i;
	fseek(pkFile, 0, SEEK_SET);
	for ( i=0; i<m_iLines; i++)
	{
		memset(strLine, 0, MAX_LINE_LENGTH);
		fgets(strLine, MAX_LINE_LENGTH, pkFile);
		iLineLength = strlen(strLine)+1;
		m_pstrLines[i] = new char[iLineLength+1];
		memset(m_pstrLines[i], 0, iLineLength);
		strcpy(m_pstrLines[i], strLine);
	}

	if(m_bCommandFile == true)
	{
		if(ProcessCommandFile() == false)
			return false;
	}
	else
	{
		if(ProcessINIFile() == false)
			return false;
	}

	return (m_bFileReady = true);
}

int ZFIni::GetNumKeysInSection(int iSection, int iStartRow, int iEndRow)
{
	int iKeys=0, start=0, end=0;

	for(int i=iStartRow; i<iEndRow; i++)
	{
		if( GetKeyNamePos(i) == -1 || GetKeyValuePos(i) == -1)
			continue;

		iKeys++;
	}

	return iKeys;
}

int ZFIni::GetKeyNamePos(int row)
{
	int length = strlen(m_pstrLines[row]);

	for(int i=0; i<length; i++)
	{
		if( m_pstrLines[row][i] == '='  || m_pstrLines[row][i] == ';' || 
			m_pstrLines[row][i] == '\r' || m_pstrLines[row][i] == '\n')
			return -1;
		else
		if( m_pstrLines[row][i] == ' '  || m_pstrLines[row][i] == '\t')
			continue;
		else
			return i;
	}

	return -1;
}

int ZFIni::GetKeyValuePos(int row)
{
	int length = strlen(m_pstrLines[row]);
	bool bFoundValue = false;

	for(int i=0; i<length; i++)
	{
		if( m_pstrLines[row][i] == '=')
		{
			bFoundValue = true;
			continue;
		}
		else
		if(bFoundValue && (m_pstrLines[row][i] != ' ' 
			&& m_pstrLines[row][i] != '\t'))
			return i;
	}

	return -1;
}

char* ZFIni::GetValue(char *strSection, char *strKey)
{
	if(m_bFileReady == false)
		return NULL;

	for(int sec=0; sec<m_iNumSections; sec++)
		if(strstr(m_pstrSections[sec], strSection) != NULL)
			for(int key=0; key<m_pkSectionData[sec].iNumKeys; key++)
				if(strstr(m_pkSectionData[sec].strKeyName[key], strKey) != NULL)
					return m_pkSectionData[sec].strKeyValue[key];

	return NULL;
}

bool ZFIni::ProcessINIFile()
{
	m_iNumSections=0;

	// Kolla hur många sektioner som finns i filen.
	// och kopiera dess position sammt dess namn.
	int iCurrSection=0, iLineLength=0;
	for(int state=0; state<2; state++)
	{
		for (int line=0; line<m_iLines; line++)
		{
			bool b1stBracketFound=false, 
				 b2ndBracketFound=false;

			int ch=0, b1=0, b2=0;
			iLineLength = strlen(m_pstrLines[line]);
			do
			{
				if(m_pstrLines[line][ch] == '[' && b2ndBracketFound == false)
				{
					b1=ch+1; b1stBracketFound = true;
				}
				if(m_pstrLines[line][ch] == ']' && b1stBracketFound == true)
				{
					b2=ch; b2ndBracketFound = true;
				}

			} while( ch++<iLineLength && !(b1stBracketFound && b2ndBracketFound));

			if(b1stBracketFound && b2ndBracketFound)
			{
				if(state==0)
				{
					m_aiSectionLineOffsets[m_iNumSections] = line+1;
					m_iNumSections++;
				}
				else
				{
					int length = b2-b1;
					if(length < 0)
						length = 0;
					m_pstrSections[iCurrSection] = new char[length+1];
					memset(m_pstrSections[iCurrSection], 0, length);
					strncpy(m_pstrSections[iCurrSection++], m_pstrLines[line]+b1, b2-b1);
				}
			}
		}

		if(state==0)
		{
			if(m_iNumSections>0)
				m_pstrSections = new char*[m_iNumSections+1];
			else
			{
				printf("No sections in file!\n");
				return (m_bFileReady = false);
			}
		}
	}

	// Kopiera in datat från texten till en strukur.
	m_pkSectionData = new SectionData[m_iNumSections+1];

	for(int sec=0; sec<m_iNumSections; sec++)
	{
		int iLineStart = m_aiSectionLineOffsets[sec];
		int iLineEnd = m_iLines-1;
		if(sec < m_iNumSections-1)
			iLineEnd = m_aiSectionLineOffsets[sec+1];

		m_pkSectionData[sec].iNumKeys = GetNumKeysInSection(sec, iLineStart, iLineEnd);
		m_pkSectionData[sec].strKeyName  = new char*[m_pkSectionData[sec].iNumKeys+1];
		m_pkSectionData[sec].strKeyValue = new char*[m_pkSectionData[sec].iNumKeys+1];

		for(int iLine=iLineStart, key=0; iLine<iLineEnd; iLine++, key++)
		{
			int iNameStartPos = GetKeyNamePos(iLine); 
			int iValueStartPos = GetKeyValuePos(iLine);
			int iValueEndPos = -1, iNameEndPos = -1;
			unsigned int ch, line_length = strlen(m_pstrLines[iLine]);
			
			if(iNameStartPos == -1 || iValueStartPos == -1)
			{
				key--;
				continue;
			}

			for( ch = iNameStartPos; ch<line_length; ch++)
				if(m_pstrLines[iLine][ch] == '=')
				{
					iNameEndPos = ch;
					break;
				}

			for( ch = iValueStartPos; ch<line_length; ch++)
				if( m_pstrLines[iLine][ch] == ';'  || m_pstrLines[iLine][ch] == '\r' ||
					m_pstrLines[iLine][ch] == '\n' || m_pstrLines[iLine][ch] == '\t')
				{
					iValueEndPos = ch;
					break;
				}

			// Skall alldrig hända...
			if(iValueEndPos == -1 || iNameEndPos == -1)
			{
				key--;
				continue;
			}

			// Allokera minne för keynames och key values
			unsigned int name_length = iNameEndPos-iNameStartPos;
			unsigned int value_length = iValueEndPos-iValueStartPos;

			m_pkSectionData[sec].strKeyName[key]  = new char[name_length+1];
			m_pkSectionData[sec].strKeyValue[key]  = new char[value_length+1];

			// Kopiera datat till strukturen
			memset(m_pkSectionData[sec].strKeyName[key], 0, name_length);
			memset(m_pkSectionData[sec].strKeyValue[key], 0, value_length);

			char* dest;
			dest = m_pstrLines[iLine] + iNameStartPos;
			strncpy(m_pkSectionData[sec].strKeyName[key], dest, name_length);
			if(name_length < strlen(dest))
				m_pkSectionData[sec].strKeyName[key][name_length] = 0;

			dest = m_pstrLines[iLine] + iValueStartPos;
			strncpy(m_pkSectionData[sec].strKeyValue[key], dest, value_length);
			if(value_length < strlen(dest))
				m_pkSectionData[sec].strKeyValue[key][value_length] = 0;
		}
	}

	return true;
}

bool ZFIni::ProcessCommandFile()
{
	int iCommands = 0;

	for( int line=0; line<m_iLines; line++)
	{
		if(GetKeyNamePos(line) != -1)
			iCommands++;
	}

	if(iCommands == 0)
	{
		printf("No commands found!\n");
		return false;
	}

	int iCmdCounter=0;
	m_pkCommandData = new CommandData;
	m_pkCommandData->iNumCommands = iCommands;
	m_pkCommandData->strCommand = new char*[iCommands];

	for( int iLine=0; iLine<m_iLines; iLine++)
	{
		int start_pos = -1, end_pos = -1;
		int line_length = strlen(m_pstrLines[iLine]);

		if((start_pos = GetKeyNamePos(iLine)) != -1)
		{
			for( int ch = start_pos; ch<line_length+1; ch++)
				 if( m_pstrLines[iLine][ch] == ';'  || m_pstrLines[iLine][ch] == '\r' ||
					 m_pstrLines[iLine][ch] == '\n' || m_pstrLines[iLine][ch] == '\t' ||
					 m_pstrLines[iLine][ch] == '\0')
				{
					end_pos = ch;
					break;
				}

			if(end_pos != -1)
			{
				unsigned int length = end_pos - start_pos;
				m_pkCommandData->strCommand[iCmdCounter] = new char[length+1];
				memset(m_pkCommandData->strCommand[iCmdCounter], 0, length);
				
				char* dest;
				dest = m_pstrLines[iLine] + start_pos;
				strncpy(m_pkCommandData->strCommand[iCmdCounter], dest, length);

				if(length < strlen(dest))
					m_pkCommandData->strCommand[iCmdCounter][length] = 0;

				iCmdCounter++;
			}
		}	
	}

	return true;
}

void ZFIni::GetCommandStrings(vector<string>* pkCommands)
{
	for(int i=0; i<m_pkCommandData->iNumCommands; i++)
		pkCommands->push_back( string(m_pkCommandData->strCommand[i]) );
}

void ZFIni::RunCommand(int cmdid, const CmdArgument* kCommand)
{
	switch(cmdid) 
	{
		case FID_EXECUTECOMMANDS:
			if(Open(kCommand->m_kSplitCommand[1].c_str(), true))
			{
				if(m_pkCommandData)
				{
					for(int i=0; i<m_pkCommandData->iNumCommands; i++)
						g_ZFObjSys.RunCommand(m_pkCommandData->strCommand[i]);
				}
			}
			break;
	}
}
