
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

local function make_ball(N, mp, R0, V0)
   local r0 = 0.5
   local M = N * mp -- total mass
   for i=0, N do
      local r = (math.random() + 0.1)*r0
      local t = math.acos(2*(math.random() - 0.5))
      local p = math.random()*2*math.pi

      local x0 = r*math.sin(t)*math.cos(p) + R0[1]
      local x1 = r*math.sin(t)*math.sin(p) + R0[2]
      local x2 = r*math.cos(t) + R0[3]

      local Menclosed = mp*r
      local V = 20*(Menclosed/r^1.5)^0.5

      local v0 = (math.random() - 0.5)*V + V0[1]
      local v1 = (math.random() - 0.5)*V + V0[2]
      local v2 = (math.random() - 0.5)*V + V0[3]

      nbody:add_particle(x0,x1,x2, v0,v1,v2, mp)
   end
end

local function make_pair()
   local r = 0.5
   local N = 300
   local mp = 10.0
   local M = N * mp
   local v = 0.5*(M/r)^0.5

   make_ball(N, mp, {0,0,-r}, { v,0,0})
   make_ball(N, mp, {0,0, r}, {-v,0,0})
--   nbody:add_particle(0, 0,-r, v, 0, 0,  M)
--   nbody:add_particle(0, 0, r,-v, 0, 0,  M)
end
--make_pair()
make_ball(500, 10, {0,0,0}, {0,0,0})

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
