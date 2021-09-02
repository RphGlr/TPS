#version 450 core

layout (location = 0) in vec3 aPos; //pos
layout (location = 1) in vec2 bodyTextCoords; //uv
layout (location = 2) in vec3 aNormal; //normal


//V2F Vertex To Fragment


out VS_OUT {
 vec3 FragPos;  
 vec3 Normal;
 vec3 smoothNormal;
 vec2 o_textCoords;
} vs_out;


flat out float lightAmount;
uniform mat4 ambiantLight; 

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
uniform vec3 viewPos;
uniform mat4 model;

void main()
{
   vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
   vs_out.Normal = mat3(transpose(inverse(model))) * aNormal; 
   vs_out.smoothNormal = vs_out.Normal;
   
    float heartbeat =clamp(smoothstep(0.9,1., sin(_time*.004)), 0,1);
     heartbeat += clamp(smoothstep(0.9,1., sin((_time+200)*.004)), 0,1);
     heartbeat = clamp(heartbeat,0,1);

	float s = abs(sin( _time *.001 + vs_out.FragPos.y));


   	Light light = Mat4ToLight(transpose(ambiantLight));

	vec3 lightDir = normalize(light.position - vs_out.FragPos);
   	float diff = max(dot(vs_out.Normal, lightDir), 0.0);

	lightAmount = diff ;

   gl_Position =  vec4(aPos *.5, 1.0);
   vs_out.o_textCoords = bodyTextCoords;
}