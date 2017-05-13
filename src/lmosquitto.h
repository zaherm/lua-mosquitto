#ifndef LMOSQUITTO_H
#define LMOSQUITTO_H
#include "mosquitto.h"
#include "lmosquitto_aux.h"

#define LMOSQUITTO_VERSION "lua-mosquitto 0.0.1"
#define LMOSQUITTO_COPYRIGHT "Copyright (c) 2017, Zaher Marzuq"
#define LMOSQUITTO_DESCRIPTION "Lua binding for MOSQUITTO"

LUALIB_API int luaopen_mosquitto(lua_State *L);
/* lib funcs */
typedef struct {
  int init;
} lmosquitto_lib;

#define LMOSQUITTO_LIB_MT "mosquitto.lib"

lmosquitto_lib *lmosquitto_lib_check(lua_State *L, int index);
LUALIB_API int lmosquitto_lib_version(lua_State *L);
LUALIB_API int lmosquitto_lib_init(lua_State *L);
LUALIB_API int lmosquitto_lib_cleanup(lua_State *L);
LUALIB_API int lmosquitto_lib_index(lua_State *L);

static const luaL_Reg lmosquitto_lib_mt[] = {
  { "version", lmosquitto_lib_version },
  { "init", lmosquitto_lib_init },
  { "cleanup", lmosquitto_lib_cleanup },
  { "__gc", lmosquitto_lib_cleanup },
  { "__index", lmosquitto_lib_index },
  { NULL, NULL }
};

/* lib funcs */
typedef struct {
  struct mosquitto *mosq;
  /* state associated with the instance */
  lua_State *L;
  /* callbacks */
  int cb[8];
  int cb_udata[8];
} lmosquitto;

#define LMOSQUITTO_MT "mosquitto"

typedef enum {
  LMOSQUITTO_CB_CONNECT = 0,
  LMOSQUITTO_CB_DISCONNECT = 1,
  LMOSQUITTO_CB_SUBSCRIBE = 2,
  LMOSQUITTO_CB_UNSUBSCRIBE = 3,
  LMOSQUITTO_CB_PUBLISH = 4,
  LMOSQUITTO_CB_MESSAGE = 5,
  LMOSQUITTO_CB_LOG = 6
} lmosquitto_callback;

static const char *const lmosquitto_callback_options[] = {
  "connect", "disconnect", "subscribe", "unsubscribe", "publish",
  "message", "log"
};


lmosquitto *lmosquitto_check(lua_State *L, int index);
LUALIB_API int lmosquitto_new(lua_State *L);
LUALIB_API int lmosquitto_index(lua_State *L);
LUALIB_API int lmosquitto_destroy(lua_State *L);
LUALIB_API int lmosquitto_connect(lua_State *L);
LUALIB_API int lmosquitto_disconnect(lua_State *L);
LUALIB_API int lmosquitto_subscribe(lua_State *L);
LUALIB_API int lmosquitto_unsubscribe(lua_State *L);
LUALIB_API int lmosquitto_publish(lua_State *L);
LUALIB_API int lmosquitto_on(lua_State *L);
LUALIB_API int lmosquitto_loop(lua_State *L);
/* setters */
LUALIB_API int lmosquitto_set_username_pw(lua_State *L);
LUALIB_API int lmosquitto_set_tls(lua_State *L);
LUALIB_API int lmosquitto_set_tls_opts(lua_State *L);
LUALIB_API int lmosquitto_set_tls_insecure(lua_State *L);
LUALIB_API int lmosquitto_set_tls_psk(lua_State *L);
LUALIB_API int lmosquitto_set_max_inflight_messages(lua_State *L);
LUALIB_API int lmosquitto_set_message_retry(lua_State *L);
LUALIB_API int lmosquitto_set_reconnect_delay(lua_State *L);
/* will */
LUALIB_API int lmosquitto_set_will(lua_State *L);
LUALIB_API int lmosquitto_clear_will(lua_State *L);
/* utils */
LUALIB_API int lmosquitto_connack_string(lua_State *L);
LUALIB_API int lmosquitto_strerror(lua_State *L);

static const luaL_Reg lmosquitto_mt[] = {
  { "__gc", lmosquitto_destroy },
  { "__index", lmosquitto_index },
  { "connect", lmosquitto_connect },
  { "disconnect", lmosquitto_disconnect },
  { "subscribe", lmosquitto_subscribe },
  { "unsubscribe", lmosquitto_unsubscribe },
  { "publish", lmosquitto_publish },
  { "loop", lmosquitto_loop },
  { "on", lmosquitto_on },
  { "set_username_pw", lmosquitto_set_username_pw },
  { "set_tls", lmosquitto_set_tls },
  { "set_tls_opts", lmosquitto_set_tls_opts },
  { "set_tls_insecure", lmosquitto_set_tls_insecure },
  { "set_tls_psk", lmosquitto_set_tls_psk },
  { "set_max_inflight_messages", lmosquitto_set_max_inflight_messages },
  { "set_message_retry", lmosquitto_set_message_retry },
  { "set_reconnect_delay", lmosquitto_set_reconnect_delay },
  { "set_will", lmosquitto_set_will },
  { "clear_will", lmosquitto_clear_will },
  { NULL, NULL }
};

static const luaL_Reg lmosquitto_funcs[] = {
  { "new", lmosquitto_new },
  { "connack_string", lmosquitto_connack_string },
  { "strerror", lmosquitto_strerror },
  { NULL, NULL }
};

#endif

