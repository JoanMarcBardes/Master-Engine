#version 440

uniform vec3 camera_pos;
uniform vec3 light_dir;
uniform vec3 light_color;

uniform vec3 ambient_color;
uniform int shininess;
uniform vec3 diffuse_color;
layout (binding = 0) uniform sampler2D diffuse_map;
uniform vec3 specular_color;
layout (binding = 1) uniform sampler2D specular_map;
uniform int has_diffuse_map;
uniform int has_specular_map;
uniform int shininess_alpha;

in vec3 fragment_normal;
in vec3 fragment_pos;
in vec2 uv;

out vec4 outColor;

void main() {
   // uv = vec2(uv.x, 1.0-uv.y) for flip texture
   vec3 Cd = pow(texture(diffuse_map, uv).rgb, vec3(2.2)) * has_diffuse_map + diffuse_color * (1-has_diffuse_map); //to Linear space
   vec3 Cs = pow(texture(specular_map, uv).rgb, vec3(2.2)) * has_specular_map + specular_color * (1-has_specular_map); //to Linear space
   float n = pow(texture(specular_map, uv).a*7+1,2) * shininess_alpha + shininess * (1-shininess_alpha);
      
   vec3 light_dir = normalize(light_dir);
   vec3 normal = normalize(fragment_normal);
         
   vec3 ambient = ambient_color * Cd;
   
   float NL = max(dot(normal, -light_dir), 0);  
   
   vec3 R = reflect(light_dir, normal); //reflection dir
   vec3 V = normalize(camera_pos - fragment_pos); //view dir
   float VR = max(dot(V, R), 0);
   
   //Normalised Phong BRDF with Fresnel
   vec3 aux1 = Cd * (1 - Cs);
      
   float a = (n + 2)/2;
   vec3 SchlickApprox = Cs + (1-Cs) * pow(1-NL,5);
   vec3 aux2 = a * SchlickApprox * pow(VR,n); 
   
   outColor.rgb = ambient + (aux1 + aux2) * light_color * NL;
   outColor.rgb = pow(outColor.rgb, vec3(1/2.2)); //Gamma correction   
}