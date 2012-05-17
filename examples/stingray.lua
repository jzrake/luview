


local luview = require 'luview'
local lunum = require 'lunum'
local utils = require 'test.utils'

local window = luview.Window()
local box = luview.BoundingBox()
local grid2d = luview.GridSource2D()
local surface = luview.SurfaceNURBS()
local ctrlpnt = luview.FunctionMapping()
local scolors = luview.FunctionMapping()
local shader = luview.ShaderProgram()
local normalize = luview.GlobalLinearTransformation()
utils.load_shader("lambertian", shader)


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

ctrlpnt:set_input(grid2d)
ctrlpnt:set_transform(stingray)

normalize:set_range(2, 0.0, 1.0)
normalize:set_input(ctrlpnt)

scolors:set_input(normalize)
scolors:set_transform(cmap2)


surface:set_data("control_points", ctrlpnt)
surface:set_data("colors", scolors)
surface:set_color(1.0, 0.5, 0.3)
--surface:set_shader(shader)

window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)
box:set_shader(shader)

local cycle = 0
while window:render_scene({box, surface}) == "continue" do
   time = time + 0.05
   cycle = cycle + 1
end
