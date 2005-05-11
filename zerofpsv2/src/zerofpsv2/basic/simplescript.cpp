#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "simplescript.h"

using namespace std;


SimpleScript::SimpleScript()
{
	cBuffer = NULL;
}

SimpleScript::~SimpleScript()
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


bool SimpleScript::Load(const char* ucFileName)
{
	FILE*	fp;
	int		iSize;

	fp = fopen(ucFileName,"rb");
	if(fp == NULL)
		return false;

	iSize = GetLengthOfFile(fp);
	cBuffer = new char [iSize + 1];
	cBuffer[iSize] = 0;
	cBufferEnd = cBuffer + iSize;
	fread(cBuffer,iSize,1,fp);
	fclose(fp);
	return true;

}

 


bool SimpleScriptFile::LoadScript(const char* ucpFileName)
{
	if(ActiveScript.Load(ucpFileName) == false)
		return false;

	
	bIsTokenReady = false;
	ucpScript		= ActiveScript.cBuffer;
	ucpScriptEnd	= ActiveScript.cBufferEnd;
	iLine = 1;
	return true;
}

void SimpleScriptFile::UnGetToken()
{
	bIsTokenReady = true;
}

bool SimpleScriptFile::SkipWhiteSpaces(void)
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

bool SimpleScriptFile::SkipLine()
{
	while(*ucpScript != '\n')	
	{
		ucpScript++;
		if(ucpScript >= ucpScriptEnd)
			return false;
	}

	ucpScript++;
	iLine++;
	if(ucpScript >= ucpScriptEnd)
		return false;

	return true;
}


char* SimpleScriptFile::GetToken(void)
{
	char* cpToken;


	while(!bIsTokenReady)
	{
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

char*	SimpleScriptFile::GetTokenUntilEOL(void)
{
	char* cpToken;

	while(!bIsTokenReady)
	{
		// Skip spaces
		// Skip spaces
		while(*ucpScript <= 32)
		{
			// If end of script
			if(ucpScript >= ucpScriptEnd)
				return false;
			if(*ucpScript == '\n')
				return NULL;
			ucpScript++;
		}


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
		return ucToken;

	}

	return NULL;
}




