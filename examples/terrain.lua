


local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local height = luview.DataSource()
local verts = luview.ParametricVertexSource3D()
local triangles = luview.TrianglesEnsemble()
local points = luview.DataSource()
local lights = shaders.load_shader("multlights")

h5_open_file(cmdline.args[1], "r")
local data = h5_read_array("prim/rho")
h5_close_file()

height:set_data(data)
verts:set_input(height)

triangles:set_data("triangles", verts:get_output("triangles"))
triangles:set_data("normals", verts:get_output("normals"))
triangles:set_shader(lights)
triangles:set_alpha(1.0)
triangles:set_color(0.3, 0.8, 0.3)
--triangles:set_color(1.0, 0.5, 0.3)
triangles:set_orientation(-90,0,0)
triangles:set_scale(1.0, 0.1, 1.0)
window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)
box:set_shader(lights)

while window:render_scene{box, triangles} == "continue" do end


--[[
local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local grid2d = luview.GridSource2D()
local surface = luview.ParametricSurface()
local points = luview.DataSource()
local shader = shaders.load_shader("multlights")


h5_open_file(cmdline.args[1], "r")
local data = h5_read_array("prim/rho")
h5_close_file()

local Nx, Ny = unpack(data:shape())
local A = lunum.zeros{Nx,Ny,3}

for i=0,Nx-1 do
   for j=0,Ny-1 do
      local x = -0.5 + i/Nx
      local y = -0.5 + j/Ny
      local z = data[i*Ny + j]
      A[(i*Ny + j)*3 + 0] = x
      A[(i*Ny + j)*3 + 1] = y
      A[(i*Ny + j)*3 + 2] = z
   end
end

points:set_data(A)

surface:set_data("points", points)
surface:set_color(1.0, 0.5, 0.3)
surface:set_orientation(-90,0,0)
surface:set_shader(shader)
--surface:set_position(0,-1,0)
surface:set_scale(1,0.1,1)

window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)
box:set_shader(shader)

while window:render_scene{box, surface} == "continue" do end
 ]]--