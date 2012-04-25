

local luview = require 'luview'
local lunum = require 'lunum'


local window = luview.Window()
local box = luview.BoundingBox()
local grid2d = luview.GridSource2D()
local surface = luview.SurfaceNURBS()
local ctrlpnt = luview.FunctionMapping()
local scolors = luview.FunctionMapping()



ctrlpnt:set_input(grid2d)
ctrlpnt:set_transform(function(u,v) return u,v,-4*u^2+v^2 end)


local function normalize_input(f)
   local function g(x,y,z,caller)
      print(caller:get_info("max3"))
      return f(x,y,z)
   end
   return g
end

scolors:set_input(ctrlpnt)
scolors:set_transform(normalize_input(function(x,y,z) return 5*z,2*x,y,0.7 end))



surface:set_data("control_points", ctrlpnt)
surface:set_data("colors", scolors)
surface:set_alpha(0.8)
surface:set_color(1.0, 0.5, 0.3)

window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)

local cycle = 0
while window:render_scene({box, surface}) == "continue" do
   local t = cycle * 0.02
   ctrlpnt:set_transform(function(u,v) return
			    u,
			    v,
			    5*(u^4 + u*v^3) * math.cos(20*u*v) * math.cos(t)
			 end)
   cycle = cycle + 1
end
