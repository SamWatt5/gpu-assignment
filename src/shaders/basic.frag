#version 420

in vec3 N;
in vec4 P;
in vec4 diffuse_colour;

uniform mat4 view;
uniform vec3 light_position;
uniform vec3 beam_light_position;
uniform vec3 beam_light_direction;
uniform vec3 beam_colour;
uniform vec3 emissive;

out vec4 fragColor;

// made the light calculation a function so it can be used for the moon as well
// as the beam light will also make it so that the beam is clamped to a cone
// shape
vec3 light_calculation(vec3 light_pos, vec3 N, vec4 P, vec4 diffuse_colour,
                       mat4 view, vec3 light_colour) {
  vec4 light_viewspace = view * vec4(light_pos, 1.0);
  vec3 L = normalize(light_viewspace.xyz - P.xyz);

  vec4 diffuse = max(0.0, dot(L, N)) * diffuse_colour;

  vec3 V = normalize(-P.xyz);
  vec3 R = reflect(-L, N);
  vec3 specular = pow(max(dot(R, V), 0.0), 8.0) * vec3(1.0, 1.0, 0.6);

  return (diffuse.rgb + specular) * light_colour;
}

void main() {
  vec3 N_norm = normalize(N);

  vec4 ambient = diffuse_colour * 0.1;

  vec3 rgbColour = ambient.rgb + emissive.rgb +
                   light_calculation(light_position, N_norm, P, diffuse_colour,
                                     view, vec3(1.0)) +
                   light_calculation(beam_light_position, N_norm, P,
                                     diffuse_colour, view, beam_colour);

  fragColor = vec4(rgbColour, diffuse_colour.a);
}
