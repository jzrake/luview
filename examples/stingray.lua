


local luview = require 'luview'
local lunum = require 'lunum'
local shaders = require 'shaders'

local window = luview.Window()
local box = luview.BoundingBox()
local height = luview.DataSource()
local verts = luview.ParametricVertexSource3D()
local triangles = luview.TrianglesEnsemble()
local points = luview.DataSource()
local surfshd = luview.ShaderProgram()
local pyluts = luview.MatplotlibColormaps()
local lights = shaders.load_shader("multlights")

local time = 0.0
local function stingray(u,v)
   return 6*(u^4 + u*v^3) * math.cos(20*u*v) * math.cos(time)
end


local vert =
[[
varying vec3 normal;
varying vec4 eyepos;

void main() {
  normal = gl_NormalMatrix * gl_Normal;
  eyepos = gl_ModelViewMatrix * gl_Vertex;
  gl_FrontColor = gl_Color;
  gl_BackColor = gl_Color;
  gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
  gl_Position = ftransform();
}
 ]]

local frag =
[[
varying vec3 normal;
varying vec4 eyepos;

uniform sampler1D tex1d;

void main()
{
  vec4 color = texture1D(tex1d, gl_TexCoord[0].s);
  float x = abs(dot(normalize(eyepos.xyz), normalize(normal)));
  float atten1 = pow(x, 0.2);
  float atten2 = pow(x, 6.0);

  gl_FragColor.rgb = color.xyz * 0.5*(atten1 + atten2);
  gl_FragColor.a = gl_Color.a;
}
 ]]
surfshd:set_program(vert, frag)

local Nx = 256
local Ny = 256
local A = lunum.zeros{Nx,Ny}

for i,j,k in A:indices() do
   local u = -0.5 + i/Nx
   local v = -0.5 + j/Ny
   A[i*Ny + j] = stingray(u,v)
end

height:set_data(A)
verts:set_input(height)

scalars = verts:get_output("scalars")
scalars:set_normalize(true)

triangles:set_data("triangles", verts:get_output("triangles"))
triangles:set_data("normals", verts:get_output("normals"))
triangles:set_data("color_table", pyluts)
triangles:set_data("scalars", scalars)
triangles:set_shader(surfshd)
triangles:set_alpha(1.0)
triangles:set_color(0.5, 0.5, 1.0)
triangles:set_orientation(-90,0,0)

window:set_color(0.2, 0.2, 0.2)
box:set_color(0.5, 0.9, 0.9)
box:set_shader(lights)
box:set_scale(1.5, 1.5, 1.5)
box:set_linewidth(0.2)

window:set_callback("]", function() pyluts:next_colormap() end, "next colormap")
window:set_callback("[", function() pyluts:prev_colormap() end, "previous colormap")
window:render_scene{triangles}
