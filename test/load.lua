

local luview = require 'luview'


print(luview.Window)
local window = luview.Window()
local bounding_box = luview.BoundingBox()

window:render_scene({bounding_box})


