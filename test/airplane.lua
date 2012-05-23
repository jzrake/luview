

local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local tess = luview.Tesselation3D()
local segments = luview.SegmentsEnsemble()
local triangles = luview.TrianglesEnsemble()

local pointlist = lunum.zeros{8,3}
local n

n = 0
pointlist[{n,0}] = 0.0
pointlist[{n,1}] = 1.0
pointlist[{n,2}] = 0.0

n = 1
pointlist[{n,0}] =-0.5
pointlist[{n,1}] = 0.0
pointlist[{n,2}] = 0.0

n = 2
pointlist[{n,0}] = 0.5
pointlist[{n,1}] = 0.0
pointlist[{n,2}] = 0.0

n = 3
pointlist[{n,0}] = 0.0
pointlist[{n,1}] = 0.25
pointlist[{n,2}] = 1.0

n = 4
pointlist[{n,0}] = 0.0
pointlist[{n,1}] = 0.25
pointlist[{n,2}] =-1.0

n = 5
pointlist[{n,0}] = 0.0
pointlist[{n,1}] =-0.25
pointlist[{n,2}] =-1.0

n = 6
pointlist[{n,0}] =-0.5
pointlist[{n,1}] = 0.25
pointlist[{n,2}] =-1.0

n = 7
pointlist[{n,0}] = 0.5
pointlist[{n,1}] = 0.25
pointlist[{n,2}] =-1.0


local shade = shaders.load_shader("lambertian")
local phong = shaders.load_shader("phong")
local lights = shaders.load_shader("multlights")




triangles:set_data("triangles", tess:get_output("triangles"))
triangles:set_shader(lights)
triangles:set_alpha(1.0)
triangles:set_color(0.5, 0.5, 1.0)
triangles:set_scale(0.04, 0.04, 0.04)
triangles:set_orientation(-90,0,0)
box:set_shader(shade)
window:set_color(0.2, 0.2, 0.2)

while window:render_scene{box, triangles} == "continue" do end
