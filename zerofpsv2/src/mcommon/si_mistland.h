
#ifndef _SI_MISTLAND_H_
#define _SI_MISTLAND_H_

class ZFScriptSystem;

#include "mcommon_x.h"
#include "../zerofpsv2/script/zfscript.h"
#include <iostream>

using namespace std;

namespace MistLandLua
{

	extern ZFScriptSystem* 		g_pkScript;
	
	void MCOMMON_API Init(ZFScriptSystem* pkScript);
	
};

#endif


