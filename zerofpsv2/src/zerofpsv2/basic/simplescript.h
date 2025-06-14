#ifndef _BASIC_SCRIPT_H_
#define _BASIC_SCRIPT_H_

#include <cstring>
#include <iostream>
#include "basic_x.h"

using namespace std;

#define MAXTOKEN	512

/// A Textfile we read tokens from.
class BASIC_API SimpleScript
{
public:
	SimpleScript();
	~SimpleScript();

	char	ucFileName[512];
	char*	cBuffer;
	char*	cBufferEnd;
	int		iLine;

	bool	Load(const char* ucFileName);
};

/// Read Simple TextFiles and get tokens.
class BASIC_API SimpleScriptFile
{
public:
	char	ucToken[MAXTOKEN];
	char*	ucScript;
	int		iLine;

	SimpleScript	ActiveScript;
	char*	ucpScript;
	char*	ucpScriptEnd;

	bool	LoadScript(const char* ucpFileName);
	
	char*	GetToken(void);
	void	UnGetToken();

	bool	bIsTokenReady;

	bool	SkipWhiteSpaces(void);
	bool  SkipLine();

	char*	GetTokenUntilEOL(void);

};

#endif









