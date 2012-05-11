

local luview = require 'luview'


local function load_shader(name, shade)
   local vert_fname = "/Users/jzrake/Work/luview/shaders/"..name..".vert"
   local frag_fname = "/Users/jzrake/Work/luview/shaders/"..name..".frag"

   local vert = io.open(vert_fname, "r"):read("*all")
   local frag = io.open(frag_fname, "r"):read("*all")

   shade:set_program(vert, frag)
   return shade
end


return {
   normalize_input=normalize_input,
   load_shader=load_shader
}
