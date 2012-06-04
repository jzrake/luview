
local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local tubes = luview.ParameterizedPathArtist()
local shader = shaders.load_shader("phong")

box:set_shader(shader)
box:set_linewidth(1.0)

local lines = { }
local f = io.open(arg[1] or arg[0])
while true do
   local line = f:read("*line")
   if not line then break end
   table.insert(lines, line)
end
f = nil

local time = -1.3
text = { }
local function update_lines(first)
   for k,v in pairs(lines) do
      local t = first and luview.TextRendering() or text[k]
      t:set_text(v)
      t:set_position(-0.5, 0.5*(0.3-k/2+time)*2/3, 0.5*(0.3+k/2-time))
      t:set_orientation(-45*3/2, 0, 0)
      t:set_color(0.5, 0.5, 0.0)
      t:set_shader(shader)
      t:set_scale(0.07, 0.1, 0.1)
      if first then text[k] = t end
   end
   collectgarbage()
   time = time + 0.03
end

update_lines(true)
window:set_color(0,0,0)
window:set_callback("idle", update_lines)
window:render_scene(text)
