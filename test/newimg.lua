
local lunum = require 'lunum'
local luview = require 'luview'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local image = luview.ImagePlane()
local lumsrc = luview.DataSource()
local rgbsrc = luview.DataSource()
local lut = luview.DataSource()
local pyluts = luview.MatplotlibColormaps()

local shade = shaders.load_shader("phong")
local cmshade = shaders.load_shader("cbar")

local cbar = require 'cbar'
local lutdata = lunum.array(cbar):resize{256,4}
local program = luview.ShaderProgram()

local vert = [[
void main() {
  gl_Position = ftransform();
}
]]
local frag = [[
void main() {
   gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
]]
program:set_program(vert, frag)

local lumdata = lunum.fromfile("data/rhoJ.bin"):resize{1024,1024}
lumsrc:set_mode("luminance")
lumsrc:set_data(lumdata)
lumsrc:set_program(program)
lumsrc:set_normalize(true)

lut:set_data(lutdata)
lut:set_mode("rgba")

window:set_color(0,0,0)
box:set_alpha(1.0)
box:set_linewidth(2.0)
box:set_shader(shade)
box:set_color(0.2, 0.2, 0.0)

--image:set_data("color_table", lut)

image:set_data("color_table", pyluts)
image:set_data("image", lumsrc)
image:set_alpha(1.0)
image:set_orientation(0, 0, -90)
image:set_shader(cmshade)


window:set_callback("]", function() pyluts:next_colormap() end, "next colormap")
window:set_callback("[", function() pyluts:prev_colormap() end, "previous colormap")

while window:render_scene{box, image} == "continue" do end
