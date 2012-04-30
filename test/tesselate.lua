

local luview = require 'luview'
local lunum = require 'lunum'


local window = luview.Window()
local box = luview.BoundingBox()
local tess = luview.Tesselation3D()

window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)
tess:set_color(0.9, 0.2, 0.2)
tess:set_alpha(0.3)
tess:set_linewidth(4.0)
tess:set_scale(0.8, 0.8, 0.8)

local vert_fname = "/Users/jzrake/Work/luview/test.vert"
local frag_fname = "/Users/jzrake/Work/luview/test.frag"

local vert = io.open(vert_fname, "r"):read("*all")
local frag = io.open(frag_fname, "r"):read("*all")

local shade = luview.ShaderProgram()
shade:set_program(vert, frag)
tess:set_shader(shade)

while window:render_scene({tess, box}) == "continue" do end
