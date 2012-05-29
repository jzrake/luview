
varying vec3 normal;
varying vec4 eyepos;

uniform sampler1D tex1d;

void main()
{
  vec4 color = texture1D(tex1d, gl_TexCoord[0].s);
  float x = abs(dot(normalize(eyepos.xyz), normalize(normal)));
  float atten1 = pow(x, 1.0);
  float atten2 = pow(x, 6.0);

  gl_FragColor.rgb = color.xyz * 0.5*(atten1 + atten2);
  gl_FragColor.a = gl_Color.a;
}
