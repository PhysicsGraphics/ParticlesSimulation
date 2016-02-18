#version 330 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_velocity;
//layout(location = 2) in vec3 in_color;

out vec3 location;
out vec3 velocity;
//out vec3 color;

out vec4 v2f_color;

uniform mat4 MVP;

void main()
{
	const float dt = 0.01f;
	bool reflect = false;
	gl_Position = MVP * vec4(in_position, 1.0f);
	
	float blue = 1.0f -  exp(-length(in_velocity) * 5.0f);
//	float green = 1.0f -  exp(-length(in_velocity) * 20.0f);
//	float red = 1.0f -  exp(-length(in_velocity) * 15.0f);
		
	v2f_color = vec4(blue, 0.5f, 1.0f - blue, 1.0f - blue);
//	v2f_color = vec4(in_color, 1.0f);
	
//	vec3 g = - 0.01f * normalize(in_position) / dot(in_position, in_position);
	vec3 g = - 0.01f * normalize(in_position) / dot(in_position, in_position);
	.z
	
	location = in_position ;//+ dt * in_velocity;
	float a = 1.0f;
	float A = 1.0f;
	float B = 1.0f;
	float n = A * 1.0f / ((a*a + x*x + y*y + z*z) * (a*a + x*x + y*y + z*z));
	velocity.x =  0.0f;//0.001f * n * 2.0f * (-a * in_position.y + in_position.x * in_position.z);
	velocity.y =  0.0f;//0.001f * n * 2.0f * (a * in_position.x + in_position.y * in_position.z);
	velocity.z = 0.0f;//0.001f * n * (a*a - x*x - y*y + z*z);
	
//	color = in_color;
}