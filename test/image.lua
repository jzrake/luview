


local luview = require 'luview'
local lunum = require 'lunum'
local utils = require 'test.utils'

local window = luview.Window()
local box = luview.BoundingBox()
local grid2d = luview.GridSource2D()
local surface = luview.SurfaceNURBS()
local ctrlpnt = luview.FunctionMapping()
local scolors = luview.FunctionMapping()
local image_src = luview.MultiImageSource()
local image = luview.ImagePlane()
local shader = luview.ShaderProgram()
utils.load_shader("lambertian", shader)


local function normalize_input(f)
   local function g(z,caller)
--      print(z,caller)
      if caller ~= nil then
         local zmin=caller:get_info("min0")
         local zmax=caller:get_info("max0")
         z = (z - zmin) / (zmax - zmin)
      end
      return f(z)
   end
   return g
end

local time = 0.0
local function stingray(u,v)
   return u, v, 6*(u^4 + u*v^3) * math.cos(20*u*v) * math.cos(time)
end

local function eggcrate(u,v)
   local sin = math.sin
   local cos = math.cos
   return u, v, 0.2 * math.sin(20*u) * cos(20*v)
end

local function cmap1(z)
   return z,z,z,0.9
end

local function cmap2(z)
   local a = 100.0
   local b = 2
--   print(z)
   return math.exp(-a*(z-0.3)^b), math.exp(-a*(z-0.5)^b), math.exp(-a*(z-0.7)^b), 0.8
end

local rho = lunum.fromfile("/Users/jzrake/Work/luview/rho.bin"):reshape{1024,1024}
image_src:set_array(rho)

--grid2d:set_num_points(128, 128)
--ctrlpnt:set_input(grid2d)
--ctrlpnt:set_transform(stingray)
--scolors:set_input(ctrlpnt)
scolors:set_input(image_src)
scolors:set_transform(normalize_input(cmap2))
image:set_data("rgba", scolors)

window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)

local cycle = 0
while window:render_scene({image}) == "continue" do end
