#!/usr/bin/env lua


local luview = require 'luview'
local lunum = require 'lunum'
local hdf5 = require 'hdf5'
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

local frag = [[
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

hdf5.open_file(arg[1], "r")
local data = hdf5.read_array("prim/pre")
hdf5.close_file()

height:set_data(data)
verts:set_input(height)

scalars = verts:get_output("scalars")
scalars:set_normalize(true)

function triangles:inc_scale()
   local hx, hy, hz = self:get_scale()
   hy = hy + 0.01
   self:set_scale(hx, hy, hz)
end
function triangles:dec_scale()
   local hx, hy, hz = self:get_scale()
   hy = hy - 0.01
   self:set_scale(hx, hy, hz)
end

triangles:set_data("triangles", verts:get_output("triangles"))
triangles:set_data("normals", verts:get_output("normals"))
triangles:set_data("color_table", pyluts)
triangles:set_data("scalars", scalars)
triangles:set_shader(surfshd)
triangles:set_alpha(1.0)
triangles:set_color(0.3, 0.8, 0.3)
triangles:set_orientation(-90,0,0)
triangles:set_scale(1.0, 0.04, 1.0)
window:set_color(0, 0, 0)
box:set_color(0.5, 0.9, 0.9)
box:set_shader(lights)

pyluts:set_colormap("afmhot")
window:set_callback("]", function() pyluts:next_colormap() end, "next colormap")
window:set_callback("[", function() pyluts:prev_colormap() end, "previous colormap")
window:set_callback("H", function() triangles:inc_scale() end, "increase scale height")
window:set_callback("h", function() triangles:dec_scale() end, "decrease scale height")

window:render_scene{triangles}

