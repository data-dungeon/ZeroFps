// zfscript.cpp: implementation of the ZFScript class.
//
//////////////////////////////////////////////////////////////////////

#include "zfscript.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZFScript::ZFScript()
{
	OpenLua();
}

ZFScript::~ZFScript()
{
	lua_close(m_pkLua);
}

bool ZFScript::OpenLua()
{
	m_pkLua = lua_open(0);
	return true;	
}

bool ZFScript::RunScript(char* szFileName)
{
	return (lua_dofile(m_pkLua, szFileName) == 0);
}
