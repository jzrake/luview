

local luview = require 'luview'
local lunum = require 'lunum'
local utils = require 'test.utils'

local window = luview.Window()
local box = luview.BoundingBox()
local grid2d = luview.GridSource2D()
local surface = luview.SurfaceNURBS()
local ctrlpnt = luview.FunctionMapping()
local shader = luview.ShaderProgram()

utils.load_shader("lambertian", shader)

local time = 0.0

local cos = math.cos
local cosh = math.cosh
local pi = math.pi
local A, phi = { }, { }
local M, N = 8,8

for m=1,M do
   A[m] = { }
   phi[m] = { }
   for n=1,N do
      A[m][n] = math.random()*0.015
      phi[m][n] = math.random()*pi
   end
end

local function waterwave(u,v)
   local L = 1.0
   local z = 0.0
   for m=1,M do
      for n=1,N do
	 local x, y = m*pi*u/L, n*pi*v/L
	 local k = ((m*pi/L)^2 + (n*pi/L)^2)^0.5
	 local w = k^0.5
	 z = z + A[m][n]*cos(x)*cos(y)*cos(w*time + phi[m][n])
      end
   end
   return u, v, z
end

grid2d:set_num_points(20, 20)
ctrlpnt:set_input(grid2d)
ctrlpnt:set_transform(waterwave)

surface:set_data("control_points", ctrlpnt)
surface:set_color(0.7, 0.7, 1.0)
surface:set_alpha(0.9)
surface:set_shader(shader)

box:set_color(0.6, 0.6, 0.6)
box:set_position(0.0, -0.5, 0.0)
box:set_shader(shader)

window:set_color(0.2, 0.2, 0.2)
window:set_orientation(45, 0, 0)

while window:render_scene({box, surface}) == "continue" do
   time = time + 0.04
end
