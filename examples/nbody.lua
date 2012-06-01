
local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local nbody = luview.NbodySimulation()
local pntens = luview.PointsEnsemble()
local shader = shaders.load_shader("multlights")

local positions = nbody:get_output("positions")
local masses = nbody:get_output("masses")

masses:set_normalize(false)

pntens:set_data("points", positions)
pntens:set_data("sizes", masses)
pntens:set_scale(0.6, 0.6, 0.6)
pntens:set_orientation(0, 90, 0)
pntens:set_linewidth(20.0)
pntens:set_color(0.5, 1.0, 0.3)

window:set_color(0,0,0)
box:set_color(0.2, 0.2, 0.2)
box:set_shader(shader)
box:set_linewidth(0.5)

window:set_callback("idle", function() nbody:advance() end)
window:render_scene{box, pntens}
