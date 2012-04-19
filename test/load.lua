

local luview = require 'luview'


print(luview.Window)
local window = luview.Window()
local bounding_box = luview.BoundingBox()

local little_box = luview.BoundingBox()

bounding_box:set_linewidth(2.5)
bounding_box:set_color(0.8, 0.9, 0.3)

little_box:set_scale(0.2, 0.5, 0.5)
little_box:set_orientation(0.0, 20.0, 0.0)

window:set_color(0.1, 0.05, 0.1)
window:render_scene({bounding_box, little_box})

