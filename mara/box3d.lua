
package.path = package.path .. ";" .. os.getenv("LUA_PATH")

local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
--local cmap = luview.TessColormaps()
local cmap = luview.MatplotlibColormaps()
local box = luview.BoundingBox()
local shader = shaders.load_shader("lambertian")

h5_open_file(cmdline.args[1], "r")

local image = { }
local image_src = { }
local normalize = { }
local scolors = { }

local dsets = {"Bx"}--"rho", "pre", "vx", "vy", "vz" }
local all_data = { }

for _,v in pairs({"x", "y", "z"}) do
   local Nq = #dsets

   local shape = h5_get_ndims("cutplanes/"..v.."_face/"..dsets[1])
   all_data[v] = lunum.zeros{shape[0], shape[1], Nq}

   image_src[v] = luview.MultiImageSource()
   normalize[v] = luview.GlobalLinearTransformation()
   scolors[v] = luview.FunctionMapping()
   image[v] = luview.ImagePlane()

   for nd,d in pairs(dsets) do
      local dsetname = "cutplanes/"..v.."_face/"..d
      local var = h5_read_array(dsetname)
      print("loading "..dsetname)

      for i=0,shape[0]*shape[1]-1 do
	 all_data[v][i*Nq + nd-1] = var[i]
      end
      normalize[v]:set_range(nd-1, 0.0, 1.0)
   end

   image_src[v]:set_array(all_data[v])
   normalize[v]:set_input(image_src[v])
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

local cmap_comp = 0
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

window:set_color(0,0,0)
window:set_orientation(0,0,0)
box:set_linewidth(0.6)
box:set_color(1,0,0)
box:set_shader(shader)
box:set_alpha(1)

local status = "continue"
local key
local actors = { image["z"], image["x"], image["y"], box }

while status == "continue" do
   status, key = window:render_scene(actors)
   if tonumber(key) then
      cmap:set_cmap(tonumber(key))
      for _,v in pairs({"x", "y", "z"}) do image[v]:stage() end
   elseif key == 'k' then
      cmap:next_colormap()
      for _,v in pairs({"x", "y", "z"}) do image[v]:stage() end
   elseif key == 'j' then
      cmap:prev_colormap()
      for _,v in pairs({"x", "y", "z"}) do image[v]:stage() end
   elseif key == 'v' then
      cmap_comp = cmap_comp + 1
      if cmap_comp == #dsets then cmap_comp = 0 end
      print("showing variable "..dsets[cmap_comp+1])
      cmap:set_component(cmap_comp)
      for _,v in pairs({"x", "y", "z"}) do image[v]:stage() end
   elseif key == 'l' then
      light_inside = not light_inside
      setup_light()
      for _,v in pairs({"x", "y", "z"}) do image[v]:stage() end
   end
end

