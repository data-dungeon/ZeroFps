#include <cstring>
#include <iostream>
#include "script.h"

using namespace std;


Script::Script()
{
	cBuffer = NULL;
}

Script::~Script()
{
	if(cBuffer)
		delete [] cBuffer;
}

int GetLengthOfFile (FILE* fp)
{
	int	iPosition = ftell (fp);
	fseek (fp, 0, SEEK_END);

	int iEnd = ftell (fp);
	fseek (fp, iPosition, SEEK_SET);
	return iEnd;
}


void Script::Load(char* ucFileName)
{
	FILE*	fp;
	int		iSize;

	fp = fopen(ucFileName,"rb");
	iSize = GetLengthOfFile(fp);
	cBuffer = new char [iSize + 1];
	cBuffer[iSize] = 0;
	cBufferEnd = cBuffer + iSize;
	fread(cBuffer,iSize,1,fp);
	fclose(fp);
}

 


void ScriptFile::LoadScript(char* ucpFileName)
{
	ActiveScript.Load(ucpFileName);
	bIsTokenReady = false;
	ucpScript		= ActiveScript.cBuffer;
	ucpScriptEnd	= ActiveScript.cBufferEnd;
	iLine = 1;
}

void ScriptFile::UnGetToken()
{
	bIsTokenReady = true;
}

bool ScriptFile::SkipWhiteSpaces(void)
{
	while(*ucpScript <= 32)
	{
		// If end of script
		if(ucpScript >= ucpScriptEnd)
			return false;
		if(*ucpScript == '\n')
			iLine++;
	
		ucpScript++;
	}

	return true;
}


char* ScriptFile::GetToken(void)
{
	char* cpToken;


	while(!bIsTokenReady)
	{
		// Skip spaces
		if(!SkipWhiteSpaces())
			return NULL;

		cpToken = ucToken;

		while(*ucpScript > 32)
		{
			*cpToken = *ucpScript;
			cpToken++;
			ucpScript++;
			if(ucpScript == ucpScriptEnd)
				break;
		}
		
		*cpToken = 0;
		
		//bIsTokenReady = true;
		return ucToken;

	}

	return NULL;
}


