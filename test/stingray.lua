
local lunum = require 'lunum'
local luview = require 'luview'


local window = luview.Window()
local bounding_box = luview.BoundingBox()
local little_box = luview.BoundingBox()
local surface = luview.SurfaceNURBS()




local Nx = 32
local Ny = 32

local X = lunum.zeros{Nx,Ny}
local Y = lunum.zeros{Nx,Ny}

for i,j in X:indices() do X[{i,j}] = i / Nx - 0.5 end
for i,j in Y:indices() do Y[{i,j}] = j / Ny - 0.5 end

local function MakeSurfaceData(t)
   return 5 * (X^4 + X*Y^3) * lunum.cos(20*X*Y) * math.cos(t)
--   return 0.3*lunum.cos(4*X*Y) * lunum.sin(3*Y*X) * math.sin(t)
end


bounding_box:set_linewidth(5.5)
bounding_box:set_color(0.1, 0.8, 0.6)

little_box:set_scale(0.2, 0.5, 0.5)
little_box:set_orientation(0.0, 20.0, 0.0)
little_box:set_color(1.0, 0.2, 0.1)
little_box:set_position(0, 0.5, 0)

window:set_orientation(45,0,0)
window:set_position(0.0, 0.0, -2)
window:set_color(0.3, 0.5, 0.5)

local function color_func(x)
   return
   math.sin(40*x)^2,
   math.cos(40*x)^6,
   math.sin(40*x+10)^4
end


function ThePoints(u,v)
   return
   u,
   v,
   5*(u^4 + u*v^3) * math.cos(20*u*v) * math.cos(0.0)
end

local ctrl_points_f = luview.ParametricFunctionDataSource()
ctrl_points_f:set_function(ThePoints)

local SurfacePoints = MakeSurfaceData(0.0)
local ctrl_points_a = luview.ParametricArrayDataSource()
ctrl_points_a:set_array(SurfacePoints)


surface:set_data("control_points", ctrl_points_a)
surface:set_alpha(0.8)
surface:set_color(1.0,0.5,0.3)


local time = 0.0
local scene = {bounding_box, surface}

while window:render_scene(scene) == "continue" do
   time = time + 0.05
   collectgarbage()
end
