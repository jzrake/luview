

varying vec3 normal;
varying vec4 pos;

uniform sampler1D tex1d;
uniform sampler2D tex2d;

void main()
{
  vec4 t2d = vec4(texture2D(tex2d, (gl_TexCoord[0].st)));
  vec4 color = texture1D(tex1d, t2d.x);

  vec4 s = -normalize(pos - gl_LightSource[0].position);
  vec3 n = normalize(normal);

  vec4 diffuse = color * max(0.0, dot(n, s.xyz)) * gl_LightSource[0].diffuse;

  float w = 0.5;
  gl_FragColor = w*diffuse + (1.0 - w) * color;
}
