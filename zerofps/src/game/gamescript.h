// gamescript.h: interface for the GameScript class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_GAMESCRIPT_H__F77A05D0_A671_42F8_80B0_D13C97C3E2D5__INCLUDED_)
//#define AFX_GAMESCRIPT_H__F77A05D0_A671_42F8_80B0_D13C97C3E2D5__INCLUDED_

#ifndef _GAMESCRIPT_H_
#define _GAMESCRIPT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../zerofps/script/script_x.h"
#include "../zerofps/script/zfscript.h"

class GameScript : public ZFScript
{
public:
	GameScript();
	virtual ~GameScript();

	void OpenPackageFiles();
	bool GetLogText(const char* szFile, const char* szKey, 
		char** pszText, char **pszIcon);

private:
	static int GetTextFromFile(lua_State* pkLua);

};

#endif // !defined(AFX_GAMESCRIPT_H__F77A05D0_A671_42F8_80B0_D13C97C3E2D5__INCLUDED_)
