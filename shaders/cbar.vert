

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
