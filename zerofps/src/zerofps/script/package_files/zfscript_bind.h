/*
** Lua binding: zfscript_bind
** Generated automatically by tolua 4.0 on 01/30/03 14:48:24.
*/

#include "tolua.h"

/* Exported function */
int tolua_zfscript_bind_open (lua_State* tolua_S);
void tolua_zfscript_bind_close (lua_State* tolua_S);

/*#include "os.h"
#include "zfobject.h"
#include "basic_x.h"
#include <iostream>
#include <vector>
#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "input.h"
#include "engine_x.h"
#include <iostream>
#include <vector>
#include <deque>*/

/* function to register type */
static void toluaI_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"CmdArgument");
 tolua_usertype(tolua_S,"BasicConsole");
 tolua_usertype(tolua_S,"Console");
}

/* method: new of class  BasicConsole */
static int toluaI_zfscript_bind_BasicConsole_new00(lua_State* tolua_S)
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
static int toluaI_zfscript_bind_BasicConsole_Print00(lua_State* tolua_S)
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
static int toluaI_zfscript_bind_BasicConsole_RunCommand00(lua_State* tolua_S)
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

/* method: new of class  Console */
static int toluaI_zfscript_bind_Console_new00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
 {
  Console* toluaI_ret = (Console*)  new Console();
 tolua_pushusertype(tolua_S,(void*)toluaI_ret,tolua_tag(tolua_S,"Console"));
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.");
 return 0;
}

/* method: Update of class  Console */
static int toluaI_zfscript_bind_Console_Update00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Console"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Console* self = (Console*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Update'");
 {
  self->Update();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Update'.");
 return 0;
}

/* method: Execute of class  Console */
static int toluaI_zfscript_bind_Console_Execute00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Console"),0) ||
 !tolua_istype(tolua_S,2,LUA_TSTRING,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  Console* self = (Console*)  tolua_getusertype(tolua_S,1,0);
  char* aText = ((char*)  tolua_getstring(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Execute'");
 {
  bool toluaI_ret = (bool)  self->Execute(aText);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Execute'.");
 return 0;
}

/* Open function */
int tolua_zfscript_bind_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 toluaI_reg_types(tolua_S);
 tolua_constant(tolua_S,NULL,"TEXT_MAX_LENGHT",TEXT_MAX_LENGHT);
 tolua_cclass(tolua_S,"BasicConsole","ZFObject");
 tolua_function(tolua_S,"BasicConsole","new",toluaI_zfscript_bind_BasicConsole_new00);
 tolua_function(tolua_S,"BasicConsole","Print",toluaI_zfscript_bind_BasicConsole_Print00);
 tolua_constant(tolua_S,"BasicConsole","FID_HELP",BasicConsole::FID_HELP);
 tolua_function(tolua_S,"BasicConsole","RunCommand",toluaI_zfscript_bind_BasicConsole_RunCommand00);
 tolua_constant(tolua_S,NULL,"COMMAND_LENGHT",COMMAND_LENGHT);
 tolua_cclass(tolua_S,"Console","BasicConsole");
 tolua_function(tolua_S,"Console","new",toluaI_zfscript_bind_Console_new00);
 tolua_function(tolua_S,"Console","Update",toluaI_zfscript_bind_Console_Update00);
 tolua_function(tolua_S,"Console","Execute",toluaI_zfscript_bind_Console_Execute00);
 return 1;
}
/* Close function */
void tolua_zfscript_bind_close (lua_State* tolua_S)
{
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"TEXT_MAX_LENGHT");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"BasicConsole");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"COMMAND_LENGHT");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"Console");
}
