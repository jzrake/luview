

local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local tess = luview.Tesselation3D()
local segments = luview.SegmentsEnsemble()

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


segments:set_data("segments", tess)
segments:set_shader(shade)
segments:set_alpha(1.0)
segments:set_color(0.7, 0.8, 0.9)
segments:set_linewidth(0.1)
box:set_shader(shade)
window:set_color(0.2, 0.2, 0.2)

while window:render_scene{box, segments} == "continue" do end
