#version 450 core

layout (location = 0) in vec3 aPos; //pos
layout (location = 1) in vec2 bodyTextCoords; //uv
layout (location = 2) in vec3 aNormal; //normal


//V2F Vertex To Fragment



out vec3 FragPos;  
out vec3 Normal;
out vec3 smoothNormal;
out vec2 o_textCoords;

flat out float lightAmount;
uniform mat4 ambiantLight; 
uniform vec3 viewPos;

uniform float _time;


struct Light
{
	vec3 position;
	vec4 color;
	vec4 values;
};

Light Mat4ToLight(mat4 mat)
{
	Light l;

	l.position = vec3(mat[0]);
	l.color = mat[1];
	l.values = mat[2];
		
	return l;
}


uniform mat4 viewProj;
uniform mat4 model;

void main()
{
   FragPos = vec3(model * vec4(aPos, 1.0));
   Normal = mat3(transpose(inverse(model))) * aNormal; 
   smoothNormal = Normal;

   
    float heartbeat =clamp(smoothstep(0.9,1., sin(_time*.004)), 0,1);
     heartbeat += clamp(smoothstep(0.9,1., sin((_time+200)*.004)), 0,1);
     heartbeat = clamp(heartbeat,0,1);

   	Light light = Mat4ToLight(transpose(ambiantLight));

	vec3 lightDir = normalize(light.position - FragPos);
   	float diff = max(dot(Normal, lightDir), 0.0);

	lightAmount = diff ;

   gl_Position =  viewProj * model * vec4(aPos +aNormal * heartbeat *.5, 1.0);
   o_textCoords = bodyTextCoords;
}