#include "lmosquitto.h"

/* */
lmosquitto_lib *lmosquitto_lib_check(lua_State *L, int index) {
  lmosquitto_lib *l = (lmosquitto_lib *) luaL_checkudata(L, index, LMOSQUITTO_LIB_MT);
  luaL_argcheck(L, l != NULL && l->init == 1, index, LMOSQUITTO_LIB_MT" expected");
  return l;
}

/*
* @function mosquitto.lib_version()
* @desc Obtain the libmosquitto version number
* @param None
* @return version[number] - Integer version number
* @return version[string] - String in the format X.Y.Z
*/
LUALIB_API int lmosquitto_lib_init(lua_State *L) {
  mosquitto_lib_init();
  lmosquitto_lib *l = (lmosquitto_lib *) lua_newuserdata(L, sizeof(lmosquitto_lib));
  l->init = 1;
  lmosquitto_setmeta(L, LMOSQUITTO_LIB_MT);
  return 1;
}
/***
* @function mosqitto.lib_version()
* @return version[string] - string represntation of the version
* @return version[number] - numerical represntation of the version
*/
LUALIB_API int lmosquitto_lib_version(lua_State *L) {
  int major, minor, revision;
  int v = mosquitto_lib_version(&major, &minor, &revision);
  lua_pushinteger(L, v);
  lua_pushfstring(L, "%d.%d.%d", major, minor, revision);
  return 2;
}

LUALIB_API int lmosquitto_lib_cleanup(lua_State *L) {
  lmosquitto_lib *self = lmosquitto_lib_check(L, 1);
  mosquitto_lib_cleanup();
  self->init = 0;
  return 0;
}

LUALIB_API int lmosquitto_lib_index(lua_State *L) {
  const char* key = luaL_checkstring(L, 2);
  lua_getmetatable(L, 1);
  lua_getfield(L, -1, key);
  if(!lua_isnil(L, -1)) {
    return 1;
  }
  return 0;
}

/* mosquitto */
LUALIB_API int lmosquitto_index(lua_State *L) {
  const char* key = luaL_checkstring(L, 2);
  lua_getmetatable(L, 1);
  lua_getfield(L, -1, key);
  if(!lua_isnil(L, -1)) {
    return 1;
  }
  return 0;
}

/***
* @function mosquitto.new(id, clean_session)
* @desc Create new mosquitto instance
* @param id[string]
* @param clean_session[boolean]
* @return mosquitto instance
*/
LUALIB_API int lmosquitto_new(lua_State *L) {
  lmosquitto *m = (lmosquitto *) lua_newuserdata(L, sizeof(lmosquitto));
  const char *id = luaL_checkstring(L, 1);
  bool clean_session = lua_toboolean(L, 2);
  m->mosq = mosquitto_new(id, clean_session, NULL);
  m->L = L;
  memset(m->cb, -1, 8*sizeof(int));
  memset(m->cb_udata, -1, 8*sizeof(int));
  mosquitto_user_data_set(m->mosq, m);
  lmosquitto_setmeta(L, LMOSQUITTO_MT);
  return 1;
}

lmosquitto *lmosquitto_check(lua_State *L, int index) {
  lmosquitto *m = (lmosquitto *) luaL_checkudata(L, index, LMOSQUITTO_MT);
  luaL_argcheck(L, m != NULL && m->mosq != NULL, index, LMOSQUITTO_MT" expected");
  return m;
}

/***
* @function mosquitto.connect(host, port, keepalive [, async])
* @desc Connect to host:port
* @param host[string]
* @param port[integer]
* @param keepalive[integer]
* @param async[boolean/optional]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_connect(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  const char *host = luaL_checkstring(L, 2);
  int port = luaL_checkinteger(L, 3);
  int keepalive = luaL_checkinteger(L, 4);
  bool async = lua_toboolean(L, 5);

  int rc;
  if(async) {
    rc = mosquitto_connect_async(self->mosq, host, port, keepalive);
  }
  else {
    rc = mosquitto_connect(self->mosq, host, port, keepalive);
  }
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosquitto:connect_bind(host, port, keepalive, bind_address [, async])
* @desc Connect to host:port
* @param host[string]
* @param port[integer]
* @param keepalive[integer]
* @param bind_address[string]
* @param async[boolean/optional]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_connect_bind(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  const char *host = luaL_checkstring(L, 2);
  int port = luaL_checkinteger(L, 3);
  int keepalive = luaL_checkinteger(L, 4);
  const char *bind_address = luaL_checkstring(L, 5);
  bool async = lua_toboolean(L, 6);
  int rc;
  if(async) {
    rc = mosquitto_connect_bind_async(self->mosq, host, port, keepalive, bind_address);
  }
  else {
    rc = mosquitto_connect_bind(self->mosq, host, port, keepalive, bind_address);
  }
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosquitto:reconnect([async])
* @desc reconnect
* @param async[boolean/optional]
* @return ok[boolean]
* @return rc[integer]
*/

LUALIB_API int lmosquitto_reconnect(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  bool async = lua_toboolean(L, 2);
  int rc;
  if (async) {
    rc = mosquitto_reconnect_async(self->mosq);
  }
  else {
    rc = mosquitto_reconnect(self->mosq);
  }
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosquitto:disconnect()
* @desc Disconnect
* @return ok[boolean]
*/
LUALIB_API int lmosquitto_disconnect(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  int rc = mosquitto_disconnect(self->mosq);
  lua_pushboolean(L, rc == 0);
  return 1;
}

/***
* @function mosquitto:loop(timeout, max_packets)
* @desc Start the mosquitto loop
* @param timeout[integer]
* @param max_packets[integer]
* @return ok[boolean]
* @return rc[intenger] - return code
*/
LUALIB_API int lmosquitto_loop(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  int timeout = luaL_checkinteger(L, 2);
  int max_packets = luaL_checkinteger(L, 3);
  int rc = mosquitto_loop(self->mosq, timeout, max_packets);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosquitto:loop_forever(timeout, max_packets)
* @desc Start the mosquitto loop forever
* @param timeout[integer]
* @param max_packets[integer]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_loop_forever(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  int timeout = luaL_checkinteger(L, 2);
  int max_packets = luaL_checkinteger(L, 3);
  int rc = mosquitto_loop_forever(self->mosq, timeout, max_packets);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosquitto:subscribe(sub, qos)
* @desc Subscribe
* @param sub[string]
* @param qos[integer]
* @return ok[boolean]
* @return mid/rc[integer] - message id on success/rc on failure
*/
LUALIB_API int lmosquitto_subscribe(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  const char *sub = luaL_checkstring(L, 2);
  int qos = luaL_checkinteger(L, 3);
  int mid = 0;
  int rc = mosquitto_subscribe(self->mosq, &mid, sub, qos);
  lua_pushboolean(L, rc == 0);
  lua_pushinteger(L, rc == 0 ? mid : rc);
  return 2;
}

/***
* @function mosquitto:unsubscribe(sub, qos)
* @desc Unsubscribe
* @param sub[string]
* @return ok[boolean]
* @return mid/rc[integer] - message id on success/rc on failure
*/
LUALIB_API int lmosquitto_unsubscribe(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  const char *sub = luaL_checkstring(L, 2);
  int mid;
  int rc = mosquitto_unsubscribe(self->mosq, &mid, sub);
  lua_pushboolean(L, rc == 0);
  lua_pushinteger(L, rc == 0 ? mid : rc);
  return 2;
}

/***
* @function mosquitto:publish(topic, payload, qos, retain)
* @desc Publish a message
* @param topic[string]
* @param payload[string]
* @param qos[integer]
* @param retain[boolean]
* @return ok[boolean]
* @return mid/rc[integer] - message id on success/rc on failure
*/
LUALIB_API int lmosquitto_publish(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  const char *topic = luaL_checkstring(L, 2);
  size_t payloadlen;
  const char *payload = luaL_checklstring(L, 3, &payloadlen);
  int mid, qos, rc;
  qos = luaL_checkinteger(L, 4);
  bool retain = lua_toboolean(L, 5);
  rc = mosquitto_publish(self->mosq, &mid, topic, payloadlen, payload, qos, retain);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/* will */
LUALIB_API int lmosquitto_set_will(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  const char *topic = luaL_checkstring(L, 2);
  size_t payloadlen;
  const char *payload = luaL_checklstring(L, 3, &payloadlen);
  int qos, rc;
  qos = luaL_checkinteger(L, 4);
  bool retain = lua_toboolean(L, 5);
  rc = mosquitto_will_set(self->mosq, topic, payloadlen, payload, qos, retain);
  lmosquitto_pushrc(L, rc);
  return 2;
}

LUALIB_API int lmosquitto_clear_will(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  int rc = mosquitto_will_clear(self->mosq);
  lmosquitto_pushrc(L, rc);
  return 2;
}



/* callbacks */

void lmosquitto_connect_callback(struct mosquitto *mosq, void *obj, int rc){
  lmosquitto *m = (lmosquitto *) obj;
  lua_State *L =  m->L;
  int top = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb[LMOSQUITTO_CB_CONNECT]);
  lua_pushinteger(L, rc);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb_udata[LMOSQUITTO_CB_CONNECT]);
  lua_pcall(L, 2, 0, 0);
  lua_settop(L, top);
}

void lmosquitto_message_callback(struct mosquitto *mosq, void *obj,
                                 const struct mosquitto_message *msg) {
  lmosquitto *m = (lmosquitto *) obj;
  lua_State *L =  m->L;
  int top = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb[LMOSQUITTO_CB_MESSAGE]);
  lua_newtable(L);
  lua_pushinteger(L, msg->mid);
  lua_setfield(L, -2, "mid");
  lua_pushstring(L, msg->topic);
  lua_setfield(L, -2, "topic");
  lua_pushlstring(L, msg->payload, msg->payloadlen);
  lua_setfield(L, -2, "payload");
  lua_pushinteger(L, msg->qos);
  lua_setfield(L, -2, "qos");
  lua_pushboolean(L, msg->retain);
  lua_setfield(L, -2, "retain");
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb_udata[LMOSQUITTO_CB_MESSAGE]);
  lua_pcall(L, 2, 0, 0);
  lua_settop(L, top);
}

void lmosquitto_disconnect_callback(struct mosquitto *mosq, void *obj, int rc) {
  lmosquitto *m = (lmosquitto *) obj;
  lua_State *L =  m->L;
  int top = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb[LMOSQUITTO_CB_DISCONNECT]);
  lua_pushinteger(L, rc);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb_udata[LMOSQUITTO_CB_DISCONNECT]);
  /* call lua function */
  lua_pcall(L, 2, 0, 0);
  lua_settop(L, top);
}

void lmosquitto_log_callback(struct mosquitto *mosq, void *obj, int rc,
                             const char *msg){
  lmosquitto *m = (lmosquitto *) obj;
  lua_State *L =  m->L;
  int top = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb[LMOSQUITTO_CB_LOG]);
  lua_pushstring(L, msg);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb_udata[LMOSQUITTO_CB_LOG]);
  /* call lua function */
  lua_pcall(L, 2, 0, 0);
  lua_settop(L, top);
}

void lmosquitto_publish_callback(struct mosquitto *mosq, void *obj, int mid) {
  lmosquitto *m = (lmosquitto *) obj;
  lua_State *L =  m->L;
  int top = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb[LMOSQUITTO_CB_PUBLISH]);
  lua_pushinteger(L, mid);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb_udata[LMOSQUITTO_CB_PUBLISH]);
  /* call lua function */
  lua_pcall(L, 2, 0, 0);
  lua_settop(L, top);
}

void lmosquitto_subscribe_callback(struct mosquitto *mosq, void *obj,
                                   int mid, int qos_count,
                                   const int *granted_qos) {
  lmosquitto *m = (lmosquitto *) obj;
  lua_State *L =  m->L;
  int top = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb[LMOSQUITTO_CB_SUBSCRIBE]);
  lua_createtable(L, qos_count, 0);
  for(int i = 0; i < qos_count; i++) {
    lua_pushinteger(L, granted_qos[i]);
    lua_rawseti(L, -2, i + 1);
  }
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb_udata[LMOSQUITTO_CB_SUBSCRIBE]);
  /* call lua function */
  lua_pcall(L, 2, 0, 0);
  lua_settop(L, top);
}

void lmosquitto_unsubscribe_callback(struct mosquitto *mosq, void *obj,
                                     int mid) {
  lmosquitto *m = (lmosquitto *) obj;
  lua_State *L =  m->L;
  int top = lua_gettop(L);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb[LMOSQUITTO_CB_UNSUBSCRIBE]);
  lua_pushinteger(L, mid);
  lua_rawgeti(L, LUA_REGISTRYINDEX, m->cb_udata[LMOSQUITTO_CB_UNSUBSCRIBE]);
  /* call lua function */
  lua_pcall(L, 2, 0, 0);
  lua_settop(L, top);
}

/***
* @function mosqitto:on(event, callback[, user_data])
* @desc Set a callback for specific event
* @param event[string] - connect, disconnect, subscribe, unsubscribe, message, log, publish
* @param callback[function] - callback with two params `data` and `user_data`
* @return none
*/
LUALIB_API int lmosquitto_on(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  int option = luaL_checkoption(L, 2, NULL, lmosquitto_callback_options);

  /* common to all callbacks */
  lua_settop(L, 4);
  luaL_unref(L, LUA_REGISTRYINDEX, self->cb_udata[option]);
  luaL_unref(L, LUA_REGISTRYINDEX, self->cb[option]);
  self->cb_udata[option] = luaL_ref(L, LUA_REGISTRYINDEX);
  self->cb[option] = luaL_ref(L, LUA_REGISTRYINDEX);

  switch(option) {
    case LMOSQUITTO_CB_CONNECT:
      mosquitto_connect_callback_set(self->mosq, lmosquitto_connect_callback);
      break;
    case LMOSQUITTO_CB_DISCONNECT:
      mosquitto_disconnect_callback_set(self->mosq, lmosquitto_disconnect_callback);
      break;
    case LMOSQUITTO_CB_SUBSCRIBE:
      mosquitto_subscribe_callback_set(self->mosq, lmosquitto_subscribe_callback);
      break;
    case LMOSQUITTO_CB_UNSUBSCRIBE:
      mosquitto_unsubscribe_callback_set(self->mosq, lmosquitto_unsubscribe_callback);
      break;
    case LMOSQUITTO_CB_MESSAGE:
      mosquitto_message_callback_set(self->mosq, lmosquitto_message_callback);
      break;
   case LMOSQUITTO_CB_LOG:
      mosquitto_log_callback_set(self->mosq, lmosquitto_log_callback);
      break;
   case LMOSQUITTO_CB_PUBLISH:
      mosquitto_publish_callback_set(self->mosq, lmosquitto_publish_callback);
      break;
    default:
      lmosquitto_error(L, "Not supported");
      break;
  }
  return 0;
}

LUALIB_API int lmosquitto_destroy(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  mosquitto_destroy(self->mosq);
  self->mosq = NULL;
  return 0;
}
/***
* @function mosquitto:set_username_pw(username, password)
* @desc Set username & password for authentication.
* @param username[string]
* @param password[string]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_set_username_pw(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  const char *username = luaL_checkstring(L, 2);
  const char *password = luaL_checkstring(L, 3);
  int rc = mosquitto_username_pw_set(self->mosq, username, password);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosqitto:set_tls(cafile, capath, certfile, keyfile)
* @desc Set TLS parameters
* @param cafile[string]
* @param capath[string]
* @param certfile[string]
* @param keyfile[string]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_set_tls(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  const char *cafile = luaL_optstring(L, 2, NULL);
  const char *capath = luaL_optstring(L, 3, NULL);
  const char *certfile = luaL_checkstring(L, 4);
  const char *keyfile = luaL_checkstring(L, 5);
  if(cafile == NULL && capath == NULL) {
    lmosquitto_error(L, "either capath or cafile must be set!");
  }
  int rc = mosquitto_tls_set(self->mosq, cafile, capath, certfile, keyfile, NULL);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosqitto:set_tls_options(cert_reqs, tls_version, ciphers)
* @desc Set TLS options
* @param cert_reqs[integer]
* @param tls_version[string]
* @param ciphers[string]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_set_tls_opts(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  int cert_reqs = luaL_checkint(L, 2);
  const char *tls_version = luaL_checkstring(L, 3);
  const char *ciphers = luaL_checkstring(L, 4);
  int rc = mosquitto_tls_opts_set(self->mosq, cert_reqs, tls_version, ciphers);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosqitto:set_tls_insecure(value)
* @desc Set TLS insecure mode
* @param value[boolean]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_set_tls_insecure(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  bool value = lua_toboolean(L, 2);
  int rc = mosquitto_tls_insecure_set(self->mosq, value);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosqitto:set_tls_psk(psk, identity, ciphers)
* @desc Set TLS psk options
* @param psk[string]
* @param identity[string]
* @param ciphers[string]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_set_tls_psk(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  const char *psk = luaL_checkstring(L, 2);
  const char *identity = luaL_checkstring(L, 3);
  const char *ciphers = luaL_checkstring(L, 4);
  int rc = mosquitto_tls_psk_set(self->mosq, psk, identity, ciphers);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosqitto:set_max_inflight_messages(max_inflight_messages)
* @desc Set max inflight messages
* @param max_inflight_messages[integer]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_set_max_inflight_messages(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  unsigned int max_inflight_messages = luaL_checkinteger(L, 2);
  int rc = mosquitto_max_inflight_messages_set(self->mosq, max_inflight_messages);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/***
* @function mosqitto:set_message_retry(message_retry)
* @desc Set message retry
* @param message_retry[integer]
* @return ok[boolean]
*/
LUALIB_API int lmosquitto_set_message_retry(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  unsigned int message_retry = luaL_checkinteger(L, 2);
  mosquitto_message_retry_set(self->mosq, message_retry);
  return 0;
}

/***
* @function mosqitto:set_reconnect_delay(delay, delay_max, exponential_backoff)
* @desc Set reconnect params
* @param delay[integer]
* @param delay_max[integer]
* @param exponential_backoff[boolean]
* @return ok[boolean]
* @return rc[integer]
*/
LUALIB_API int lmosquitto_set_reconnect_delay(lua_State *L) {
  lmosquitto *self = lmosquitto_check(L, 1);
  unsigned int delay = luaL_checkinteger(L, 2);
  unsigned int delay_max = luaL_checkinteger(L, 3);
  bool exponential_backoff = lua_toboolean(L, 4);
  int rc = mosquitto_reconnect_delay_set(self->mosq, delay, delay_max, exponential_backoff);
  lmosquitto_pushrc(L, rc);
  return 2;
}

/* utils */
/***
* @function mosqitto.connack_string(connack_code)
* @desc Get the connack string of a connack code
* @param connack_code[integer]
* @return connack_string[connack_code]
*/
LUALIB_API int lmosquitto_connack_string(lua_State *L) {
  int connack_code = luaL_checkinteger(L, 1);
  const char *connack_string = mosquitto_connack_string(connack_code);
  lua_pushstring(L, connack_string);
  return 1;
}

/***
* @function mosqitto.strerror(errno)
* @desc Get the error representation of an errno
* @param errno[integer]
* @return str[errno]
*/
LUALIB_API int lmosquitto_strerror(lua_State *L) {
  int errno = luaL_checkinteger(L, 1);
  const char *str = mosquitto_strerror(errno);
  lua_pushstring(L, str);
  return 1;
}

LUALIB_API int luaopen_mosquitto(lua_State *L) {
  lmosquitto_createmeta(L, LMOSQUITTO_LIB_MT, lmosquitto_lib_mt);
  lmosquitto_createmeta(L, LMOSQUITTO_MT, lmosquitto_mt);
  lua_newtable(L);
  luaL_setfuncs(L, lmosquitto_funcs, 0);
  lmosquitto_lib_init(L);
  lua_setfield(L, -2, "lib");
  lua_pushliteral(L, LMOSQUITTO_VERSION);
  lua_setfield(L, -2, "_VERSION");
  lua_pushliteral(L, LMOSQUITTO_COPYRIGHT);
  lua_setfield(L, -2, "_COPYRIGHT");
  lua_pushliteral(L, LMOSQUITTO_DESCRIPTION);
  lua_setfield(L, -2, "_DESCRIPTION");
  return 1;
}
