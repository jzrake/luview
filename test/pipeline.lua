

local luview = require 'luview'
local lunum = require 'lunum'


local window = luview.Window()
local box = luview.BoundingBox()
local grid2d = luview.GridSource2D()
local surface = luview.SurfaceNURBS()
local ctrlpnt = luview.FunctionMapping()
local scolors = luview.FunctionMapping()







local function normalize_input(f)
   local function g(x,y,z,caller)
      if caller ~= nil then
	 local zmin=caller:get_info("min2")
	 local zmax=caller:get_info("max2")
	 z = 6*(z - zmin)
      end
      return f(x,y,z)
   end
   return g
end

local time = 0.0
local function stingray(u,v)
   return u, v, 5*(u^4 + u*v^3) * math.cos(20*u*v) * math.cos(time)
end

local function cmap1(x,y,z)
   return z,z,z,0.9
end

local function cmap2(x,y,z)
   local b = 6
   return math.exp(-(z-0.1)^b), math.exp(-(z-0.5)^b), math.exp(-(z-0.9)^b), 0.8
end

ctrlpnt:set_input(grid2d)
ctrlpnt:set_transform(stingray)

scolors:set_input(ctrlpnt)
scolors:set_transform(normalize_input(cmap2))

surface:set_data("control_points", ctrlpnt)
surface:set_data("colors", scolors)
surface:set_alpha(0.8)
surface:set_color(1.0, 0.5, 0.3)


window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)

local cycle = 0
while window:render_scene({box, surface}) == "continue" do
   time = time + 0.01
   ctrlpnt:set_transform(stingray)
   cycle = cycle + 1
end
