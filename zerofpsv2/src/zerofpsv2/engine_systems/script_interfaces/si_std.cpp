#include "si_std.h"
#include <cmath>                    // for trigonometry functions

ZFScriptSystem* StdLua::g_pkScript;

void StdLua::Init(ZFScriptSystem* pkScript)
{
	g_pkScript = pkScript;
	
	pkScript->ExposeFunction("Print",	StdLua::PrintLua);
	pkScript->ExposeFunction("Sin",	StdLua::SinLua);	
	pkScript->ExposeFunction("Cos",	StdLua::CosLua);		
	pkScript->ExposeFunction("Tan",	StdLua::TanLua);			
}


int StdLua::PrintLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		char acText[256];
		g_pkScript->GetArg(pkLua, 0, acText);
	
		cout<<acText<<endl;
	}

	return 1;
}

int StdLua::SinLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 0)
		return 0;
		
		
	double dTal;
	g_pkScript->GetArgNumber(pkLua, 0, &dTal);
	
	g_pkScript->AddReturnValue(pkLua,sin(dTal));
	
	return 1;
}

int StdLua::CosLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 0)
		return 0;
		
		
	double dTal;
	g_pkScript->GetArgNumber(pkLua, 0, &dTal);
	
	g_pkScript->AddReturnValue(pkLua,cos(dTal));
	
	return 1;
}

int StdLua::TanLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 0)
		return 0;
		
		
	double dTal;
	g_pkScript->GetArgNumber(pkLua, 0, &dTal);
	
	g_pkScript->AddReturnValue(pkLua,tan(dTal));
	
	return 1;
}



