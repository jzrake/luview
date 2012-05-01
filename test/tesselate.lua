

local luview = require 'luview'
local lunum = require 'lunum'
local utils = require 'test.utils'

local window = luview.Window()
local box = luview.BoundingBox()
local tess = luview.Tesselation3D()

window:set_color(0.2, 0.2, 0.2)
tess:set_color(0.2, 0.8, 0.2)
tess:set_alpha(0.4)
tess:set_linewidth(5.0)


local shade = luview.ShaderProgram()
utils.load_shader("lambertian", shade)
--tess:set_shader(shade)

while window:render_scene({box, tess}) == "continue" do end
