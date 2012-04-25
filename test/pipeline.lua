


local lunum = require 'lunum'
local luview = require 'luview'




local window = luview.Window()
local grid2d = luview.GridSource2D()
local ctrlpnt = luview.FunctionMapping()
local box = luview.BoundingBox()
local surface = luview.SurfaceNURBS()


ctrlpnt:set_input(grid2d)
ctrlpnt:set_transform(function(u,v) return u,v,u^2 + v^2 end)

surface:set_data("control_points", ctrlpnt)
surface:set_color(0.2, 0.8, 0.4)

box:set_color(0.5, 0.9, 0.9)

window:set_color(0.2, 0.2, 0.2)
window:set_position(0, 0, -2.0)

while window:render_scene({box, surface}) == "continue" do end
