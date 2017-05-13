local mosquitto = require("mosquitto")

for k,v in pairs(mosquitto) do
  print(k, v)
end
assert(mosquitto.lib.version())

local m = mosquitto.new("id:test", true)
assert(m)
m:on("connect", function(rc, ud)
  print("on:connect", rc)
end)

m:on("disconnect", function(rc, ud)
  print("on:disconnect", rc)
end)

m:on("subscribe", function(qos, ud)
  print("subscribe::qos = ", qos, "qos_count = ", #qos)
end)

m:on("unsubscribe", function(mid, ud)
  print("unsubscribe::mid = ", mid)
end)

m:on("log", function(msg)
  print("log::message = '"..msg.."'")
end)

m:on("publish", function(mid)
  print("mid: ", mid)
end)

--[[m:on("message", function(msg)
  print("message::mid =", msg.mid)
end)
]]
local ok, rc
ok, rc = m:connect("localhost", 1883, 10, true)
assert(ok)
ok, rc = m:set_will("topic/will-path", "will!", 1, true)

local mid = m:subscribe("topic/path", 1)
local mid = m:subscribe("topic/path/2", 1)

m:publish("topic/path", "test test", 1, false)

local stime = os.time()


local mid = m:unsubscribe("topic/path")
while (os.time() - stime < 10) do
  m:loop(-1, 1)
end

print("mid = ", mid)
m:clear_will()
m:disconnect()
m = nil
collectgarbage()

