

varying vec3 normal;
varying vec4 eyepos;
varying vec4 vert_color;

uniform sampler1D tex1d;
uniform sampler2D tex2d;

void main()
{
  vec4 t2d = vec4(texture2D(tex2d, (gl_TexCoord[0].st)));
  vec4 color = texture1D(tex1d, t2d.s);
  float atten1 = pow(abs(dot(normalize(eyepos.xyz), normal)), 0.4);
  float atten2 = pow(abs(dot(normalize(eyepos.xyz), normal)), 6.0);

  gl_FragColor.rgb = color.xyz * 0.5*(atten1 + atten2);
  gl_FragColor.a = gl_Color.a;
}
