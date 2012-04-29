

local luview = require 'luview'
local lunum = require 'lunum'


local window = luview.Window()
local box = luview.BoundingBox()
local tess = luview.Tesselation3D()

window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)
tess:set_color(0.9, 0.2, 0.2)
tess:set_alpha(0.3)
tess:set_linewidth(8.0)
tess:set_scale(0.8, 0.8, 0.8)

while window:render_scene({tess, box}) == "continue" do end
