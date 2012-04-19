

local luview = require 'luview'


print(luview.Window)
local window = luview.Window()
local bounding_box = luview.BoundingBox()

window:set_color(0.1, 0.05, 0.3)
window:render_scene({bounding_box})

