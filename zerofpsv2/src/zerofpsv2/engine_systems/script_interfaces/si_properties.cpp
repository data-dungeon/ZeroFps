// si_properties.cpp: Scriptfunctions of common used properties here. //
// Common property = one that is not unique for a single game.		  //
////////////////////////////////////////////////////////////////////////
#include "si_properties.h"
#include "../../basic/zfvfs.h"
#include "../../engine/entitymanager.h"
#include "../../engine_systems/propertys/p_controller.h"
#include "../../engine_systems/propertys/p_mad.h"

ZFScriptSystem* PropertiesLua::g_pkScript;
EntityManager*  PropertiesLua::g_pkObjMan;


// ------------------------------------------------------------------------------------------


void PropertiesLua::Init(ZFScriptSystem* pkScript, EntityManager* pkObjMan)
{
	cout << "Add SI: Properties" << endl;

	g_pkScript = NULL;
	g_pkObjMan = NULL;

//	g_pkScript = pkScript;
//	g_pkObjMan = pkObjMan;

//	pkScript->ExposeFunction("RotWithCamYAxis",		PropertiesLua::RotWithCamYAxisLua);
//	pkScript->ExposeFunction("BindKey",				PropertiesLua::BindKeyLua);
	pkScript->ExposeFunction("SetMadfile",			PropertiesLua::SetMadfileLua);
}

// ------------------------------------------------------------------------------------------

/*
// p_controller: objectID(0), NameOfKey(string)(1), axis:X,Y,Z(2), force(float)(3), input (optional, default=KEYBOARD)(4)
int BindKeyLua(lua_State* pkLua)
{
	char strKeyName[255];
	double dObjectID, dAxis, dForce;

	if( g_pkScript->GetNumArgs(pkLua) == 4 )
	{
		g_pkScript->GetArgNumber(pkLua, 0, &dObjectID);
		g_pkScript->GetArgString(pkLua, 1, strKeyName);
		g_pkScript->GetArgNumber(pkLua, 2, &dAxis);
		g_pkScript->GetArgNumber(pkLua, 3, &dForce);

		Entity* pkEntity = g_pkObjMan->GetEntityByID( int(dObjectID) );
		if ( !pkEntity )
			return 0;

		if ( P_Controller* pkP_Controller = (P_Controller*)pkEntity->GetProperty("P_Controller") )
			pkP_Controller->AddTrigger (string(strKeyName), int(dAxis), dForce);
	}

	return 0;
}
// ------------------------------------------------------------------------------------------

// p_controller: makes the steering forces rotate with camera y-axis. Takes ObjectID(0), bool(1)
int RotWithCamYAxisLua(lua_State* pkLua)
{
	if ( g_pkScript->GetNumArgs(pkLua) != 2 )
	{
		cout << "Warning! RotWithCamYAxisLua: takes two arguments: objectId, bool" << endl;
		return 0;
	}
	
	double dObjectID, dBool;


	g_pkScript->GetArgNumber(pkLua, 0, &dObjectID);
	g_pkScript->GetArgNumber(pkLua, 1, &dBool);

	Entity* pkEntity = g_pkObjMan->GetEntityByID( int(dObjectID) );

	if ( !pkEntity )
		return 0;

	if ( P_Controller* pkP_Controller = (P_Controller*)pkEntity->GetProperty("P_Controller") ) 
		pkP_Controller->SetCameraRotation(dBool);

	return 0;
}
*/

// ------------------------------------------------------------------------------------------

// p_mad: changes MadFile: Takes ObjectID, filename
int PropertiesLua::SetMadfileLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		double dId;		
		g_pkScript->GetArgNumber(pkLua, 0, &dId);				
		
		if (Entity* pkEnt = g_pkObjMan->GetEntityByID((int)dId))
		{
			if ( P_Mad* pkMad = ((P_Mad*)pkEnt->GetProperty("P_Mad")) )
			{
				char acModel[100];
				g_pkScript->GetArg(pkLua, 1, acModel);
				pkMad->SetBase(acModel);
			}
		}
		
	}
	return 0;
}




