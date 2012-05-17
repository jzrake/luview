
local luview = require 'luview'
local lunum = require 'lunum'
local utils = require 'test.utils'

local window = luview.Window()
local box = luview.BoundingBox()
--local colors = luview.FunctionMapping()
local nbody = luview.NbodySimulation()
local shader = luview.ShaderProgram()
local pntens = luview.PointsEnsemble()
utils.load_shader("lambertian", shader)


--colors:set_input(points)
--colors:set_transform(cmap2)

pntens:set_data("points", nbody)
--pntens:set_data("colors", colors)
--pntens:set_position(-0.5, -0.5, 0.5)
pntens:set_scale(0.5, 0.5, 0.5)
pntens:set_orientation(0, 90, 0)
pntens:set_linewidth(16.0)
pntens:set_alpha(0.5)

window:set_color(0.05, 0.1, 0.1)
box:set_color(0.5, 0.9, 0.9)
box:set_shader(shader)

local status = "continue"
local key = ''
local lambda = 3.6

while status == "continue" do
   status, key = window:render_scene({box, pntens})
   nbody:advance()
   if key == 'l' then


   end
   if key == 'L' then


   end
end
