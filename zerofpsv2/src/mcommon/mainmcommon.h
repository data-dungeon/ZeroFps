#ifndef MCOMMON_MAIN
#define MCOMMON_MAIN

#include "mcommon_x.h"
#include "../zerofpsv2/engine/zerofps.h"

void MCOMMON_API MCommon_RegisterSystems();
void MCOMMON_API MCommon_RegisterResources();
void MCOMMON_API MCommon_RegisterPropertys(ZSSZeroFps* pkZeroFps, ZSSPropertyFactory*	pkPropertyFactory);


#endif
