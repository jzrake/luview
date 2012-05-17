

local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'


local window = luview.Window()
local box = luview.BoundingBox()
local points = luview.PointsSource()
local colors = luview.FunctionMapping()
local pntens = luview.PointsEnsemble()
local shader = shaders.load_shader("lambertian")


local function make_data(lam)
   local Npnt = 800
   local pntdata = lunum.zeros{Npnt,3}

   local xn = { }
   xn[1] = 0.5

   for n=1,Npnt+10 do
      xn[n+1] = lam * xn[n] * (1 - xn[n])
   end

   for i=0,Npnt-1 do
      pntdata[{i,0}] = xn[i+1]
      pntdata[{i,1}] = xn[i+2]
      pntdata[{i,2}] = xn[i+3]
   end
   collectgarbage()
   return pntdata
end

local function cmap1(x,y,z)
   return z,z,z,0.9
end
local function cmap2(x,y,z)
   local a = 100.0
   local b = 2
   return math.exp(-a*(z-0.3)^b), math.exp(-a*(z-0.5)^b), math.exp(-a*(z-0.7)^b), 1.0
end


points:set_points(make_data(3.8))
colors:set_input(points)
colors:set_transform(cmap2)

pntens:set_data("points", points)
pntens:set_data("colors", colors)
pntens:set_position(-0.5, -0.5, 0.5)
pntens:set_linewidth(64.0)
pntens:set_alpha(0.1)

window:set_color(0.05, 0.1, 0.1)
box:set_color(0.5, 0.9, 0.9)
box:set_shader(shader)

local status = "continue"
local key = ''
local lambda = 3.6

while status == "continue" do
   status, key = window:render_scene({box, pntens})
   if key == 'l' then
      lambda = lambda - 0.01
      points:set_points(make_data(lambda))
   end
   if key == 'L' then
      lambda = lambda + 0.01
      points:set_points(make_data(lambda))
   end
end
