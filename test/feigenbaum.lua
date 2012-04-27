

local luview = require 'luview'
local lunum = require 'lunum'


local window = luview.Window()
local box = luview.BoundingBox()
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

   clrdata[{i,0}] = xn[i+1]
   clrdata[{i,1}] = xn[i+2]
   clrdata[{i,2}] = xn[i+3]
   clrdata[{i,3}] = 0.0
end

points:set_points(pntdata)
colors:set_points(clrdata)

pntens:set_data("control_points", points)
pntens:set_data("colors", colors)
pntens:set_position(-0.5, -0.5, 0.5)
pntens:set_linewidth(64.0)
pntens:set_alpha(0.2)

window:set_color(0.2, 0.2, 0.1)
box:set_color(0.5, 0.9, 0.9)

while window:render_scene({box, pntens}) == "continue" do end
