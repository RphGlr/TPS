#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <new>
#include <chrono>

#include <fstream>
#include <sstream>
#include <iostream>

#include "Model.h"
#include "Texture.h"
#include "Maths.h"

using namespace Maths;
using namespace std::chrono;
using namespace std;

struct ArraySizeBuffer {
	int vertexSize;
	int normalSize;
	int uvSize;
};

void parseStringIntoArray(std::string l, float* array, const int tokenSize = 0, std::string delimiter = " ")
{
	std::string token;
	int index = 0;
	size_t pos = 0;
	while (true)
	{
		pos = l.find(delimiter);

		token = l.substr(0, pos);
		l.erase(0, pos + delimiter.length());
		bool notANumber;
		if (token[0] == '-')
			notANumber = !(token[1] >= '0' && token[1] < ':') || (token == "/" || token == " ");
		else
			notANumber = !(token[0] >= '0' && token[0] < ':') || (token == "/" || token == " ");

		if (notANumber)
			continue;

		array[index] = std::stof(token);
		index++;
		if ((pos == std::string::npos))
			break;
	}
}
void parseStringIntoArray(std::string l, int* array, const int tokenSize = 0, std::string delimiter = " ")
{
	std::string token;
	int index = 0;
	size_t pos = 0;
	while (true)
	{
		pos = l.find(delimiter);
		token = l.substr(0, pos);
		l.erase(0, pos + delimiter.length());
		if (token.size() < tokenSize || token == "/" || token == " ")
			continue;
		array[index] = std::stof(token);
		index++;
		if ((pos == std::string::npos))
			break;
	}
}
void parseStringIntoStringArray(std::string l, std::string* array, std::string delimiter = " ")
{
	std::string token;
	int index = 0;
	size_t pos = 0;
	int stringSize = l.length();

	while (true)
	{
		pos = l.find(delimiter);
		//if (pos > stringSize)
		//	break;

		token = l.substr(0, pos);
		l.erase(0, pos + delimiter.length());

		if (token.size() > 2)
		{
			array[index] = token;
			index++;
		}

		if ((pos == std::string::npos))
			break;
	}
}

void StringToStringList(std::string input, std::list<std::string>& list, std::string delimiter = " ", bool countSize = false)
{
	std::string token;
	size_t pos = 0;
	int stringSize = input.size();

	while (true)
	{
		pos = input.find(delimiter);

		token = input.substr(0, pos);

		input.erase(0, pos + delimiter.length());

		list.push_back(token);

		if (pos > stringSize)
			break;
	}
}

void structToPositionList(std::list<triangle>& faces, std::vector<float>& list)
{
	std::list<triangle>::iterator faceIt = faces.begin();;

	// for each faces
	for (int i = 0; i < faces.size(); i++)
	{
		triangle current_face = faceIt._Ptr->_Myval;

		//for each vertex
		for (int k = 0; k < 3; k++)
		{
			list.push_back(current_face.vertices[k].x);
			list.push_back(current_face.vertices[k].y);
			list.push_back(current_face.vertices[k].z);
		}

		std::advance(faceIt, 1);
	}
}
void structToUVList(std::list<triangle>& faces, std::vector<float>& list)
{
	std::list<triangle>::iterator faceIt = faces.begin();;

	// for each faces
	for (int i = 0; i < faces.size(); i++)
	{
		triangle current_face = faceIt._Ptr->_Myval;

		//for each vertex
		for (int k = 0; k < 3; k++)
		{
			list.push_back(current_face.vertices[k].u);
			list.push_back(current_face.vertices[k].v);
		}

		std::advance(faceIt, 1);
	}
}
void structToNormalsList(std::list<triangle>& faces, std::vector<float>& list)
{
	std::list<triangle>::iterator faceIt = faces.begin();;

	// for each faces
	for (int i = 0; i < faces.size(); i++)
	{
		triangle current_face = faceIt._Ptr->_Myval;

		//for each vertex
		for (int k = 0; k < 3; k++)
		{
			list.push_back(current_face.vertices[k].nx);
			list.push_back(current_face.vertices[k].ny);
			list.push_back(current_face.vertices[k].nz);
		}

		std::advance(faceIt, 1);
	}
}


void Model::HandleVao()
{
	GLuint VBOs[3];

	glGenBuffers(1, &VBOs[0]); //positions
	glGenBuffers(1, &VBOs[1]); //UVs
	glGenBuffers(1, &VBOs[2]); //Normals

	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);

	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

Model::Model(const char* objPath, Texture texture, float3 position, int iD)
{
	PopulateTrianglesArray(objPath);
	FillInfoArrays();
	HandleVao();

	transform.position = position;
	this->iD = iD;

	this->texture = texture;
}
Model::Model(Model m, Texture texture, float3 position, int iD)
{
	triangles = m.triangles;
	positions = m.positions;
	uvs = m.uvs;
	normals = m.normals;

	this->VAO = VAO;
	subMeshes = m.subMeshes;
	this->iD = iD;
	transform.position = position;
	this->texture = texture;
}

struct MeshData
{
	float3 vertex;
	float3 normal;
	float2 uv;
};

void Model::UpdateModelMatrice(Transform transform, mat4x4& translate, mat4x4& rotate, mat4x4& scale)
{
	float3 translation = { transform.position.x, transform.position.y, transform.position.z };
	translate = Maths::Translate(translation);
	rotate = rotateX(transform.rotation.x) * rotateY(transform.rotation.y) * rotateZ(transform.rotation.z);
	scale = Scale(transform.scale);
}
void Model::PopulateTrianglesArray(const char* path)
{
	auto start = high_resolution_clock::now();

	Mesh current_mesh;

	std::vector<float3> vertices;
	std::vector<float2> uvs;
	std::vector<float3> normals;

	ifstream fichier(path, ios::in);
	string sInfos[3];

	float3 current_vertice = { 0,0,0 };
	float2 current_uv = { 0,0 };
	float3 current_normal = { 0,0,0 };

	std::stringstream sstream;
	sstream << fichier.rdbuf();
	string file = sstream.str();

	std::list<string> lines;

	StringToStringList(file, lines, "\n", true);

	for (string ligne : lines)
	{

		switch (ligne[0])
		{
		case 'v':
		{
			if (ligne[1] == 't')
			{
				float current_uv[3];
				parseStringIntoArray(ligne, &current_uv[0], 3);
				uvs.push_back(float2{ current_uv[0],current_uv[1] });
			}
			else if (ligne[1] == 'n')
			{
				float current_normal[3];
				parseStringIntoArray(ligne, &current_normal[0], 3);
				normals.push_back(float3{ current_normal[0],current_normal[1],current_normal[2] });
			}
			else
			{
				float current_vertice[3];
				parseStringIntoArray(ligne, &current_vertice[0], 3);
				vertices.push_back(float3{ current_vertice[0],current_vertice[1],current_vertice[2] });
			}
			break;
		}
		case 'f':
		{
			rdrVertex current_vertices[3];
			float fInfos[3];

			parseStringIntoStringArray(ligne, sInfos);

			//for each separate group indexes
			for (int i = 0; i < 3; i++)
			{
				parseStringIntoArray(sInfos[i], fInfos, 1, "/");

				current_vertice = vertices[fInfos[0] - 1];
				current_uv = uvs[fInfos[1] - 1];
				current_normal = normals[fInfos[2] - 1];

				current_vertices[i] = {
						current_vertice.x, current_vertice.y, current_vertice.z,
						current_normal.x,	current_normal.y, current_normal.z,
						current_uv.x,			current_uv.y
				};
			}

			triangles.push_back({ current_vertices[0],current_vertices[1],current_vertices[2] });
			current_mesh.triangles.push_back({ current_vertices[0],current_vertices[1],current_vertices[2] });

			break;
		}
		//new mesh
		case 'o':
			if (current_mesh.triangles.size() == 0)
				break;
			subMeshes.push_back(current_mesh);
			current_mesh.triangles.clear();
			break;

		default:
			break;
		}
	}

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);

	std::cout << "Done in : " << duration.count() / (1000.f * 1000.f) << "s" << endl;

}
void Model::FillInfoArrays()
{
	std::list<triangle>::iterator faceIt = triangles.begin();;

	// for each faces
	for (int i = 0; i < triangles.size(); i++)
	{
		triangle current_face = faceIt._Ptr->_Myval;

		//for each vertex
		for (int k = 0; k < 3; k++)
		{
			positions.push_back(current_face.vertices[k].x);
			positions.push_back(current_face.vertices[k].y);
			positions.push_back(current_face.vertices[k].z);

			uvs.push_back(current_face.vertices[k].u);
			uvs.push_back(current_face.vertices[k].v);

			normals.push_back(current_face.vertices[k].nx);
			normals.push_back(current_face.vertices[k].ny);
			normals.push_back(current_face.vertices[k].nz);
		}

		std::advance(faceIt, 1);
	}

}