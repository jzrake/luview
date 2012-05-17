
local lunum = require 'lunum'
local luview = require 'luview'

local window = luview.Window()
local box = luview.BoundingBox()
local func = luview.FunctionMapping()
func:set_transform(function(x,y,z)
		      return x^1.1, y^2.2, z^3.3
		   end)
local f = func:get_transform()
print(f(1.0, 2.0, 3.0))
