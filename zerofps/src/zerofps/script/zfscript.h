// zfscript.h: interface for the ZFScript class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZFSCRIPT_H__8BAF0143_B0D3_476F_8410_552548EB16B2__INCLUDED_)
#define AFX_ZFSCRIPT_H__8BAF0143_B0D3_476F_8410_552548EB16B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "script_x.h"

extern "C"  {
	#include "lua.h"
}

class SCRIPT_API ZFScript  
{
public:
	ZFScript();
	virtual ~ZFScript();

	bool RunScript(char* szFileName);

private:
	bool OpenLua();
	lua_State* m_pkLua;
};

#endif // !defined(AFX_ZFSCRIPT_H__8BAF0143_B0D3_476F_8410_552548EB16B2__INCLUDED_)
