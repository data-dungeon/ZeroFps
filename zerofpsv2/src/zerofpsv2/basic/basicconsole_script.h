/*
** Lua binding: basicconsole
** Generated automatically by tolua 4.0 on 01/27/03 16:40:53.
*/

#include "tolua.h"

/* Exported function */
int tolua_basicconsole_open (lua_State* tolua_S);
void tolua_basicconsole_close (lua_State* tolua_S);

//#include "os.h"
#include "zfobject.h"
#include "basic_x.h"
#include <iostream>
#include <vector>

/* function to register type */
static void toluaI_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"CmdArgument");
 tolua_usertype(tolua_S,"BasicConsole");
}

/* method: new of class  BasicConsole */
static int toluaI_basicconsole_BasicConsole_new00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,2,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  char* szName = ((char*)  tolua_getstring(tolua_S,2,0));
 {
  BasicConsole* toluaI_ret = (BasicConsole*)  new BasicConsole(szName);
 tolua_pushusertype(tolua_S,(void*)toluaI_ret,tolua_tag(tolua_S,"BasicConsole"));
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.");
 return 0;
}

/* method: Print of class  BasicConsole */
static int toluaI_basicconsole_BasicConsole_Print00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"BasicConsole"),0) ||
 !tolua_istype(tolua_S,2,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  BasicConsole* self = (BasicConsole*)  tolua_getusertype(tolua_S,1,0);
  const char* aText = ((const char*)  tolua_getstring(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Print'");
 {
  self->Print(aText);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Print'.");
 return 0;
}

/* method: RunCommand of class  BasicConsole */
static int toluaI_basicconsole_BasicConsole_RunCommand00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"BasicConsole"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,tolua_tag(tolua_S,"const CmdArgument"),0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  BasicConsole* self = (BasicConsole*)  tolua_getusertype(tolua_S,1,0);
  int cmdid = ((int)  tolua_getnumber(tolua_S,2,0));
  const CmdArgument* kCommand = ((const CmdArgument*)  tolua_getusertype(tolua_S,3,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RunCommand'");
 {
  self->RunCommand(cmdid,kCommand);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RunCommand'.");
 return 0;
}

/* Open function */
int tolua_basicconsole_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 toluaI_reg_types(tolua_S);
 tolua_constant(tolua_S,NULL,"TEXT_MAX_LENGHT",TEXT_MAX_LENGHT);
 tolua_cclass(tolua_S,"BasicConsole","ZFObject");
 tolua_function(tolua_S,"BasicConsole","new",toluaI_basicconsole_BasicConsole_new00);
 tolua_function(tolua_S,"BasicConsole","Print",toluaI_basicconsole_BasicConsole_Print00);
 tolua_constant(tolua_S,"BasicConsole","FID_HELP",BasicConsole::FID_HELP);
 tolua_function(tolua_S,"BasicConsole","RunCommand",toluaI_basicconsole_BasicConsole_RunCommand00);
 return 1;
}
/* Close function */
void tolua_basicconsole_close (lua_State* tolua_S)
{
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"TEXT_MAX_LENGHT");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"BasicConsole");
}
