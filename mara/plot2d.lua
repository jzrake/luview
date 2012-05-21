

local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local image_src = luview.MultiImageSource()
local normalize = luview.GlobalLinearTransformation()
local cmap = luview.MatplotlibColormaps()
local cmshade = shaders.load_shader("cbar")


normalize:set_input(image_src)
normalize:set_range(0, 0.0, 1.0)
cmap:set_colormap(4)


local function load_frame(fname, var)
   local image = luview.ImagePlaneGpuShaded()

   collectgarbage()
   h5_open_file(fname, "r")
   local data = h5_read_array("prim/"..var)
   h5_close_file()

   image_src:set_array(data)
   image:set_data("data", normalize)
   image:set_data("color_table", cmap:get_output())

   image:set_orientation(0,0,-90)
   image:set_shader(cmshade)
   collectgarbage()
   return image
end


window:set_color(0,0,0)
window:set_orientation(0,0,0)
window:set_position(0,0,-1.3)

local auto_movie = true
local narg = 1
local status = "continue"
local key
local actors = { image }

actors[1] = load_frame(cmdline.args[narg], "rho")

local function next_frame()
   narg = narg + 1
   local fn = cmdline.args[narg]
   if not fn then os.exit() end
   actors[1] = load_frame(fn, "rho")
end
local function idle()
   if auto_movie then
--      window:print_screen("img/screen")
      next_frame()
   end
end

window:set_callback("]", function() cmap:next_colormap() end, "next colormap")
window:set_callback("[", function() cmap:prev_colormap() end, "previous colormap")
window:set_callback("n", next_frame, "next frame")
window:set_callback("idle", idle)

while window:render_scene(actors) == "continue" do end
