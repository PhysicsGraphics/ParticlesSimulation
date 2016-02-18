#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_velocity;
//layout(location = 2) in vec3 in_color;

out vec3 location;
out vec3 velocity;
//out vec3 color;

out vec4 v2f_color;

uniform mat4 MVP;
uniform vec3 attractor;

void main()
{
const float dt = 0.01f;
bool reflect = false;
gl_Position = MVP * vec4(in_position, 1.0f);

float blue = 1.0f - exp(-length(in_velocity) * 3.0f);

v2f_color = vec4(blue, 0.5f, 1.0f - blue, 1.0f); //- blue);

vec3 dir1 = attractor - in_position;
vec3 dir2 = -attractor - in_position;

dir1 = 0.1f * normalize(dir1) * min(0.1f, 1.0f / dot(dir1, dir1));
dir2 = 0.1f * normalize(dir2) * min(0.1f, 1.0f / dot(dir2, dir2));

location = in_position + dt * in_velocity;
	float a = 1.0f;
	float A = 1.0f;
	float B = 1.0f;
	float n = A * 1.0f / ((a*a + in_position.x*in_position.x + in_position.y*in_position.y + in_position.z*in_position.z) * (a*a + in_position.x*in_position.x + in_position.y*in_position.y + in_position.z*in_position.z));
	velocity.x =  32.0f * n * 2.0f * (-a * in_position.y + in_position.x * in_position.z);
	velocity.y = 32.0f * n * 2.0f * (a * in_position.x + in_position.y * in_position.z);
	velocity.z = 32.0f * n * (a*a - in_position.x*in_position.x - in_position.y*in_position.y + in_position.z*in_position.z);
//float sigma = 10.0;
//float beta = 8.0/3.0;
//float ro = 28.0;
//velocity.x =  0.3f * sigma * ( in_position.y - in_position.x);
//velocity.y =  0.3f * ( in_position.x *( ro - in_position.z) - in_position.y);
//velocity.z =  0.3f *  ( in_position.y * in_position.x - beta * in_position.z);

//velocity.x =  1.0f * ( -in_position.y - in_position.z);
//velocity.y =  1.0f * ( in_position.x + 0.2f * in_position.y);
//velocity.z =  1.0f *  ( 0.2 + in_position.z * (in_position.x - 14.0f));


// color = in_color;
}
