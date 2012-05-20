


local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local scolors = luview.MultiImageSource()
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

  gl_FragColor = vec4((1.0-cos(0.02*float(n)))/2.0,
                      (1.0-sin(0.12*float(n)))/2.0,
                      (1.0-cos(0.18*float(n)))/2.0,
		      1.0);
}
]], zoom, x0, y0)
end

local coords = lunum.zeros{8,8,4}
scolors:set_array(coords)

image:set_data("rgba", scolors)
image:set_orientation(0,0,-90)
image:set_shader(shade)
window:set_color(0,0,0)
window:set_orientation(0,0,0)
window:set_position(0,0,-0.9)


local params = { z=3, x=0, y=0 }
shade:set_program(vert, frag(params))

window:set_callback("a", function() params.x = params.x + 0.03*params.z end, "move left")
window:set_callback("d", function() params.x = params.x - 0.03*params.z end, "move right")
window:set_callback("s", function() params.y = params.y + 0.03*params.z end, "move down")
window:set_callback("w", function() params.y = params.y - 0.03*params.z end, "move up")
window:set_callback("-", function() params.z = params.z * 1.1 end, "zoom out")
window:set_callback("+", function() params.z = params.z / 1.1 end, "zoom in")
window:set_callback("idle", function() shade:set_program(vert, frag(params)) end)

while window:render_scene{image} == "continue" do
   shade:set_program(vert, frag(params))
end
