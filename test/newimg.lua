
local lunum = require 'lunum'
local luview = require 'luview'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local shade = shaders.load_shader("phong")
local image = luview.ImagePlane()
local lumsrc = luview.DataSource()
local rgbsrc = luview.DataSource()

window:set_color(0,0,0)
box:set_alpha(1.0)
box:set_linewidth(2.0)
box:set_shader(shade)
box:set_color(0.2, 0.2, 0.0)

local lumdata = lunum.zeros{128,128}
local rgbdata = lunum.zeros{128,128,3}

for i,j in lumdata:indices() do
   lumdata[{i,j}] = 0.5*(i + j) / 128
end
for i,j,k in rgbdata:indices() do
   rgbdata[{i,j,k}] = 0.5*(i + j + 100*k) / 256
end

lumsrc:set_mode("luminance")
lumsrc:set_data(lumdata)

rgbsrc:set_mode("rgb")
rgbsrc:set_data(rgbdata)

image:set_data("image", rgbsrc)
image:set_data("image", lumsrc)

image:set_alpha(0.2)

while window:render_scene{box, image} == "continue" do end
