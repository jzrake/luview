

local luview = require 'luview'
local lunum = require 'lunum'


local window = luview.Window()
local box = luview.BoundingBox()


local grid2d = luview.GridSource2D()
local ctrlpnt = luview.FunctionMapping()

ctrlpnt:set_input(grid2d)
ctrlpnt:set_transform(function(u,v) return u, v, -4*u^2+v^2 end)

local surface = luview.SurfaceNURBS()
surface:set_data("control_points", ctrlpnt)
surface:set_color(0.2, 0.8, 0.4)
surface:set_alpha(0.7)

box:set_color(0.5, 0.9, 0.9)

window:set_color(0.2, 0.2, 0.2)
window:set_position(0, 0, -2.0)

local cycle = 0
while window:render_scene({box, surface}) == "continue" do
   cycle = cycle + 1
   ctrlpnt:set_transform(function(u,v) return
			    u,
			    v,
			    (-4*u^2+v^2*math.sin(cycle*0.01)) end)
end
