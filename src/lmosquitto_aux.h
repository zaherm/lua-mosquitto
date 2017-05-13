#ifndef LMOSQUITTO_AUX_H
#define LMOSQUITTO_AUX_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lauxlib.h"
#include "mosquitto.h"

#if !defined LUA_VERSION_NUM || LUA_VERSION_NUM==501
LUALIB_API void luaL_setfuncs(lua_State *L, const luaL_Reg *l, int nup);
#endif

void lmosquitto_setmeta(lua_State *L, const char *name);
void lmosquitto_createmeta(lua_State *L, const char *name, const luaL_Reg *methods);
void lmosquitto_error(lua_State *L, const char *msg);
void lmosquitto_assert(lua_State *L, int cond, const char *msg);
void lmosquitto_pushrc(lua_State *L, int rc);
#endif
