
local luview = require 'luview'
local window = luview.Window()
local info = luview.GpuInformation()

info:print()
print(info:ext_supported("GL_EXT_framebuffer_object"))
