#include <cmath>
#include <glad/glad.h>

#include "Maths.h"
#include "Collider.h"
#include "Application.h"

using namespace Maths;

Collider::Collider(GameObject& go) : Collider(go, std::shared_ptr<Collider>(this))
{
}
Collider::Collider(GameObject& go, std::shared_ptr<Collider> ptr) : Component(go, ptr)
{
    Application::getInstance().physicManager.colliders.push_back(ptr);
}

BoxCollider::BoxCollider(GameObject& go, sCube c) : Collider(go)
{
    cube = c;
}
BoxCollider::BoxCollider(GameObject& go, float3 c) : Collider(go)
{
    cube = { c.x,c.y,c.z };
}

SphereCollider::SphereCollider(GameObject& go, sSphere s) : Collider(go)
{
    sphere = s;
}
SphereCollider::SphereCollider(GameObject& go, float radius) : Collider(go)
{
    sphere.radius = radius;
}

Collision Collider::SphereBoxCollision(SphereCollider s, BoxCollider b)
{
    bool isColliding = false;

    float3 deltaP = s.localPos - b.localPos;

    float3 clamped{};
    clamped.x = Maths::Clamp(deltaP.x, -b.cube.width, b.cube.width);
    clamped.y = Maths::Clamp(deltaP.y, -b.cube.height, b.cube.height);
    clamped.z = Maths::Clamp(deltaP.z, -b.cube.length, b.cube.length);

    float3 closest = b.localPos + clamped;

    deltaP = closest - s.localPos;

    if (Norm(deltaP) <= s.sphere.radius)
        isColliding = true;

    Collision c;

    if (isColliding)
        c = { true, b.localPos };

    return c;

}
Collision Collider::SphereSphereCollision(SphereCollider s1, SphereCollider s2)
{
    bool isColliding = false;
    float3 s1GlobalPos = s1.cgameObject.transform.position + s1.localPos;
    float3 s2GlobalPos = s2.cgameObject.transform.position + s2.localPos;

    float3 deltaC = s1GlobalPos - s2GlobalPos;
    float radiusSum = s1.sphere.radius + s2.sphere.radius;

    if (Norm(deltaC) < radiusSum)
    {
        isColliding = true;
    }

    Collision c;

    float3 inter = s1GlobalPos + Normalize(s2GlobalPos - s1GlobalPos) * s1.sphere.radius;

    if (isColliding)
        c = { true, inter };

    return c;

}
Collision Collider::BoxBoxCollision(BoxCollider b1, BoxCollider b2, float3& p)
{
    bool isColliding = false;

    bool collisionX = ((b1.localPos.x + b1.cube.width) >= b2.localPos.x && (b2.localPos.x + b2.cube.width) >= b1.localPos.x);

    bool collisionY = ((b1.localPos.y + b1.cube.height) >= b2.localPos.y && (b2.localPos.y + b2.cube.height) >= b1.localPos.y);

    bool collisionZ = ((b1.localPos.z + b1.cube.length) >= b2.localPos.z && (b2.localPos.z + b2.cube.height) >= b1.localPos.z);

    p.x = (b1.localPos.x + b1.cube.width) - (b2.localPos.x - b2.cube.width);
    p.y = (b1.localPos.y + b1.cube.height) - (b2.localPos.y - b2.cube.height);
    p.z = (b1.localPos.z + b1.cube.length) - (b2.localPos.z - b2.cube.length);

    isColliding = collisionX && collisionY && collisionZ;

    Collision c;

    if (isColliding)
        c = { true, b2.localPos};

    return c;
}

void BoxCollider::Draw(mat4x4& translate, mat4x4& rotate, mat4x4& scale, mat4x4& model, const GLint& modelLocation, void* resourcesManager)
{
    ResourcesManager* _RMptr = (ResourcesManager*)resourcesManager;

    Model m = _RMptr->GetModel((int)Cube);

    Transform transform; 
    transform.scale = { cube.width,cube.height, cube.length };
    transform.position = this->localPos;

    transform = transform.AddTransform(transform, cgameObject.transform);

    m.UpdateModelMatrice(transform, translate, rotate, scale);

    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m.texture.textureGLLocation);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glUniform1i(glGetUniformLocation(_RMptr->basicShaderProgram, "bodyTextCoords"), m.texture.textureGLLocation);

    model = translate * rotate * scale;
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.e);
    
    glBindVertexArray(m.VAO);
    glDrawArrays(GL_TRIANGLES, 0, m.positions.size());
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
}
void SphereCollider::Draw(mat4x4& translate, mat4x4& rotate, mat4x4& scale, mat4x4& model, const GLint& modelLocation, void* resourcesManager)
{
    ResourcesManager* _RMptr = (ResourcesManager*)resourcesManager;

    Model m = _RMptr->GetModel((int)Sphere);
    Transform transform;

    float sphereScale = 10 * sphere.radius;
    transform.scale = { sphereScale ,sphereScale ,sphereScale };
    transform.position = this->localPos;
    transform = transform.AddTransform(transform, cgameObject.transform);

    m.UpdateModelMatrice(transform, translate, rotate, scale);

    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m.texture.textureGLLocation);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glUniform1i(glGetUniformLocation(_RMptr->basicShaderProgram, "bodyTextCoords"), m.texture.textureGLLocation);

    model = translate * rotate * scale;
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.e);

    glBindVertexArray(m.VAO);
    glDrawArrays(GL_TRIANGLES, 0, m.positions.size());
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
}