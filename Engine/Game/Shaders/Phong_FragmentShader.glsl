#version 330

uniform vec3 camera_pos;
uniform vec3 light_dir;
uniform vec4 light_color;
uniform vec4 ambient_color;
uniform float Ks;
uniform float Kd;
uniform int shininess;
uniform sampler2D my_texture;

in vec3 fragment_normal;
in vec3 fragment_pos;
in vec2 uv;

out vec4 outColor;

void main() {
   vec4 diffuse_color = texture(my_texture, uv);
   
   vec3 light_dir = normalize(light_dir);
   vec3 normal = normalize(fragment_normal);
         
   vec4 ambient = ambient_color * diffuse_color;
   
   float NL = max(dot(normal, -light_dir), 0);     
   vec4 diffuse = Kd * diffuse_color * light_color * NL;   
   
   vec3 R = reflect(light_dir, normal); //reflection dir
   vec3 V = normalize(camera_pos - fragment_pos); //view dir
   float VR = max(dot(V, R), 0);
   vec4 specular = Ks * light_color * pow(VR,shininess);   
   
   outColor = ambient + diffuse + specular;
}