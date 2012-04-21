
local lunum = require 'lunum'
local luview = require 'luview'


local window = luview.Window()
local bounding_box = luview.BoundingBox()
local little_box = luview.BoundingBox()
local surface = luview.SurfaceNURBS()


local function MakeSurfaceData(t)
   local Nx = 32
   local Ny = 32
   local surfdata = lunum.zeros{Nx,Ny}
   for i,j in surfdata:indices() do
      local x = i / Nx - 0.5
      local y = j / Ny - 0.5
      surfdata[{i,j}] = 5*(x^4 + x*y^3) * math.cos(t) * math.cos(20*x*y)
   end
   return surfdata
end


surface:set_data(MakeSurfaceData(0.0))
surface:set_position(0,0,0)

bounding_box:set_linewidth(5.5)
bounding_box:set_color(0.1, 0.8, 0.6)

little_box:set_scale(0.2, 0.5, 0.5)
little_box:set_orientation(0.0, 20.0, 0.0)
little_box:set_color(1.0, 0.2, 0.1)
little_box:set_position(0, 0.5, 0)

window:set_orientation(45,0,0)
window:set_position(0.0, 0.0, -2)
window:set_color(0.7, 0.95, 0.95)


local scene = {bounding_box, little_box, surface}
local time = 0.0

while window:render_scene(scene) == "continue" do
   time = time + 0.05
   surface:set_data(MakeSurfaceData(time))
   collectgarbage()
end
