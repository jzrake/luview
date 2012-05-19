


local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local volume = luview.VolumeRendering()
local shader = shaders.load_shader("lambertian")


window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)
box:set_shader(shader)
--volume:set_scale(0.8, 0.8, 0.8)

local cycle = 0
while window:render_scene{volume} == "continue" do
   cycle = cycle + 1
end
