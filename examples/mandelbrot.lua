


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

local function frag(zoom, x0, y0)
   return string.format([[
varying vec4 pos;
void main() {
  vec2 z0 = vec2(0,0);
  vec2 z1 = z0;
  vec2 c = %f*(pos.xy) - vec2(%f, %f);
  int n = 0;

  while (z0.x*z0.x + z0.y*z0.y < 4.0 && n < 250) {
     z1.x = z0.x*z0.x - z0.y*z0.y + c.x;
     z1.y = 2.0*z0.x*z0.y + c.y;
     z0 = z1;
     ++n;
  }
  
  gl_FragColor = vec4((-cos(0.02*float(n))+1.0)/2.0,
                      (-sin(0.24*float(n))+1.0)/2.0,
                      (-cos(0.12*float(n))+1.0)/2.0,
		      1.0);
}
]], zoom, x0, y0)
end

shade:set_program(vert, frag(3.0, 0.1, 0.0))

local coords = lunum.zeros{512,512,4}
scolors:set_array(coords)

image:set_data("rgba", scolors)
image:set_orientation(0,0,-90)
image:set_shader(shade)
window:set_color(0.2, 0.2, 0.2)
window:set_orientation(0,0,0)
window:set_position(0,0,-1.4)

local status = "continue"
local key
local iter = 0

local params = { z=3, x=0, y=0 }

while status == "continue" do
   status, key = window:render_scene{image}

   if key == '[' then params.x = params.x + 0.01*params.z end
   if key == ']' then params.x = params.x - 0.01*params.z end
   if key == 'm' then params.y = params.y + 0.01*params.z end
   if key == 'k' then params.y = params.y - 0.01*params.z end
   if key == '-' then params.z = params.z * 1.1 end
   if key == '+' then params.z = params.z / 1.1 end

   shade:set_program(vert, frag(params.z, params.x, params.y))
   iter = iter + 1
end
