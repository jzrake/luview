
local luview = require 'luview'

local function load_shader(name)
   local shade = luview.ShaderProgram()
   local shader_dir = os.getenv("LUVIEW_HOME") .. "/shaders/"

   local vert_fname = shader_dir..name..".vert"
   local frag_fname = shader_dir..name..".frag"

   local vert = io.open(vert_fname, "r"):read("*all")
   local frag = io.open(frag_fname, "r"):read("*all")

   shade:set_program(vert, frag)
   return shade
end

return {
   load_shader=load_shader
}
