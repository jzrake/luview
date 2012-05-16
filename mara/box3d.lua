

local luview = require 'luview'
local lunum = require 'lunum'
local util = require 'util'

local window = luview.Window()
local cmap = luview.ColorMaps()
local box = luview.BoundingBox()


h5_open_file(cmdline.args[1], "r")

image = { }
image_src = { }
normalize = { }
scolors = { }

for _,v in pairs({"x", "y", "z"}) do
   local data = h5_read_array("cutplanes/"..v.."_face/pre")

   image_src[v] = luview.MultiImageSource()
   normalize[v] = luview.GlobalLinearTransformation()
   scolors[v] = luview.FunctionMapping()
   image[v] = luview.ImagePlane()

   image_src[v]:set_array(data)
   normalize[v]:set_input(image_src[v])
   normalize[v]:set_range(0, 0.0, 1.0)
   scolors[v]:set_input(normalize[v])
   scolors[v]:set_transform(cmap)
   image[v]:set_data("rgba", scolors[v])
   image[v]:set_alpha(1.0)
end

h5_close_file()

image["x"]:set_position(-0.5,  0.0,  0.0)
image["y"]:set_position( 0.0, -0.5,  0.0)
image["z"]:set_position( 0.0,  0.0, -0.5)

image["x"]:set_orientation(0,-90,0)
image["y"]:set_orientation(90,0,90)
image["z"]:set_orientation(0,0,90)

local light_inside = true
local function setup_light()
   if light_inside then
      image["x"]:set_scale(1,1,-1) -- puts the front face inside the box
      image["y"]:set_scale(1,1,-1)
      image["z"]:set_scale(1,1, 1)
   else
      image["x"]:set_scale(1,1, 1)
      image["y"]:set_scale(1,1, 1)
      image["z"]:set_scale(1,1,-1)
   end
end
setup_light(light_inside)

window:set_color(0.1, 0.1, 0.1)
window:set_orientation(0,0,0)
box:set_linewidth(2)
box:set_color(0,0,0)

local status = "continue"
local key
local actors = { image["x"], image["y"], image["z"], box }

while status == "continue" do
   status, key = window:render_scene(actors)
   if tonumber(key) then
      cmap:set_cmap(tonumber(key))
      for _,v in pairs({"x", "y", "z"}) do image[v]:stage() end
   elseif key == 'l' then
      light_inside = not light_inside
      setup_light()
      for _,v in pairs({"x", "y", "z"}) do image[v]:stage() end
   end
end

