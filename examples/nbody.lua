


local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local nbody = luview.NbodySimulation()
local pntens = luview.PointsEnsemble()
local shader = shaders.load_shader("lambertian")

pntens:set_data("points", nbody)
pntens:set_scale(0.5, 0.5, 0.5)
pntens:set_orientation(0, 90, 0)
pntens:set_linewidth(16.0)
pntens:set_color(0.8, 0.1, 0.9)
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
end
