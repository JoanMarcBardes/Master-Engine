#version 440

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

in layout (location = 0) vec3 position;
in layout (location = 1) vec3 normal;
in layout (location = 2) vec2 texture_uv;

out vec3 fragment_normal;
out vec3 fragment_pos;
out vec2 uv;

void main() {
	gl_Position = proj * view * model * vec4(position,1);
	fragment_normal = mat3(model) * normal;
	fragment_pos = (model * vec4(position,1)).xyz;
	uv = texture_uv;
}
