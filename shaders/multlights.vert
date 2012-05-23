
varying vec3 v_V;
varying vec3 v_N;

void main()
{
  v_V = (gl_ModelViewMatrix * gl_Vertex).xyz;
  v_N = gl_NormalMatrix * gl_Normal;
  gl_FrontColor = gl_Color;
  gl_BackColor = gl_Color;
  gl_Position = ftransform();
}
