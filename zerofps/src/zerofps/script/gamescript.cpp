// gamescript.cpp: implementation of the GameScript class.
//
//////////////////////////////////////////////////////////////////////

#include "gamescript.h"

// package files
#include "../engine/engine.pkg"
#include "package_files/zfscript_bind.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GameScript::GameScript() : ZFScript()
{
	OpenPackageFiles();
}

GameScript::~GameScript()
{

}

void GameScript::OpenPackageFiles()
{
	tolua_zfscript_bind_open(GetLua());
}






