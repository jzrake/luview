

local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local lumsrc = luview.DataSource()
local pyluts = luview.MatplotlibColormaps()
local cmshade = shaders.load_shader("cbar")


local function load_frame(fname, var)
   local image = luview.ImagePlane()

   collectgarbage()
   h5_open_file(fname, "r")
   local lumdata = h5_read_array("prim/"..var)
   local Nx, Ny = unpack(lumdata:shape())
   h5_close_file()

   lumsrc:set_mode("luminance")
   lumsrc:set_data(lumdata)
   lumsrc:set_normalize(true)

   image:set_data("image", lumsrc)
   image:set_data("color_table", pyluts)
   image:set_alpha(1.0)
   image:set_orientation(0,0,-90)
   image:set_scale(-1, Nx/Ny, 1)
   image:set_shader(cmshade)

   collectgarbage()
   return image
end


window:set_color(0,0,0)
window:set_orientation(0,0,0)
window:set_position(0,0,-1.3)

local auto_movie = false
local narg = 1
local status = "continue"
local actors = { }

actors[1] = load_frame(cmdline.args[narg], "rho")
pyluts:set_colormap("afmhot")

local function next_frame()
   narg = narg + 1
   local fn = cmdline.args[narg]
   if not fn then os.exit() end
   actors[1] = load_frame(fn, "rho")
end
local function idle()
   if auto_movie then
      window:print_screen("img/screen")
      next_frame()
   end
end

window:set_callback("]", function() pyluts:next_colormap() end, "next colormap")
window:set_callback("[", function() pyluts:prev_colormap() end, "previous colormap")
window:set_callback("n", next_frame, "next frame")
window:set_callback("idle", idle)

while window:render_scene(actors) == "continue" do end
