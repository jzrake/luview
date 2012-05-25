


local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local lumsrc = luview.DataSource()
local image = luview.ImagePlane()
local shade = luview.ShaderProgram()

local vert = [[
varying vec4 pos;
void main() {
  gl_Position = ftransform();
  pos = gl_ModelViewMatrix * gl_Vertex;
}
]]

local function frag(params)
   local zoom, x0, y0 = params.z, params.x, params.y
   local rf, gf, bf = params.r, params.g, params.b
   return string.format([[
varying vec4 pos;
void main() {
  vec2 z0 = vec2(0,0);
  vec2 z1 = z0;
  vec2 c = %f*(pos.xy) - vec2(%f, %f);
  int n = 0;

  while (z0.x*z0.x + z0.y*z0.y < 4.0 && n < 500) {
     z1.x = z0.x*z0.x - z0.y*z0.y + c.x;
     z1.y = 2.0*z0.x*z0.y + c.y;
     z0 = z1;
     ++n;
  }

  gl_FragColor = vec4((1.0-cos(%f*float(n)))/2.0,
                      (1.0-cos(%f*float(n)))/2.0,
                      (1.0-cos(%f*float(n)))/2.0,
		      1.0);
}
]], zoom, x0, y0, rf, gf, bf)
end

local coords = lunum.zeros{8,8,4}
scolors:set_array(coords)

image:set_data("rgba", scolors)
image:set_orientation(0,0,-90)
image:set_shader(shade)
window:set_color(0,0,0)
window:set_orientation(0,0,0)
window:set_position(0,0,-0.9)


local p = { z=3, x=0, y=0, r=0.1, g=0.08, b=0.10 }
shade:set_program(vert, frag(p))

window:set_callback("P", function() for k,v in pairs(p) do print(k,v) end end,
		    "print color params")
window:set_callback("a", function() p.x = p.x + 0.03*p.z end, "move left")
window:set_callback("d", function() p.x = p.x - 0.03*p.z end, "move right")
window:set_callback("s", function() p.y = p.y + 0.03*p.z end, "move down")
window:set_callback("w", function() p.y = p.y - 0.03*p.z end, "move up")
window:set_callback("R", function() p.r = p.r + 0.01 end, "increase red frequency")
window:set_callback("G", function() p.g = p.g + 0.01 end, "increase green frequency")
window:set_callback("B", function() p.b = p.b + 0.01 end, "increase blue frequency")
window:set_callback("r", function() p.r = p.r - 0.01 end, "decrease red frequency")
window:set_callback("g", function() p.g = p.g - 0.01 end, "decrease green frequency")
window:set_callback("b", function() p.b = p.b - 0.01 end, "decrease blue frequency")
window:set_callback("-", function() p.z = p.z * 1.1 end, "zoom out")
window:set_callback("+", function() p.z = p.z / 1.1 end, "zoom in")
window:set_callback("idle", function() shade:set_program(vert, frag(p)) end)

while window:render_scene{image} == "continue" do end
