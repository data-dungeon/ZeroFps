/*
** Lua binding: zfscript_bind
** Generated automatically by tolua 4.0 on 02/04/03 16:27:54.
*/

#include "tolua.h"

/* Exported function */
int tolua_zfscript_bind_open (lua_State* tolua_S);
void tolua_zfscript_bind_close (lua_State* tolua_S);


/* function to register type */
static void toluaI_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"OBJECT_TYPE");
 tolua_usertype(tolua_S,"Vector3");
 tolua_usertype(tolua_S,"CmdArgument");
 tolua_usertype(tolua_S,"GameScriptInterface");
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

/* get function: ZERO of class  Vector3 */
static int toluaI_get_zfscript_bind_Vector3_ZERO(lua_State* tolua_S)
{
 tolua_pushusertype(tolua_S,(void*)&Vector3::ZERO,tolua_tag(tolua_S,"const Vector3"));
 return 1;
}

/* get function: AXIS_X of class  Vector3 */
static int toluaI_get_zfscript_bind_Vector3_AXIS_X(lua_State* tolua_S)
{
 tolua_pushusertype(tolua_S,(void*)&Vector3::AXIS_X,tolua_tag(tolua_S,"const Vector3"));
 return 1;
}

/* get function: AXIS_Y of class  Vector3 */
static int toluaI_get_zfscript_bind_Vector3_AXIS_Y(lua_State* tolua_S)
{
 tolua_pushusertype(tolua_S,(void*)&Vector3::AXIS_Y,tolua_tag(tolua_S,"const Vector3"));
 return 1;
}

/* get function: AXIS_Z of class  Vector3 */
static int toluaI_get_zfscript_bind_Vector3_AXIS_Z(lua_State* tolua_S)
{
 tolua_pushusertype(tolua_S,(void*)&Vector3::AXIS_Z,tolua_tag(tolua_S,"const Vector3"));
 return 1;
}

/* get function: degtorad of class  Vector3 */
static int toluaI_get_zfscript_bind_Vector3_degtorad(lua_State* tolua_S)
{
 tolua_pushnumber(tolua_S,(double)Vector3::degtorad);
 return 1;
}

/* get function: x of class  Vector3 */
static int toluaI_get_zfscript_bind_Vector3_x(lua_State* tolua_S)
{
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'");
 tolua_pushnumber(tolua_S,(double)self->x);
 return 1;
}

/* set function: x of class  Vector3 */
static int toluaI_set_zfscript_bind_Vector3_x(lua_State* tolua_S)
{
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'");
 if (!tolua_istype(tolua_S,2,LUA_TNUMBER,0))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.");
  self->x = ((float)  tolua_getnumber(tolua_S,2,0));
 return 0;
}

/* get function: y of class  Vector3 */
static int toluaI_get_zfscript_bind_Vector3_y(lua_State* tolua_S)
{
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'");
 tolua_pushnumber(tolua_S,(double)self->y);
 return 1;
}

/* set function: y of class  Vector3 */
static int toluaI_set_zfscript_bind_Vector3_y(lua_State* tolua_S)
{
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'");
 if (!tolua_istype(tolua_S,2,LUA_TNUMBER,0))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.");
  self->y = ((float)  tolua_getnumber(tolua_S,2,0));
 return 0;
}

/* get function: z of class  Vector3 */
static int toluaI_get_zfscript_bind_Vector3_z(lua_State* tolua_S)
{
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'z'");
 tolua_pushnumber(tolua_S,(double)self->z);
 return 1;
}

/* set function: z of class  Vector3 */
static int toluaI_set_zfscript_bind_Vector3_z(lua_State* tolua_S)
{
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'z'");
 if (!tolua_istype(tolua_S,2,LUA_TNUMBER,0))
 tolua_error(tolua_S,"#vinvalid type in variable assignment.");
  self->z = ((float)  tolua_getnumber(tolua_S,2,0));
 return 0;
}

/* method: new of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_new00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  float fX = ((float)  tolua_getnumber(tolua_S,2,0));
  float fY = ((float)  tolua_getnumber(tolua_S,3,0));
  float fZ = ((float)  tolua_getnumber(tolua_S,4,0));
 {
  Vector3* toluaI_ret = (Vector3*)  new Vector3(fX,fY,fZ);
 tolua_pushusertype(tolua_S,(void*)toluaI_ret,tolua_tag(tolua_S,"Vector3"));
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.");
 return 0;
}

/* method: new of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_new01(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
 {
  Vector3* toluaI_ret = (Vector3*)  new Vector3();
 tolua_pushusertype(tolua_S,(void*)toluaI_ret,tolua_tag(tolua_S,"Vector3"));
 }
 }
 return 1;
tolua_lerror:
 return toluaI_zfscript_bind_Vector3_new00(tolua_S);
}

/* method: Length of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Length00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Length'");
 {
  float toluaI_ret = (float)  self->Length();
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Length'.");
 return 0;
}

/* method: LengthSqr of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_LengthSqr00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LengthSqr'");
 {
  float toluaI_ret = (float)  self->LengthSqr();
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LengthSqr'.");
 return 0;
}

/* method: SquaredLength of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_SquaredLength00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SquaredLength'");
 {
  float toluaI_ret = (float)  self->SquaredLength();
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SquaredLength'.");
 return 0;
}

/* method: Normalize of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Normalize00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Normalize'");
 {
  self->Normalize();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Normalize'.");
 return 0;
}

/* method: Dot of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Dot00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"const Vector3"),0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"const Vector3"),0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  const Vector3* self = (const Vector3*)  tolua_getusertype(tolua_S,1,0);
  const Vector3* v = ((const Vector3*)  tolua_getusertype(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Dot'");
 {
  float toluaI_ret = (float)  self->Dot(*v);
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Dot'.");
 return 0;
}

/* method: Unit of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Unit00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"const Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  const Vector3* self = (const Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Unit'");
 {
  Vector3 toluaI_ret = (Vector3)  self->Unit();
 {
#ifdef __cplusplus
 void* toluaI_clone = new Vector3(toluaI_ret);
#else
 void* toluaI_clone = tolua_copy(tolua_S,(void*)&toluaI_ret,sizeof(Vector3));
#endif
 tolua_pushusertype(tolua_S,tolua_doclone(tolua_S,toluaI_clone,tolua_tag(tolua_S,"Vector3")),tolua_tag(tolua_S,"Vector3"));
 }
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Unit'.");
 return 0;
}

/* method: Cross of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Cross00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"const Vector3"),0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"const Vector3"),0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  const Vector3* self = (const Vector3*)  tolua_getusertype(tolua_S,1,0);
  const Vector3* v = ((const Vector3*)  tolua_getusertype(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Cross'");
 {
  Vector3 toluaI_ret = (Vector3)  self->Cross(*v);
 {
#ifdef __cplusplus
 void* toluaI_clone = new Vector3(toluaI_ret);
#else
 void* toluaI_clone = tolua_copy(tolua_S,(void*)&toluaI_ret,sizeof(Vector3));
#endif
 tolua_pushusertype(tolua_S,tolua_doclone(tolua_S,toluaI_clone,tolua_tag(tolua_S,"Vector3")),tolua_tag(tolua_S,"Vector3"));
 }
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Cross'.");
 return 0;
}

/* method: IsZero of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_IsZero00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"const Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  const Vector3* self = (const Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsZero'");
 {
  bool toluaI_ret = (bool)  self->IsZero();
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsZero'.");
 return 0;
}

/* method: NearlyEquals of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_NearlyEquals00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"const Vector3"),0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"const Vector3"),0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  const Vector3* self = (const Vector3*)  tolua_getusertype(tolua_S,1,0);
  const Vector3* v = ((const Vector3*)  tolua_getusertype(tolua_S,2,0));
  const float e = ((const float)  tolua_getnumber(tolua_S,3,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NearlyEquals'");
 {
  bool toluaI_ret = (bool)  self->NearlyEquals(*v,e);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NearlyEquals'.");
 return 0;
}

/* method: NearlyZero of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_NearlyZero00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"const Vector3"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  const Vector3* self = (const Vector3*)  tolua_getusertype(tolua_S,1,0);
  const float e = ((const float)  tolua_getnumber(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NearlyZero'");
 {
  bool toluaI_ret = (bool)  self->NearlyZero(e);
 tolua_pushbool(tolua_S,(int)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NearlyZero'.");
 return 0;
}

/* method: Proj of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Proj00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
  Vector3* v = ((Vector3*)  tolua_getusertype(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Proj'");
 {
  Vector3 toluaI_ret = (Vector3)  self->Proj(*v);
 {
#ifdef __cplusplus
 void* toluaI_clone = new Vector3(toluaI_ret);
#else
 void* toluaI_clone = tolua_copy(tolua_S,(void*)&toluaI_ret,sizeof(Vector3));
#endif
 tolua_pushusertype(tolua_S,tolua_doclone(tolua_S,toluaI_clone,tolua_tag(tolua_S,"Vector3")),tolua_tag(tolua_S,"Vector3"));
 }
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Proj'.");
 return 0;
}

/* method: Perp of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Perp00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
  Vector3* v = ((Vector3*)  tolua_getusertype(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Perp'");
 {
  Vector3 toluaI_ret = (Vector3)  self->Perp(*v);
 {
#ifdef __cplusplus
 void* toluaI_clone = new Vector3(toluaI_ret);
#else
 void* toluaI_clone = tolua_copy(tolua_S,(void*)&toluaI_ret,sizeof(Vector3));
#endif
 tolua_pushusertype(tolua_S,tolua_doclone(tolua_S,toluaI_clone,tolua_tag(tolua_S,"Vector3")),tolua_tag(tolua_S,"Vector3"));
 }
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Perp'.");
 return 0;
}

/* method: Angle of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Angle00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
  Vector3* v = ((Vector3*)  tolua_getusertype(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Angle'");
 {
  float toluaI_ret = (float)  self->Angle(*v);
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Angle'.");
 return 0;
}

/* method: Abs of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Abs00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Abs'");
 {
  self->Abs();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Abs'.");
 return 0;
}

/* method: AbsMaxCoo of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_AbsMaxCoo00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AbsMaxCoo'");
 {
  int toluaI_ret = (int)  self->AbsMaxCoo();
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AbsMaxCoo'.");
 return 0;
}

/* method: Lerp of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Lerp00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_istype(tolua_S,2,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_istype(tolua_S,3,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_istype(tolua_S,4,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,5)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
  Vector3* from = ((Vector3*)  tolua_getusertype(tolua_S,2,0));
  Vector3* to = ((Vector3*)  tolua_getusertype(tolua_S,3,0));
  float flerp = ((float)  tolua_getnumber(tolua_S,4,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Lerp'");
 {
  self->Lerp(*from,*to,flerp);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Lerp'.");
 return 0;
}

/* method: Angels of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Angels00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Angels'");
 {
  Vector3 toluaI_ret = (Vector3)  self->Angels();
 {
#ifdef __cplusplus
 void* toluaI_clone = new Vector3(toluaI_ret);
#else
 void* toluaI_clone = tolua_copy(tolua_S,(void*)&toluaI_ret,sizeof(Vector3));
#endif
 tolua_pushusertype(tolua_S,tolua_doclone(tolua_S,toluaI_clone,tolua_tag(tolua_S,"Vector3")),tolua_tag(tolua_S,"Vector3"));
 }
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Angels'.");
 return 0;
}

/* method: AToU of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_AToU00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AToU'");
 {
  Vector3 toluaI_ret = (Vector3)  self->AToU();
 {
#ifdef __cplusplus
 void* toluaI_clone = new Vector3(toluaI_ret);
#else
 void* toluaI_clone = tolua_copy(tolua_S,(void*)&toluaI_ret,sizeof(Vector3));
#endif
 tolua_pushusertype(tolua_S,tolua_doclone(tolua_S,toluaI_clone,tolua_tag(tolua_S,"Vector3")),tolua_tag(tolua_S,"Vector3"));
 }
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AToU'.");
 return 0;
}

/* method: Print of class  Vector3 */
static int toluaI_zfscript_bind_Vector3_Print00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"Vector3"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  Vector3* self = (Vector3*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Print'");
 {
  self->Print();
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Print'.");
 return 0;
}

/* method: new of class  GameScriptInterface */
static int toluaI_zfscript_bind_GameScriptInterface_new00(lua_State* tolua_S)
{
 if (
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
 {
  GameScriptInterface* toluaI_ret = (GameScriptInterface*)  new GameScriptInterface();
 tolua_pushusertype(tolua_S,(void*)toluaI_ret,tolua_tag(tolua_S,"GameScriptInterface"));
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.");
 return 0;
}

/* method: delete of class  GameScriptInterface */
static int toluaI_zfscript_bind_GameScriptInterface_delete00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"GameScriptInterface"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  GameScriptInterface* self = (GameScriptInterface*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'");
 delete self;
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.");
 return 0;
}

/* method: raw_delete; called by garbage collector */
static int tolua_GameScriptInterface_tolua_raw_delete(lua_State* tolua_S)
{
  GameScriptInterface* self = (GameScriptInterface*)  lua_touserdata(tolua_S,1);
 delete self;
 return 0;
}

/* method: GetPlayer of class  GameScriptInterface */
static int toluaI_zfscript_bind_GameScriptInterface_GetPlayer00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"GameScriptInterface"),0) ||
 !tolua_isnoobj(tolua_S,2)
 )
 goto tolua_lerror;
 else
 {
  GameScriptInterface* self = (GameScriptInterface*)  tolua_getusertype(tolua_S,1,0);
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPlayer'");
 {
  OBJECT toluaI_ret = (OBJECT)  self->GetPlayer();
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPlayer'.");
 return 0;
}

/* method: GetClosestEnemy of class  GameScriptInterface */
static int toluaI_zfscript_bind_GameScriptInterface_GetClosestEnemy00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"GameScriptInterface"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  GameScriptInterface* self = (GameScriptInterface*)  tolua_getusertype(tolua_S,1,0);
  OBJECT kFocusObj = ((OBJECT)  tolua_getnumber(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetClosestEnemy'");
 {
  OBJECT toluaI_ret = (OBJECT)  self->GetClosestEnemy(kFocusObj);
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetClosestEnemy'.");
 return 0;
}

/* method: GetObjectType of class  GameScriptInterface */
static int toluaI_zfscript_bind_GameScriptInterface_GetObjectType00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"GameScriptInterface"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  GameScriptInterface* self = (GameScriptInterface*)  tolua_getusertype(tolua_S,1,0);
  OBJECT obj = ((OBJECT)  tolua_getnumber(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectType'");
 {
  OBJECT_TYPE toluaI_ret = (OBJECT_TYPE)  self->GetObjectType(obj);
 {
#ifdef __cplusplus
 void* toluaI_clone = new OBJECT_TYPE(toluaI_ret);
#else
 void* toluaI_clone = tolua_copy(tolua_S,(void*)&toluaI_ret,sizeof(OBJECT_TYPE));
#endif
 tolua_pushusertype(tolua_S,tolua_doclone(tolua_S,toluaI_clone,tolua_tag(tolua_S,"OBJECT_TYPE")),tolua_tag(tolua_S,"OBJECT_TYPE"));
 }
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectType'.");
 return 0;
}

/* method: GetObjectPos of class  GameScriptInterface */
static int toluaI_zfscript_bind_GameScriptInterface_GetObjectPos00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"GameScriptInterface"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  GameScriptInterface* self = (GameScriptInterface*)  tolua_getusertype(tolua_S,1,0);
  OBJECT obj = ((OBJECT)  tolua_getnumber(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectPos'");
 {
  Vector3 toluaI_ret = (Vector3)  self->GetObjectPos(obj);
 {
#ifdef __cplusplus
 void* toluaI_clone = new Vector3(toluaI_ret);
#else
 void* toluaI_clone = tolua_copy(tolua_S,(void*)&toluaI_ret,sizeof(Vector3));
#endif
 tolua_pushusertype(tolua_S,tolua_doclone(tolua_S,toluaI_clone,tolua_tag(tolua_S,"Vector3")),tolua_tag(tolua_S,"Vector3"));
 }
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectPos'.");
 return 0;
}

/* method: GetClosestObject of class  GameScriptInterface */
static int toluaI_zfscript_bind_GameScriptInterface_GetClosestObject00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"GameScriptInterface"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  GameScriptInterface* self = (GameScriptInterface*)  tolua_getusertype(tolua_S,1,0);
  OBJECT kFocusObj = ((OBJECT)  tolua_getnumber(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetClosestObject'");
 {
  OBJECT toluaI_ret = (OBJECT)  self->GetClosestObject(kFocusObj);
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetClosestObject'.");
 return 0;
}

/* method: GetMetersBetween of class  GameScriptInterface */
static int toluaI_zfscript_bind_GameScriptInterface_GetMetersBetween00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"GameScriptInterface"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_istype(tolua_S,3,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,4)
 )
 goto tolua_lerror;
 else
 {
  GameScriptInterface* self = (GameScriptInterface*)  tolua_getusertype(tolua_S,1,0);
  OBJECT obj1 = ((OBJECT)  tolua_getnumber(tolua_S,2,0));
  OBJECT obj2 = ((OBJECT)  tolua_getnumber(tolua_S,3,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMetersBetween'");
 {
  float toluaI_ret = (float)  self->GetMetersBetween(obj1,obj2);
 tolua_pushnumber(tolua_S,(double)toluaI_ret);
 }
 }
 return 1;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMetersBetween'.");
 return 0;
}

/* method: DestroyObject of class  GameScriptInterface */
static int toluaI_zfscript_bind_GameScriptInterface_DestroyObject00(lua_State* tolua_S)
{
 if (
 !tolua_istype(tolua_S,1,tolua_tag(tolua_S,"GameScriptInterface"),0) ||
 !tolua_istype(tolua_S,2,LUA_TNUMBER,0) ||
 !tolua_isnoobj(tolua_S,3)
 )
 goto tolua_lerror;
 else
 {
  GameScriptInterface* self = (GameScriptInterface*)  tolua_getusertype(tolua_S,1,0);
  OBJECT obj = ((OBJECT)  tolua_getnumber(tolua_S,2,0));
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DestroyObject'");
 {
  self->DestroyObject(obj);
 }
 }
 return 0;
tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DestroyObject'.");
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
 tolua_cclass(tolua_S,"Vector3","");
 tolua_tablevar(tolua_S,"Vector3","ZERO",toluaI_get_zfscript_bind_Vector3_ZERO,NULL);
 tolua_tablevar(tolua_S,"Vector3","AXIS_X",toluaI_get_zfscript_bind_Vector3_AXIS_X,NULL);
 tolua_tablevar(tolua_S,"Vector3","AXIS_Y",toluaI_get_zfscript_bind_Vector3_AXIS_Y,NULL);
 tolua_tablevar(tolua_S,"Vector3","AXIS_Z",toluaI_get_zfscript_bind_Vector3_AXIS_Z,NULL);
 tolua_tablevar(tolua_S,"Vector3","degtorad",toluaI_get_zfscript_bind_Vector3_degtorad,NULL);
 tolua_tablevar(tolua_S,"Vector3","x",toluaI_get_zfscript_bind_Vector3_x,toluaI_set_zfscript_bind_Vector3_x);
 tolua_tablevar(tolua_S,"Vector3","y",toluaI_get_zfscript_bind_Vector3_y,toluaI_set_zfscript_bind_Vector3_y);
 tolua_tablevar(tolua_S,"Vector3","z",toluaI_get_zfscript_bind_Vector3_z,toluaI_set_zfscript_bind_Vector3_z);
 tolua_function(tolua_S,"Vector3","new",toluaI_zfscript_bind_Vector3_new00);
 tolua_function(tolua_S,"Vector3","new",toluaI_zfscript_bind_Vector3_new01);
 tolua_function(tolua_S,"Vector3","Length",toluaI_zfscript_bind_Vector3_Length00);
 tolua_function(tolua_S,"Vector3","LengthSqr",toluaI_zfscript_bind_Vector3_LengthSqr00);
 tolua_function(tolua_S,"Vector3","SquaredLength",toluaI_zfscript_bind_Vector3_SquaredLength00);
 tolua_function(tolua_S,"Vector3","Normalize",toluaI_zfscript_bind_Vector3_Normalize00);
 tolua_function(tolua_S,"Vector3","Dot",toluaI_zfscript_bind_Vector3_Dot00);
 tolua_function(tolua_S,"Vector3","Unit",toluaI_zfscript_bind_Vector3_Unit00);
 tolua_function(tolua_S,"Vector3","Cross",toluaI_zfscript_bind_Vector3_Cross00);
 tolua_function(tolua_S,"Vector3","IsZero",toluaI_zfscript_bind_Vector3_IsZero00);
 tolua_function(tolua_S,"Vector3","NearlyEquals",toluaI_zfscript_bind_Vector3_NearlyEquals00);
 tolua_function(tolua_S,"Vector3","NearlyZero",toluaI_zfscript_bind_Vector3_NearlyZero00);
 tolua_function(tolua_S,"Vector3","Proj",toluaI_zfscript_bind_Vector3_Proj00);
 tolua_function(tolua_S,"Vector3","Perp",toluaI_zfscript_bind_Vector3_Perp00);
 tolua_function(tolua_S,"Vector3","Angle",toluaI_zfscript_bind_Vector3_Angle00);
 tolua_function(tolua_S,"Vector3","Abs",toluaI_zfscript_bind_Vector3_Abs00);
 tolua_function(tolua_S,"Vector3","AbsMaxCoo",toluaI_zfscript_bind_Vector3_AbsMaxCoo00);
 tolua_function(tolua_S,"Vector3","Lerp",toluaI_zfscript_bind_Vector3_Lerp00);
 tolua_function(tolua_S,"Vector3","Angels",toluaI_zfscript_bind_Vector3_Angels00);
 tolua_function(tolua_S,"Vector3","AToU",toluaI_zfscript_bind_Vector3_AToU00);
 tolua_function(tolua_S,"Vector3","Print",toluaI_zfscript_bind_Vector3_Print00);
 tolua_constant(tolua_S,NULL,"ObjTypeSelf",ObjTypeSelf);
 tolua_constant(tolua_S,NULL,"ObjTypeCreature",ObjTypeCreature);
 tolua_constant(tolua_S,NULL,"ObjTypeDoor",ObjTypeDoor);
 tolua_constant(tolua_S,NULL,"ObjTypeItem",ObjTypeItem);
 tolua_constant(tolua_S,NULL,"ObjTypeNone",ObjTypeNone);
 tolua_cclass(tolua_S,"GameScriptInterface","");
 tolua_function(tolua_S,"GameScriptInterface","new",toluaI_zfscript_bind_GameScriptInterface_new00);
 tolua_function(tolua_S,"GameScriptInterface","delete",toluaI_zfscript_bind_GameScriptInterface_delete00);
 tolua_function(tolua_S,"GameScriptInterface","tolua_raw_delete",tolua_GameScriptInterface_tolua_raw_delete);
 tolua_function(tolua_S,"GameScriptInterface","GetPlayer",toluaI_zfscript_bind_GameScriptInterface_GetPlayer00);
 tolua_function(tolua_S,"GameScriptInterface","GetClosestEnemy",toluaI_zfscript_bind_GameScriptInterface_GetClosestEnemy00);
 tolua_function(tolua_S,"GameScriptInterface","GetObjectType",toluaI_zfscript_bind_GameScriptInterface_GetObjectType00);
 tolua_function(tolua_S,"GameScriptInterface","GetObjectPos",toluaI_zfscript_bind_GameScriptInterface_GetObjectPos00);
 tolua_function(tolua_S,"GameScriptInterface","GetClosestObject",toluaI_zfscript_bind_GameScriptInterface_GetClosestObject00);
 tolua_function(tolua_S,"GameScriptInterface","GetMetersBetween",toluaI_zfscript_bind_GameScriptInterface_GetMetersBetween00);
 tolua_function(tolua_S,"GameScriptInterface","DestroyObject",toluaI_zfscript_bind_GameScriptInterface_DestroyObject00);
 return 1;
}
/* Close function */
void tolua_zfscript_bind_close (lua_State* tolua_S)
{
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"TEXT_MAX_LENGHT");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"BasicConsole");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"COMMAND_LENGHT");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"Console");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"Vector3");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ObjTypeSelf");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ObjTypeCreature");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ObjTypeDoor");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ObjTypeItem");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"ObjTypeNone");
 lua_pushnil(tolua_S); lua_setglobal(tolua_S,"GameScriptInterface");
}
