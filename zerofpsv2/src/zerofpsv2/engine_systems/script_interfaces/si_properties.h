//  ______________________________________________________________________
// /																	  \
// | si_properties.h:                                                     |
// | A place to put common property functions in instead of rewritning    |
// | them for every new game.											  |
// \______________________________________________________________________/

#ifndef _SI_PROPERTIES_H_
#define _SI_PROPERTIES_H_

class ZSSScriptSystem;

#include "../engine_systems_x.h"
#include "../../script/zfscript.h"
#include <iostream>

using namespace std;

class ZSSVFileSystem;
class ZSSEntityManager;

namespace PropertiesLua
{

	extern ZSSScriptSystem* 		g_pkScript;
	extern ZSSEntityManager*		g_pkObjMan;

	void ENGINE_SYSTEMS_API Init(ZSSScriptSystem* pkScript, ZSSEntityManager* pkObjMan);
};

#endif


