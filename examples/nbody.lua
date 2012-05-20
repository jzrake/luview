


local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local nbody = luview.NbodySimulation()
local pntens = luview.PointsEnsemble()
local shader = shaders.load_shader("lambertian")

nbody:advance()

pntens:set_data("points", nbody:get_output())
pntens:set_scale(0.6, 0.6, 0.6)
pntens:set_orientation(0, 90, 0)
pntens:set_linewidth(16.0)
pntens:set_color(0.5, 1.0, 0.3)
pntens:set_alpha(1)

window:set_color(0,0,0)
box:set_color(0.7, 0.9, 0.9)
box:set_shader(shader)
box:set_alpha(0.9)

local status = "continue"
local key = ''
local lambda = 3.6

while status == "continue" do
   status, key = window:render_scene({box, pntens})
   nbody:advance()
end
