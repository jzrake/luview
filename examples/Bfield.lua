
local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local shader = shaders.load_shader("lambertian")

local toroidal = 1.0
local function Bdipole(x,y,z)
   local r = (x^2 + y^2 + z^2)^0.5
   local r3 = r^3
   local rhat = {x/r, y/r, z/r}
   local m = {0, 0, 1}
   local mdotrhat = m[1]*rhat[1] + m[2]*rhat[2] + m[3]*rhat[3]
   local Bp = {(3*mdotrhat*rhat[1] - m[1])/r3,
	       (3*mdotrhat*rhat[2] - m[2])/r3,
	       (3*mdotrhat*rhat[3] - m[3])/r3}
   local Bt = {toroidal*y*z/r, -toroidal*x*z/r, 0}
   if r < 1e-2 then return nil
   else return Bp[1] + Bt[1], Bp[2] + Bt[2], Bp[3] + Bt[3] end
end

local function build_tube(x0, y0, z0)
   local curve = luview.IntegralCurve()
   curve:set_starting_point(x0, y0, z0)
   curve:set_transform(Bdipole)
   curve:set_num_component_axes(1)

   local tube = luview.ParameterizedPathArtist()
   tube:set_data("points", curve)
   tube:set_linewidth(1.0)
   tube:set_shader(shader)
   tube:set_color(1,0,0)
   tube:set_orientation(-90,0,0)
   return tube
end

local actors = {box=box}
local radius = 0.1
local function init_tubes()
   local ntubes = 10
   for i=1,ntubes do
      local t = 2 * math.pi * i / ntubes
      local r = radius
      actors[i] = build_tube(r*math.cos(t), r*math.sin(t), r)
   end
end
init_tubes()

box:set_shader(shader)
box:set_linewidth(1.0)

window:set_callback("T", function() toroidal = toroidal + 1; init_tubes() end,
		    "increase toroidal component")
window:set_callback("t", function() toroidal = toroidal - 1; init_tubes() end,
		    "decrease toroidal component")
window:set_callback("R", function() radius = radius + 0.01; init_tubes() end,
		    "increase draw radius")
window:set_callback("r", function() radius = radius - 0.01; init_tubes() end,
		    "decrease draw radius")
window:set_color(0,0,0)
window:render_scene(actors)
