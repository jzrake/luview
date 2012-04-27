

local luview = require 'luview'
local lunum = require 'lunum'


local window = luview.Window()
local box = luview.BoundingBox()
local grid2d = luview.GridSource2D()
local surface = luview.SurfaceNURBS()
local ctrlpnt = luview.FunctionMapping()
local scolors = luview.FunctionMapping()



local points = luview.PointsSource()
local colors = luview.PointsSource()
local pntens = luview.PointsEnsemble()

local Npnt = 200
local pntdata = lunum.zeros{Npnt,3}
local clrdata = lunum.zeros{Npnt,4}

local xn = { }
xn[1] = 0.5

for n=1,Npnt+10 do
   xn[n+1] = 3.999 * xn[n] * (1 - xn[n])
end

for i=0,Npnt-1 do
   pntdata[{i,0}] = xn[i+1]
   pntdata[{i,1}] = xn[i+2]
   pntdata[{i,2}] = xn[i+3]

   clrdata[{i,0}] = 0.5
   clrdata[{i,1}] = 0.5
   clrdata[{i,2}] = 0.5
   clrdata[{i,3}] = 0.9
end

points:set_points(pntdata)
colors:set_points(clrdata)

pntens:set_data("control_points", points)
pntens:set_data("colors", colors)
pntens:set_position(-0.5, -0.5, 0.5)

local function normalize_input(f)
   local function g(x,y,z,caller)
      if caller ~= nil then
	 local zmin=caller:get_info("min2")
	 local zmax=caller:get_info("max2")
	 z = (z - zmin) / (zmax - zmin)
      end
      return f(x,y,z)
   end
   return g
end

local time = 0.0
local function stingray(u,v)
   return u, v, 6*(u^4 + u*v^3) * math.cos(20*u*v) * math.cos(time)
end

local function cmap1(x,y,z)
   return z,z,z,0.9
end

local function cmap2(x,y,z)
   local a = 100.0
   local b = 2
   return math.exp(-a*(z-0.3)^b), math.exp(-a*(z-0.5)^b), math.exp(-a*(z-0.7)^b), 0.8
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
while window:render_scene({box, pntens}) == "continue" do
   time = time + 0.05
   ctrlpnt:set_transform(stingray)
   cycle = cycle + 1
end
