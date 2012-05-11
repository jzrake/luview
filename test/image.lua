


local luview = require 'luview'
local lunum = require 'lunum'
local utils = require 'test.utils'

local window = luview.Window()
local scolors = luview.FunctionMapping()
local image_src = luview.MultiImageSource()
local image = luview.ImagePlane()
local normalize = luview.GlobalLinearTransformation()
local cmap = luview.ColorMaps()
local rho = lunum.fromfile("/Users/jzrake/Work/luview/rho.bin"):reshape{1024,1024}

image_src:set_array(rho['::2,::2'])
normalize:set_input(image_src)
normalize:set_range(0, 0.0, 1.0)
scolors:set_input(normalize)
scolors:set_transform(cmap)
image:set_data("rgba", scolors)
window:set_color(0.2, 0.2, 0.2)
window:set_orientation(0,0,0)
window:set_position(0,0,-1.4)
while window:render_scene({image}) == "continue" do end
