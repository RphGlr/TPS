#version 450 core

layout (location = 0) in vec3 aPos; //pos
layout (location = 1) in vec2 bodyTextCoords; //uv
layout (location = 2) in vec3 aNormal; //normal


//V2F Vertex To Fragment
out vec3 FragPos;  
out vec3 Normal;
out vec2 o_textCoords;
uniform mat4 viewProj;
uniform mat4 model;

void main()
{
   FragPos = vec3(model * vec4(aPos, 1.0));
   Normal = mat3(transpose(inverse(model))) * aNormal; 

   gl_Position =  viewProj * model * vec4(aPos, 1.0);
   o_textCoords = bodyTextCoords;
}