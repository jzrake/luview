


local luview = require 'luview'
local lunum = require 'lunum'
local utils = require 'test.utils'

local window = luview.Window()
local scolors = luview.FunctionMapping()
local image_src = luview.MultiImageSource()
local image = luview.ImagePlane()
local normalize = luview.GlobalLinearTransformation()
local cmap = luview.ColorMaps()


local rhoJ = lunum.fromfile("/Users/jzrake/Work/luview/data/rhoJ.bin"):reshape{1024,1024}
local rhoZ = lunum.fromfile("/Users/jzrake/Work/luview/data/rhoZ.bin"):reshape{1024,1024}
local rho = lunum.zeros{1024,1024}

for m=0,1024*1024-1 do
   local j = m / 1024
   rho[m] = j < 512 and rhoJ[m] or rhoZ[m]
end


image_src:set_array(rho['::1,::1'])
normalize:set_input(image_src)
normalize:set_range(0, 0.0, 1.0)
scolors:set_input(normalize)
scolors:set_transform(cmap)
image:set_data("rgba", scolors)
image:set_orientation(0,0,-90)
window:set_color(0.2, 0.2, 0.2)
window:set_orientation(0,0,0)
window:set_position(0,0,-1.4)

local status = "continue"
local key
local actors = { image }

while status == "continue" do
   status, key = window:render_scene(actors)
   if tonumber(key) then
      cmap:set_cmap(tonumber(key))
      actors[1] = luview.ImagePlane()
      actors[1]:set_data("rgba", scolors)
      actors[1]:set_orientation(0,0,-90)
   end
end
