

local luview = require 'luview'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local tess = luview.Tesselation3D()
local segments = luview.SegmentsEnsemble()
local triangles = luview.TrianglesEnsemble()

local shade = shaders.load_shader("lambertian")
local phong = shaders.load_shader("phong")
local lights = shaders.load_shader("multlights")


tess:load_node("data/brain")
--tess:load_poly("data/CutSphere")

segments:set_data("segments", tess:get_output("segments"))
segments:set_orientation(-90,0,0)
segments:set_color(0,0,0)
segments:set_linewidth(0.02)
segments:set_scale(1.01, 1.01, 1.01)

triangles:set_data("triangles", tess:get_output("triangles"))
triangles:set_shader(lights)
triangles:set_alpha(1.0)
triangles:set_color(0.5, 0.5, 1.0)
triangles:set_orientation(-90,0,0)
box:set_shader(shade)
window:set_color(0.2, 0.2, 0.2)

window:set_callback("]", function() tess:load_poly("data/CutSphere") end)
while window:render_scene{box, triangles} == "continue" do end
