#include <glad/glad.h>
#include "Application.h"
#include "RenderingResources.h"



using namespace Maths;

RenderingResources::RenderingResources(GameObject& gameObject, Model model, Material material, Transform transform) : Component(gameObject, std::shared_ptr<RenderingResources>(this))
{
	this->model = model;
	this->material = material;
	this->transform = transform;
	 t1 = high_resolution_clock::now();
}

void RenderingResources::Draw(mat4x4& translate, mat4x4& rotate, mat4x4& scale, mat4x4& model)
{
	Renderer& renderer = Application::getInstance().renderer;

	Transform t = t.AddTransform(transform, cgameObject.transform);

	this->model.UpdateModelMatrice(t, translate, rotate, scale);

	glUseProgram(material.shader.shaderId);
	renderer.GetShaderUniformLocations(material.shader.shaderId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->model.texture.textureGLLocation);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	 if (material.shader.shaderId == ShaderEnum::BrickShaderProgram || material.shader.shaderId == ShaderEnum::PlayerShaderProgram || material.shader.shaderId == ShaderEnum::EnemyShaderProgram )
	{
		int timeLocation = glGetUniformLocation(material.shader.shaderId, "_time");
		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<float, std::milli> time_span = t2 - t1;
		float d = time_span.count();

		glUniform1f(timeLocation, d);
	}


	 if (material.shader.shaderId == ShaderEnum::TerrainShaderProgram || material.shader.shaderId == ShaderEnum::PlayerShaderProgram || material.shader.shaderId == ShaderEnum::BrickShaderProgram || material.shader.shaderId == ShaderEnum::EnemyShaderProgram)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Application::getInstance().sceneManager.trailTexture->textureGLLocation);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Application::getInstance().sceneManager.fieldOfViewTexture->textureGLLocation);
			
		glUniform1i(glGetUniformLocation(material.shader.shaderId, "trailTexture"), 1);
		glUniform1i(glGetUniformLocation(material.shader.shaderId, "fovTexture"), 2);

	}

	
	unsigned int uvLocation = glGetUniformLocation(material.shader.shaderId, "bodyTextCoords");
	glUniform1i(uvLocation, this->model.texture.textureGLLocation);

	model = translate * rotate * scale;
	glUniformMatrix4fv(renderer.modelLocation, 1, GL_FALSE, model.e);
	glBindVertexArray(this->model.VAO);

	glDrawArrays(GL_TRIANGLES, 0, this->model.positions.size());
	glBindVertexArray(0);
}
