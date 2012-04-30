
varying vec3 normal;
varying vec3 vertex_to_light_vector;

void main()
{
  // Defining The Material Colors
  vec4 AmbientColor = vec4(0.4, 0.4, 0.4, 0.2);
  vec4 DiffuseColor = vec4(0.7, 0.0, 0.0, 0.7);

  // Scaling The Input Vector To Length 1
  vec3 normalized_normal = normalize(normal);
  vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);

  // Calculating The Diffuse Term And Clamping It To [0;1]
  float DiffuseTerm = clamp(dot(normal, vertex_to_light_vector), 0.0, 1.0);

  // Calculating The Final Color
  //  gl_FragColor = vec4(0.0, 0.5, 0.0, 0.1);
  gl_FragColor = AmbientColor + DiffuseColor * DiffuseTerm;
}
