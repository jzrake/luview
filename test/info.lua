
local luview = require 'luview'
local window = luview.Window()
local info = luview.GpuInformation()

info:print()
info:ext_supported("thing")
