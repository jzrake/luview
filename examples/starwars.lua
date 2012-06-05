
local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local tubes = luview.ParameterizedPathArtist()
local shader = shaders.load_shader("phong")

local lines = { }
local f = io.open(arg[1] or arg[0])
while true do
   local line = f:read("*line")
   if not line then break end
   table.insert(lines, line)
end
f = nil

local function new_text(str)
   local t = luview.TextRendering()
   t:set_text(str)
   t:set_color(0.5, 0.5, 0.0)
--   t:set_shader(shader)
   t:set_scale(0.07, 0.1, 0.1)
   t:set_orientation(-45*3/2, 0, 0)
   return t
end

local time = -1.3
local text = { }

local function update_lines(first)
   for k,v in pairs(lines) do
      local t = first and new_text(v) or text[k]
      t:set_position(-0.5, 0.5*(0.3-k+time)*2/3, 0.5*(0.3+k-time))
      if first then text[k] = t end
   end
   collectgarbage()
   time = time + 0.06
end

box:set_shader(shader)
box:set_linewidth(1.0)

update_lines(true)
window:set_color(0,0,0)
window:set_callback("idle", update_lines)
window:render_scene(text)
