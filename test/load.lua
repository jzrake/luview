
local lunum = require 'lunum'
local luview = require 'luview'


local window = luview.Window()
local bounding_box = luview.BoundingBox()
local little_box = luview.BoundingBox()
--local nurbs = luview.ExampleSimpleNURBS()
local nurbs = luview.SurfaceNURBS()
local evaluator = luview.ExampleSimpleEvaluator()
local surface = luview.SurfaceEvaluator()


local function MakeSurfaceData()
   local Nx = 64
   local Ny = 64
   local surfdata = lunum.zeros{Nx,Ny}
   for i,j in surfdata:indices() do
      local x = i / Nx - 0.5
      local y = j / Ny - 0.5
      surfdata[{i,j}] = 10*(x^4 + x*y^3)
   end
   return surfdata
end


nurbs:set_data(MakeSurfaceData())


--surface:set_data(surfdata)
surface:set_position(0,-1,0)

evaluator:set_color(1.0, 0.5, 0.0)
evaluator:set_linewidth(1.0)



bounding_box:set_linewidth(2.5)
bounding_box:set_color(0.1, 0.1, 0.6)

little_box:set_scale(0.2, 0.5, 0.5)
little_box:set_orientation(0.0, 20.0, 0.0)
little_box:set_color(1.0, 0.2, 0.1)


window:set_orientation(45,0,0)
window:set_position(0.0, 0.0, -3.0)
window:set_color(0.95, 0.95, 0.95)
window:render_scene({bounding_box, nurbs})
