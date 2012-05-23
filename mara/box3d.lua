
local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local pyluts = luview.MatplotlibColormaps()
local box = luview.BoundingBox()
local cmshade = shaders.load_shader("cbar")
local boxshader = shaders.load_shader("lambertian")


local images = { }
local light_inside = true


local function load_frame(fname, dir, var)
   local image = luview.ImagePlane()
   local lumsrc = luview.DataSource()

   collectgarbage()
   h5_open_file(fname, "r")
   local lumdata = h5_read_array("cutplanes/"..dir.."_face/"..var)
   h5_close_file()

   lumsrc:set_mode("luminance")
   lumsrc:set_data(lumdata)
   lumsrc:set_normalize(true)

   image:set_data("image", lumsrc)
   image:set_data("color_table", pyluts)
   image:set_alpha(1.0)
   image:set_shader(cmshade)

   return image
end

local function setup_planes()
   if light_inside then
      images["x"]:set_scale(1,1,-1) -- puts the front face inside the box
      images["y"]:set_scale(1,1,-1)
      images["z"]:set_scale(1,1, 1)
   else
      images["x"]:set_scale(1,1, 1)
      images["y"]:set_scale(1,1, 1)
      images["z"]:set_scale(1,1,-1)
   end

   images["x"]:set_position(-0.5,  0.0,  0.0)
   images["y"]:set_position( 0.0, -0.5,  0.0)
   images["z"]:set_position( 0.0,  0.0, -0.5)

   images["x"]:set_orientation( 0,-90,  0)
   images["y"]:set_orientation(90,  0, 90)
   images["z"]:set_orientation( 0,  0, 90)
end
local function switch_light()
   light_inside = not light_inside
   setup_planes()
   stage() 
end

for _,d in pairs{"x","y","z"} do
   images[d] = load_frame(cmdline.args[1], d, "rho")
end
setup_planes()


window:set_color(0,0,0)
window:set_orientation(0,0,0)
box:set_linewidth(0.8)
box:set_color(1,0,0)
box:set_shader(boxshader)
box:set_alpha(1.0)

local actors = { images["x"], images["z"], images["y"], box }

window:set_callback("]", function() pyluts:next_colormap() end, "next colormap")
window:set_callback("[", function() pyluts:prev_colormap() end, "previous colormap")
window:set_callback("l", switch_light, "switch lighting")

while window:render_scene(actors) == "continue" do end
