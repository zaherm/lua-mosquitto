# lua-mosquitto
Lua binding for libmosquitto (https://github.com/eclipse/mosquitto).

# Example
```lua
local mosquitto = require("mosquitto")
local m = mosquitto.new("id:test", true)
-- set callbacks
m:on("connect", function(rc, ud)
  print("on:connect", rc)
end)

m:on("log", function(msg)
  print("log::message = '"..msg.."'")
end)

m:on("publish", function(mid)
  print("mid: ", mid)
end)

m:on("message", function(msg)
  print("message::mid =", msg.mid)
end)
-- connect
local ok, rc = m:connect("localhost", 1883, 10, true)
assert(ok)
-- set will
m:set_will("topic/will-path", "will!", 1, true)
-- subscribe
m:subscribe("topic/path", 1)
m:subscribe("topic/path/2", 1)
-- publish
m:publish("topic/path", "test test", 1, false)
-- loop for 10 seconds
m:loop(10, 1)
-- clear will
m:clear_will()
m:disconnect()

```

# Doc
## mosquitto
### mosqitto.lib_version()
Returns:
* version[string] - string represntation of the version
* version[number] - numerical represntation of the version

### mosquitto.new(id, clean_session)
Create new mosquitto instance

Params:
* id[string]
* clean_session[boolean]

Returns:
* mosquitto instance

### mosquitto.connect(host, port, keepalive [, async])
Connect to host:port

Params:
* host[string]
* port[integer]
* keepalive[integer]
* async[boolean/optional]

Returns:
* ok[boolean]
* rc[integer]

### mosquitto:connect_bind(host, port, keepalive, bind_address [, async])
Connect to host:port

Params:
* host[string]
* port[integer]
* keepalive[integer]
* bind_address[string]
* async[boolean/optional]

Returns:
* ok[boolean]
* rc[integer]

### mosquitto:reconnect([async])
reconnect

Params:
* async[boolean/optional]

Returns:
* ok[boolean]
* rc[integer]

### mosquitto:disconnect()
Disconnect

Returns:
* ok[boolean]

### mosquitto:loop(timeout, max_packets)
Start the mosquitto loop

Params:
* timeout[integer]
* max_packets[integer]

Returns:
* ok[boolean]
* rc[intenger] - return code

### mosquitto:loop_forever(timeout, max_packets)
Start the mosquitto loop forever

Params:
* timeout[integer]
* max_packets[integer]

Returns:
* ok[boolean]
* rc[integer]

### mosquitto:subscribe(sub, qos)
Subscribe

Params:
* sub[string]
* qos[integer]

Returns:
* ok[boolean]
* mid/rc[integer] - message id on success/rc on failure

### mosquitto:unsubscribe(sub, qos)
Unsubscribe

Params:
* sub[string]

Returns:
* ok[boolean]
* mid/rc[integer] - message id on success/rc on failure

### mosquitto:publish(topic, payload, qos, retain)
Publish a message

Params:
* topic[string]
* payload[string]
* qos[integer]
* retain[boolean]

Returns:
* ok[boolean]
* mid/rc[integer] - message id on success/rc on failure

### mosqitto:on(event, callback[, user_data])
Set a callback for specific event

Params:
* event[string] - connect, disconnect, subscribe, unsubscribe, message, log, publish
* callback[function] - callback with two params `data` and `user_data`

Returns:
* none

### mosquitto:set_username_pw(username, password)
Set username & password for authentication.

Params:
* username[string]
* password[string]

Returns:
* ok[boolean]
* rc[integer]

### mosqitto:set_tls(cafile, capath, certfile, keyfile)
Set TLS parameters

Params:
* cafile[string]
* capath[string]
* certfile[string]
* keyfile[string]

Returns:
* ok[boolean]
* rc[integer]

### mosqitto:set_tls_options(cert_reqs, tls_version, ciphers)
Set TLS options

Params:
* cert_reqs[integer]
* tls_version[string]
* ciphers[string]

Returns:
* ok[boolean]
* rc[integer]

### mosqitto:set_tls_insecure(value)
Set TLS insecure mode

Params:
* value[boolean]

Returns:
* ok[boolean]
* rc[integer]

### mosqitto:set_tls_psk(psk, identity, ciphers)
Set TLS psk options

Params:
* psk[string]
* identity[string]
* ciphers[string]

Returns:
* ok[boolean]
* rc[integer]

### mosqitto:set_max_inflight_messages(max_inflight_messages)
Set max inflight messages

Params:
* max_inflight_messages[integer]

Returns:
* ok[boolean]
* rc[integer]

### mosqitto:set_message_retry(message_retry)
Set message retry

Params:
* message_retry[integer]

Returns:
* ok[boolean]

### mosqitto:set_reconnect_delay(delay, delay_max, exponential_backoff)
Set reconnect params

Params:
* delay[integer]
* delay_max[integer]
* exponential_backoff[boolean]

Returns:
* ok[boolean]
* rc[integer]

### mosqitto.connack_string(connack_code)
Get the connack string of a connack code

Params:
* connack_code[integer]

Returns:
* connack_string[connack_code]

### mosqitto.strerror(errno)
Get the error representation of an errno

Params:
* errno[integer]

Returns:
* str[errno]
