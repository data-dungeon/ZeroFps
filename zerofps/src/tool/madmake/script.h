#ifndef FILE__SCRIPT_H
#define FILE__SCRIPT_H

#include <cstring>
#include <iostream>

using namespace std;

#define MAXTOKEN	512

class Script
{
public:
	Script();
	~Script();

	char	ucFileName[512];
	char*	cBuffer;
	char*	cBufferEnd;
	int		iLine;

	bool	Load(const char* ucFileName);
};

class ScriptFile
{
public:
	char	ucToken[MAXTOKEN];
	char*	ucScript;
	int		iLine;

	Script	ActiveScript;
	char*	ucpScript;
	char*	ucpScriptEnd;

	bool	LoadScript(const char* ucpFileName);
	
	char*	GetToken(void);
	void	UnGetToken();

	bool	bIsTokenReady;

	bool	SkipWhiteSpaces(void);

};

#endif