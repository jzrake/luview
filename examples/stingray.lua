


local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local grid2d = luview.GridSource2D()
local surface = luview.ParametricSurface()
local points = luview.DataSource()
--local scolors = luview.FunctionMapping()
--local normalize = luview.GlobalLinearTransformation()
local shader = shaders.load_shader("multlights")


local time = 0.0
local function stingray(u,v)
   return u, v, 6*(u^4 + u*v^3) * math.cos(20*u*v) * math.cos(time)
end

local function eggcrate(u,v)
   local sin = math.sin
   local cos = math.cos
   return u, v, 0.2 * math.sin(20*u) * cos(20*v)
end

local function cmap1(x,y,z)
   return z,z,z,0.9
end

local function cmap2(x,y,z)
   local a = 100.0
   local b = 2
   return math.exp(-a*(z-0.3)^b), math.exp(-a*(z-0.5)^b), math.exp(-a*(z-0.7)^b), 0.8
end

local Nx = 64
local Ny = 64
local A = lunum.zeros{Nx,Ny,3}
for i,j,k in A:indices() do
   local u = -0.5 + i/Nx
   local v = -0.5 + j/Ny
   local x, y, z = stingray(u,v)
   A[{i,j,0}] = x
   A[{i,j,1}] = y
   A[{i,j,2}] = z
end

points:set_data(A)
surface:set_data("points", points)
surface:set_color(1.0, 0.5, 0.3)
surface:set_orientation(-90,0,0)
surface:set_shader(shader)

window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)
box:set_shader(shader)

local cycle = 0
while window:render_scene{box, surface} == "continue" do
   time = time + 0.05
   cycle = cycle + 1
end
