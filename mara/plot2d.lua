

local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local image_src = luview.MultiImageSource()
local normalize = luview.GlobalLinearTransformation()
--local cmap = luview.TessColormaps()
local cmap = luview.MatplotlibColormaps()
local scolors = luview.FunctionMapping()
local cmshade = shaders.load_shader("cbar")


local function load_frame(fname)
   collectgarbage()
   h5_open_file(fname, "r")

   local dsets = { "rho", "pre", "vx", "vy"}--, "vz" }
   local shape = h5_get_ndims("prim/"..dsets[1])
   local data = lunum.zeros{shape[0], shape[1], #dsets}

   for nd,d in pairs(dsets) do
      local dsetname = "prim/"..d
      local var = h5_read_array(dsetname)
      print("loading "..dsetname)
      for i=0,shape[0]*shape[1]-1 do
         data[i*#dsets + nd-1] = var[i]
      end
      normalize:set_range(nd-1, 0.0, 1.0)
   end

   h5_close_file()

   image_src:set_array(data)
   normalize:set_input(image_src)

--   scolors:set_input(normalize)
--   scolors:set_transform(cmap)

   local image = luview.ImagePlane()
--   image:set_data("rgba", scolors)
   image:set_data("rgba", normalize)
   image:set_orientation(0,0,-90)
   image:set_shader(cmshade)
   return image
end


window:set_color(0,0,0)
window:set_orientation(0,0,0)
window:set_position(0,0,-1.3)


local auto_movie = false
local narg = 1
local status = "continue"
local key
local actors = { image }
actors[1] = load_frame(cmdline.args[narg])
cmap:set_colormap(4)

while status == "continue" do
   status, key = window:render_scene(actors)

   if key == 'j' then
      cmap:prev_colormap()
      actors[1]:stage()
   elseif key == 'k' then
      cmap:next_colormap()
      actors[1]:stage()
   elseif key == 'v' then
      cmap_comp = cmap_comp + 1
      if cmap_comp == #dsets then cmap_comp = 0 end
      print("showing variable "..dsets[cmap_comp+1])
      cmap:set_component(cmap_comp)
      actors[1]:stage()
   elseif key == 'n' then
      narg = narg + 1
      local fname = cmdline.args[narg]
      if not fname then break end
      actors[1] = load_frame(fname)
      actors[1]:stage()
   end

   if auto_movie then
      narg = narg + 1
      local fname = cmdline.args[narg]
      if not fname then break end
      actors[1] = load_frame(fname)
      actors[1]:stage()
      window:print_screen("img/KH-smooth")
   end
end
